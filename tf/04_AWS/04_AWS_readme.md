# 04_AWS

Core AWS building blocks. The direct analog to `04_Azure` — same concepts, different provider and resource names.

## Files

| File | Covers | Azure Analog |
|---|---|---|
| `01_provider.tf` | AWS provider, OIDC auth, S3 backend with DynamoDB locking | `azurerm` provider, Azure Blob backend |
| `02_vpc_subnets.tf` | VPC, public/private subnets, Internet Gateway, NAT Gateway, route tables | VNet, subnets, NSG |
| `03_iam_ecr_s3.tf` | IAM roles for EKS, ECR repository, S3 bucket, Secrets Manager | Managed Identity, ACR, Storage Account, Key Vault |

## Notable Differences from Azure

AWS requires considerably more explicit infrastructure than Azure. Things Azure handles implicitly that AWS requires as explicit resources:

- **Internet Gateway** — required for any public subnet routing
- **NAT Gateway + Elastic IP** — required for private subnet outbound traffic
- **Route Tables** — must be created and associated manually
- **IAM Roles** — must be explicitly created and attached; no auto-assigned managed identity
- **DynamoDB table** — required for state locking (Azure uses blob leases automatically)

## State Backend

AWS uses S3 for state storage and DynamoDB for locking — two resources that must be bootstrapped outside Terraform before the backend can be configured.
