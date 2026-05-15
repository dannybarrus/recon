# 04_Azure

Core Azure building blocks. Primary cloud for this repo — reflects real production experience at WTW.

## Files

| File | Covers |
|---|---|
| `01_resource_group.tf` | Resource group with tags |
| `02_vnet_subnet.tf` | Virtual network, purpose-named subnets with service endpoints |
| `03_aks_cluster.tf` | AKS cluster — see `04_Azure_AKS` for the deep-dive version |
| `04_keyvault_acr_storage.tf` | Key Vault with network ACLs, ACR with geo-replication, Storage Account with versioning |

## Azure vs AWS Conceptual Mapping

| Azure | AWS |
|---|---|
| Resource Group | No direct equivalent (tags serve a similar purpose) |
| Virtual Network | VPC |
| Subnet | Subnet (AZ-scoped in AWS, region-scoped in Azure) |
| NSG | Security Group / Network ACL |
| Managed Identity | IAM Role |
| Key Vault | Secrets Manager / KMS |
| ACR | ECR |
| Storage Account | S3 |
| Azure Monitor | CloudWatch |

## Notes

Azure handles several networking concerns implicitly that AWS requires as explicit resources: internet routing, outbound NAT, and DNS are all available by default. This makes the Azure configuration shorter but can obscure what is actually happening at the network layer.
