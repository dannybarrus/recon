# ─── EKS: CLUSTER AND NODE GROUPS ────────────────────────────────────────────
#
# Analog to 04_AKS/03_cluster.tf
#
# Variables, locals, and data sources defined in 01_variables.tf and
# 02_locals.tf are referenced directly -- same directory, same namespace.

# ─── Security Group: cluster ──────────────────────────────────────────────────
# Analog to Azure NSG.
# Controls traffic to and from the EKS API server.

resource "aws_security_group" "cluster" {
  name        = "sg-eks-cluster-${local.cluster_name}"
  description = "EKS cluster control plane security group"
  vpc_id      = var.vpc_id
  tags        = merge(local.common_tags, { Name = "sg-eks-cluster-${local.cluster_name}" })
}

resource "aws_security_group" "nodes" {
  name        = "sg-eks-nodes-${local.cluster_name}"
  description = "EKS node group security group"
  vpc_id      = var.vpc_id
  tags        = merge(local.common_tags, { Name = "sg-eks-nodes-${local.cluster_name}" })
}

# Allow nodes to communicate with each other
resource "aws_security_group_rule" "nodes_internal" {
  type                     = "ingress"
  from_port                = 0
  to_port                  = 65535
  protocol                 = "-1"
  security_group_id        = aws_security_group.nodes.id
  source_security_group_id = aws_security_group.nodes.id
  description              = "Allow all intra-node communication"
}

# Allow control plane to communicate with nodes
resource "aws_security_group_rule" "cluster_to_nodes" {
  type                     = "ingress"
  from_port                = 1025
  to_port                  = 65535
  protocol                 = "tcp"
  security_group_id        = aws_security_group.nodes.id
  source_security_group_id = aws_security_group.cluster.id
  description              = "Allow control plane to reach nodes"
}

# ─── EKS Cluster ─────────────────────────────────────────────────────────────

resource "aws_eks_cluster" "main" {
  name     = local.cluster_name
  role_arn = var.cluster_role_arn
  version  = var.kubernetes_version

  vpc_config {
    subnet_ids              = concat(var.private_subnet_ids, var.public_subnet_ids)
    security_group_ids      = [aws_security_group.cluster.id]
    endpoint_private_access = var.endpoint_private_access
    endpoint_public_access  = var.endpoint_public_access
    public_access_cidrs     = var.endpoint_public_access ? var.public_access_cidrs : []
  }

  # Analog to AKS control plane log shipping to Log Analytics
  enabled_cluster_log_types = var.enable_cloudwatch_logging ? var.cluster_log_types : []

  # Encrypt Kubernetes secrets at rest using KMS
  encryption_config {
    resources = ["secrets"]
    provider {
      key_arn = aws_kms_key.eks.arn
    }
  }

  # Enable OIDC provider -- required for IRSA (IAM Roles for Service Accounts)
  # Analog to AKS workload identity / OIDC issuer
  tags = local.common_tags

  depends_on = [aws_kms_key.eks]
}

# ─── OIDC Provider ────────────────────────────────────────────────────────────
# Required for IRSA -- allows pods to assume IAM roles without node-level credentials.
# Analog to AKS workload identity federation.

data "tls_certificate" "eks" {
  url = aws_eks_cluster.main.identity[0].oidc[0].issuer
}

resource "aws_iam_openid_connect_provider" "eks" {
  client_id_list  = ["sts.amazonaws.com"]
  thumbprint_list = [data.tls_certificate.eks.certificates[0].sha1_fingerprint]
  url             = aws_eks_cluster.main.identity[0].oidc[0].issuer
  tags            = local.common_tags
}

# ─── KMS Key ─────────────────────────────────────────────────────────────────
# Encrypts Kubernetes secrets at rest.
# Analog to Azure Key Vault CMK for AKS.

resource "aws_kms_key" "eks" {
  description             = "EKS secrets encryption - ${local.cluster_name}"
  deletion_window_in_days = 30
  enable_key_rotation     = true
  tags                    = local.common_tags
}

resource "aws_kms_alias" "eks" {
  name          = "alias/eks-${local.cluster_name}"
  target_key_id = aws_kms_key.eks.key_id
}

# ─── System Node Group ────────────────────────────────────────────────────────
# Analog to AKS system node pool.
# Runs cluster-critical add-ons: CoreDNS, kube-proxy, VPC CNI.

resource "aws_eks_node_group" "system" {
  cluster_name    = aws_eks_cluster.main.name
  node_group_name = "system"
  node_role_arn   = var.node_role_arn
  subnet_ids      = var.private_subnet_ids
  instance_types  = var.system_node_group.instance_types
  disk_size       = var.system_node_group.disk_size
  capacity_type   = "ON_DEMAND"   # system nodes always on-demand

  scaling_config {
    desired_size = var.system_node_group.desired_size
    min_size     = var.system_node_group.min_size
    max_size     = var.system_node_group.max_size
  }

  update_config {
    max_unavailable_percentage = 33   # analog to AKS max_surge = "33%"
  }

  # Taint system nodes to prevent user workloads scheduling here
  # Analog to AKS only_critical_addons_enabled
  taint {
    key    = "CriticalAddonsOnly"
    value  = "true"
    effect = "NO_SCHEDULE"
  }

  labels = {
    "nodegroup-type" = "system"
    "environment"    = var.environment
  }

  tags = local.common_tags

  lifecycle {
    ignore_changes = [scaling_config[0].desired_size]   # managed by cluster autoscaler
  }
}

# ─── User Node Groups ─────────────────────────────────────────────────────────
# Analog to AKS user node pools.
# for_each over the map creates one node group per entry.

resource "aws_eks_node_group" "user" {
  for_each = var.user_node_groups

  cluster_name    = aws_eks_cluster.main.name
  node_group_name = each.key
  node_role_arn   = var.node_role_arn
  subnet_ids      = var.private_subnet_ids
  instance_types  = each.value.instance_types
  disk_size       = each.value.disk_size
  capacity_type   = each.value.capacity_type

  scaling_config {
    desired_size = each.value.desired_size
    min_size     = each.value.min_size
    max_size     = each.value.max_size
  }

  update_config {
    max_unavailable_percentage = 33
  }

  dynamic "taint" {
    for_each = each.value.taints
    content {
      key    = taint.value.key
      value  = taint.value.value
      effect = taint.value.effect
    }
  }

  labels = merge(each.value.labels, {
    "nodegroup-type" = each.key
    "environment"    = var.environment
  })

  tags = merge(local.common_tags, {
    "k8s.io/cluster-autoscaler/${local.cluster_name}" = "owned"
    "k8s.io/cluster-autoscaler/enabled"               = "true"
  })

  lifecycle {
    ignore_changes = [scaling_config[0].desired_size]
  }
}
