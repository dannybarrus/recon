# ─── EKS: LOCALS AND DATA SOURCES ────────────────────────────────────────────
#
# Analog to 04_AKS/02_locals.tf

data "aws_caller_identity" "current" {}

locals {
  cluster_name = var.cluster_name != "" \
    ? var.cluster_name \
    : "eks-recon-${var.environment}"

  is_prod = var.environment == "prod"

  common_tags = merge(
    {
      environment  = var.environment
      managed_by   = "terraform"
      cluster_name = local.cluster_name
    },
    var.tags
  )

  # EKS requires specific tags on subnets -- include cluster name
  cluster_tags = {
    "kubernetes.io/cluster/${local.cluster_name}" = "owned"
  }
}
