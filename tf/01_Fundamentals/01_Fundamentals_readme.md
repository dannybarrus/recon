# 01_Fundamentals

The core building blocks of any Terraform configuration. Every other folder in this repo builds on these concepts.

## Files

| File | Covers |
|---|---|
| `01_providers.tf` | Provider configuration, version constraints, OIDC auth, provider aliases for multi-region |
| `02_resources.tf` | Resource blocks, meta-arguments — `depends_on`, `count`, `lifecycle` |
| `03_variables.tf` | Input variable types — string, number, bool, list, map, object, sensitive |
| `04_outputs.tf` | Output blocks — basic, sensitive, list, map, conditional |
| `05_locals.tf` | Local values — naming conventions, tag merging, CIDR calculations, conditional config |
| `06_data_sources.tf` | Reading existing infrastructure — current context, existing VNets, Key Vault secrets, dynamic AKS version |

## Key Concepts

**Variables** are the inputs to a module — what callers supply.  
**Locals** are computed values internal to a module — never exposed as inputs or outputs.  
**Outputs** are the return values of a module — what callers can read back.  
**Data sources** read existing infrastructure without managing it.

Credentials are never hardcoded. Use environment variables, CLI auth, or managed identity.
