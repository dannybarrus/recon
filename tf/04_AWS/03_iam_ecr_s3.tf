# ─── AWS: IAM, ECR, S3 ───────────────────────────────────────────────────────
#
# AWS analog to 04_Azure/04_keyvault_acr_storage.tf
#
# Azure concept  -> AWS concept
# Key Vault      -> AWS Secrets Manager / Parameter Store / KMS
# ACR            -> ECR (Elastic Container Registry)
# Storage Account-> S3 (Simple Storage Service)
# Managed Identity -> IAM Role with trust policy

data "aws_caller_identity" "current" {}
data "aws_region" "current"          {}

# ─── IAM: EKS cluster role ────────────────────────────────────────────────────
# Analog to AKS SystemAssigned managed identity.
# AWS requires explicit IAM roles for EKS -- no auto-assignment.

data "aws_iam_policy_document" "eks_assume_role" {
  statement {
    effect  = "Allow"
    actions = ["sts:AssumeRole"]
    principals {
      type        = "Service"
      identifiers = ["eks.amazonaws.com"]
    }
  }
}

resource "aws_iam_role" "eks_cluster" {
  name               = "role-eks-cluster-recon-${var.environment}"
  assume_role_policy = data.aws_iam_policy_document.eks_assume_role.json
  tags               = { environment = var.environment, managed_by = "terraform" }
}

resource "aws_iam_role_policy_attachment" "eks_cluster_policy" {
  role       = aws_iam_role.eks_cluster.name
  policy_arn = "arn:aws:iam::aws:policy/AmazonEKSClusterPolicy"
}

# ─── IAM: EKS node group role ────────────────────────────────────────────────
# Analog to AKS kubelet managed identity.

data "aws_iam_policy_document" "eks_node_assume_role" {
  statement {
    effect  = "Allow"
    actions = ["sts:AssumeRole"]
    principals {
      type        = "Service"
      identifiers = ["ec2.amazonaws.com"]
    }
  }
}

resource "aws_iam_role" "eks_node" {
  name               = "role-eks-node-recon-${var.environment}"
  assume_role_policy = data.aws_iam_policy_document.eks_node_assume_role.json
  tags               = { environment = var.environment, managed_by = "terraform" }
}

resource "aws_iam_role_policy_attachment" "eks_worker_node" {
  role       = aws_iam_role.eks_node.name
  policy_arn = "arn:aws:iam::aws:policy/AmazonEKSWorkerNodePolicy"
}

resource "aws_iam_role_policy_attachment" "eks_cni" {
  role       = aws_iam_role.eks_node.name
  policy_arn = "arn:aws:iam::aws:policy/AmazonEKS_CNI_Policy"
}

resource "aws_iam_role_policy_attachment" "eks_ecr_read" {
  role       = aws_iam_role.eks_node.name
  policy_arn = "arn:aws:iam::aws:policy/AmazonEC2ContainerRegistryReadOnly"
}

# ─── ECR: Container Registry ──────────────────────────────────────────────────
# Analog to Azure Container Registry (ACR).

resource "aws_ecr_repository" "main" {
  name                 = "recon-${var.environment}"
  image_tag_mutability = "IMMUTABLE"   # prevents tag overwriting in prod

  image_scanning_configuration {
    scan_on_push = true   # analog to ACR's vulnerability scanning
  }

  encryption_configuration {
    encryption_type = "KMS"
  }

  tags = { environment = var.environment, managed_by = "terraform" }
}

resource "aws_ecr_lifecycle_policy" "main" {
  repository = aws_ecr_repository.main.name

  policy = jsonencode({
    rules = [{
      rulePriority = 1
      description  = "Keep last 30 images"
      selection = {
        tagStatus   = "any"
        countType   = "imageCountMoreThan"
        countNumber = 30
      }
      action = { type = "expire" }
    }]
  })
}

# ─── S3: Object Storage ───────────────────────────────────────────────────────
# Analog to Azure Storage Account with blob storage.

resource "aws_s3_bucket" "main" {
  bucket = "recon-${var.environment}-${data.aws_caller_identity.current.account_id}"
  tags   = { environment = var.environment, managed_by = "terraform" }
}

resource "aws_s3_bucket_versioning" "main" {
  bucket = aws_s3_bucket.main.id
  versioning_configuration { status = "Enabled" }
}

resource "aws_s3_bucket_server_side_encryption_configuration" "main" {
  bucket = aws_s3_bucket.main.id
  rule {
    apply_server_side_encryption_by_default {
      sse_algorithm = "aws:kms"
    }
  }
}

resource "aws_s3_bucket_public_access_block" "main" {
  bucket                  = aws_s3_bucket.main.id
  block_public_acls       = true
  block_public_policy     = true
  ignore_public_acls      = true
  restrict_public_buckets = true
}

# ─── Secrets Manager ─────────────────────────────────────────────────────────
# Analog to Azure Key Vault for secrets.

resource "aws_secretsmanager_secret" "db_password" {
  name                    = "recon/${var.environment}/db-password"
  recovery_window_in_days = 30
  tags                    = { environment = var.environment, managed_by = "terraform" }
}

# ─── Outputs ─────────────────────────────────────────────────────────────────

output "eks_cluster_role_arn" { value = aws_iam_role.eks_cluster.arn }
output "eks_node_role_arn"    { value = aws_iam_role.eks_node.arn    }
output "ecr_repository_url"   { value = aws_ecr_repository.main.repository_url }
output "s3_bucket_id"         { value = aws_s3_bucket.main.id        }
