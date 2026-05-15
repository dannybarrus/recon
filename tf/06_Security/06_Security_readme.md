# 06_Security

Security patterns and best practices for production Terraform configurations.

## Files

| File | Covers |
|---|---|
| `01_sensitive_rbac_tfsec.tf` | Sensitive variables and outputs, Key Vault secret pattern, RBAC role assignments, tfsec configuration |

## Sensitive Variables

Marking a variable or output `sensitive = true` redacts its value from `terraform plan` and `terraform apply` output. It does **not** encrypt the value in state. The state file itself must be protected — use encrypted storage, RBAC access controls, and private endpoints on the backend.

## Secrets Management

Never store real secrets in `.tfvars` files committed to source control. The correct pattern:

1. Store the secret in Key Vault (Azure) or Secrets Manager (AWS)
2. Read it at apply time via a `data` source
3. Pass the value directly to the resource that needs it

## RBAC Assignments

Role assignments in Terraform follow the principle of least privilege. Assign the narrowest role at the narrowest scope. Common patterns covered:

- Platform team Contributor on resource group
- App team Reader on resource group
- AKS RBAC Cluster Admin for platform group
- Storage Blob Data Contributor for the kubelet identity

## tfsec

tfsec is a static analysis tool that scans Terraform for security misconfigurations before plan or apply. Run it in CI alongside `terraform validate`. Rules can be suppressed with documented justification when a risk is accepted.
