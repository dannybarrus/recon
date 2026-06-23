# tf/

Terraform implementations organized by concept and platform. Each 
folder is self-contained with its own README. The numbering reflects 
a loose progression from foundational concepts to production patterns, 
but folders can be read independently.

## Folders

| Folder              | Covers |
|---------------------|--------|
| `00_FlatNamespace/` | Same-directory resource sharing, no module boundaries |
| `01_Fundamentals/`  | Providers, resources, variables, outputs, locals, data sources |
| `02_Modules/`       | Same-dir, module boundary, and cross-workspace remote state |
| `03_Patterns/`      | for_each vs count, dynamic blocks, conditionals, workspaces |
| `04_AWS/`           | AWS provider, VPC, IAM, ECR, S3 |
| `04_AWS_EKS/`       | EKS cluster, node groups, OIDC, KMS, add-ons, IRSA |
| `04_Azure/`         | Azure provider, VNet, Key Vault, ACR, Storage |
| `04_Azure_AKS/`     | Production AKS -- node pools, Azure CNI, AD RBAC, auto-upgrade |
| `05_CICD/`          | GitHub Actions -- plan on PR, gated apply, drift detection |
| `06_Security/`      | Sensitive variables, Key Vault, RBAC, tfsec |
| `07_Testing/`       | Terratest integration tests, mock providers for unit testing |
## AKS and EKS -- a note

`04_Azure_AKS/` and `04_AWS_EKS/` are intentionally parallel -- same 
architecture, same depth, different providers. The goal was to build 
both from scratch and map the concepts directly across platforms.

`04_AWS_EKS/05_outputs.tf` contains a full side-by-side comparison 
table covering identity, registry, secrets, monitoring, autoscaling, 
networking, OIDC, and state backends. If you want a quick reference 
for how the two platforms map to each other, that is the place to 
start.
