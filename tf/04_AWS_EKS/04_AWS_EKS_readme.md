# 04_AWS_EKS

Amazon Elastic Kubernetes Service — the AWS analog to `04_Azure_AKS`. Same structure, same depth, different platform.

## Files

| File | Covers |
|---|---|
| `01_variables.tf` | Cluster name, Kubernetes version, node group configs, endpoint access, OIDC settings |
| `02_locals.tf` | Cluster name resolution, prod flag, tag merging |
| `03_cluster.tf` | Security groups, EKS cluster, OIDC provider, KMS key, system and user node groups |
| `04_addons.tf` | Core add-ons (VPC CNI, CoreDNS, kube-proxy, EBS CSI), CloudWatch logging, cluster autoscaler IRSA |
| `05_outputs.tf` | Cluster endpoint, OIDC ARN, security group IDs, node group names, AKS vs EKS comparison table |

## Key EKS Concepts

**OIDC Provider:** Must be created as an explicit Terraform resource. Required before IRSA works. AKS creates this automatically.

**IRSA (IAM Roles for Service Accounts):** Grants individual pods IAM permissions without node-level credentials. The AWS equivalent of AKS workload identity. Requires the OIDC provider and a trust policy linking the IAM role to a Kubernetes service account.

**Managed Add-ons:** Core components (CoreDNS, VPC CNI, kube-proxy) are managed by AWS and upgraded independently of the cluster. AKS handles these transparently.

**Cluster Autoscaler:** Must be deployed and configured explicitly in EKS. AKS has built-in autoscaling as a cluster flag.

## AKS vs EKS Quick Reference

See the comparison table at the bottom of `05_outputs.tf` for a full side-by-side mapping of concepts across both platforms.
