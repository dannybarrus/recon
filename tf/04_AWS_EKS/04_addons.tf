# ─── EKS: ADD-ONS AND SUPPORTING RESOURCES ───────────────────────────────────
#
# Analog to 04_AKS/04_supporting.tf
#
# EKS add-ons are managed components that AWS installs and upgrades.
# Analog to AKS add-ons like oms_agent and azure_policy.

# ─── Core add-ons ────────────────────────────────────────────────────────────

resource "aws_eks_addon" "vpc_cni" {
  cluster_name             = aws_eks_cluster.main.name
  addon_name               = "vpc-cni"
  resolve_conflicts_on_update = "OVERWRITE"
  tags                     = local.common_tags
}

resource "aws_eks_addon" "coredns" {
  cluster_name             = aws_eks_cluster.main.name
  addon_name               = "coredns"
  resolve_conflicts_on_update = "OVERWRITE"
  tags                     = local.common_tags

  depends_on = [aws_eks_node_group.system]
}

resource "aws_eks_addon" "kube_proxy" {
  cluster_name             = aws_eks_cluster.main.name
  addon_name               = "kube-proxy"
  resolve_conflicts_on_update = "OVERWRITE"
  tags                     = local.common_tags
}

resource "aws_eks_addon" "ebs_csi" {
  cluster_name             = aws_eks_cluster.main.name
  addon_name               = "aws-ebs-csi-driver"
  resolve_conflicts_on_update = "OVERWRITE"
  tags                     = local.common_tags
}

# ─── CloudWatch Container Insights ────────────────────────────────────────────
# Analog to AKS diagnostic settings shipping to Log Analytics.

resource "aws_cloudwatch_log_group" "eks" {
  name              = "/aws/eks/${local.cluster_name}/cluster"
  retention_in_days = local.is_prod ? 90 : 30
  tags              = local.common_tags
}

# ─── Cluster Autoscaler IAM ───────────────────────────────────────────────────
# IRSA: gives the cluster autoscaler pod permission to adjust node groups.
# Analog to AKS's built-in autoscaler -- AWS requires explicit IAM wiring.

data "aws_iam_policy_document" "cluster_autoscaler_assume" {
  statement {
    effect  = "Allow"
    actions = ["sts:AssumeRoleWithWebIdentity"]

    principals {
      type        = "Federated"
      identifiers = [aws_iam_openid_connect_provider.eks.arn]
    }

    condition {
      test     = "StringEquals"
      variable = "${replace(aws_iam_openid_connect_provider.eks.url, "https://", "")}:sub"
      values   = ["system:serviceaccount:kube-system:cluster-autoscaler"]
    }
  }
}

data "aws_iam_policy_document" "cluster_autoscaler" {
  statement {
    effect = "Allow"
    actions = [
      "autoscaling:DescribeAutoScalingGroups",
      "autoscaling:DescribeAutoScalingInstances",
      "autoscaling:DescribeLaunchConfigurations",
      "autoscaling:DescribeScalingActivities",
      "autoscaling:DescribeTags",
      "ec2:DescribeInstanceTypes",
      "ec2:DescribeLaunchTemplateVersions",
      "autoscaling:SetDesiredCapacity",
      "autoscaling:TerminateInstanceInAutoScalingGroup",
    ]
    resources = ["*"]
  }
}

resource "aws_iam_role" "cluster_autoscaler" {
  name               = "role-cluster-autoscaler-${local.cluster_name}"
  assume_role_policy = data.aws_iam_policy_document.cluster_autoscaler_assume.json
  tags               = local.common_tags
}

resource "aws_iam_role_policy" "cluster_autoscaler" {
  name   = "cluster-autoscaler"
  role   = aws_iam_role.cluster_autoscaler.id
  policy = data.aws_iam_policy_document.cluster_autoscaler.json
}
