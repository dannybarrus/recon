# ─── EKS: OUTPUTS ────────────────────────────────────────────────────────────
#
# Analog to 04_AKS/05_outputs.tf

output "cluster_id" {
  value       = aws_eks_cluster.main.id
  description = "EKS cluster ID"
}

output "cluster_name" {
  value       = aws_eks_cluster.main.name
  description = "EKS cluster name"
}

output "cluster_endpoint" {
  value       = aws_eks_cluster.main.endpoint
  description = "EKS API server endpoint"
}

output "cluster_certificate_authority" {
  value       = aws_eks_cluster.main.certificate_authority[0].data
  description = "Base64 encoded certificate authority data"
  sensitive   = true
}

output "cluster_version" {
  value       = aws_eks_cluster.main.version
  description = "Kubernetes version running on the cluster"
}

output "oidc_provider_arn" {
  value       = aws_iam_openid_connect_provider.eks.arn
  description = "OIDC provider ARN -- used for IRSA role trust policies"
}

output "oidc_provider_url" {
  value       = aws_iam_openid_connect_provider.eks.url
  description = "OIDC provider URL"
}

output "cluster_security_group_id" {
  value       = aws_security_group.cluster.id
  description = "Security group ID for the EKS control plane"
}

output "node_security_group_id" {
  value       = aws_security_group.nodes.id
  description = "Security group ID for EKS node groups"
}

output "kms_key_arn" {
  value       = aws_kms_key.eks.arn
  description = "KMS key ARN used for secrets encryption"
}

output "cluster_autoscaler_role_arn" {
  value       = aws_iam_role.cluster_autoscaler.arn
  description = "IAM role ARN for the cluster autoscaler -- annotate the service account with this"
}

output "node_group_names" {
  value       = keys(aws_eks_node_group.user)
  description = "Names of all user node groups"
}

# ─── AKS vs EKS COMPARISON ────────────────────────────────────────────────────
#
# Feature               AKS                            EKS
# ──────────────────────────────────────────────────────────────────────────────
# Identity              Managed Identity (auto)        IAM Role (explicit)
# Image registry        ACR                            ECR
# Secret store          Key Vault                      Secrets Manager / KMS
# Monitoring            Azure Monitor / Log Analytics  CloudWatch Container Insights
# Autoscaler            Built-in                       Cluster Autoscaler add-on (IRSA)
# Network plugin        Azure CNI / Kubenet            VPC CNI (aws-node daemonset)
# Load balancer         Azure LB (automatic)           AWS Load Balancer Controller
# OIDC / Workload ID    Built-in OIDC issuer           OIDC provider (explicit resource)
# State backend         Azure Blob Storage             S3 + DynamoDB locking
# Node OS upgrade       NodeImage channel              Managed node group AMI update
# Private cluster       API server access profile      endpoint_private_access = true
