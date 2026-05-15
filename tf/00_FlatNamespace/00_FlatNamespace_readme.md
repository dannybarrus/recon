# 00_FlatNamespace

Demonstrates how Terraform shares values between files within the same directory without output blocks, imports, or any explicit wiring.

All `.tf` files in the same directory are merged into one flat namespace at plan time. A resource defined in `01_network.tf` is directly referenceable in `02_compute.tf` by name. Terraform resolves the dependency graph automatically.

## Files

| File | Purpose |
|---|---|
| `01_network.tf` | Defines the resource group, VNet, subnet, and locals |
| `02_compute.tf` | Consumes those resources directly — no output blocks needed |

## Key Concept

Output blocks are only required when values need to cross a **module boundary** or a **workspace boundary**. Within the same directory, everything is already visible to everything else.

| Scenario | Mechanism |
|---|---|
| Same directory | Direct reference — `azurerm_resource_group.main.name` |
| Module boundary | Output block in child, `module.<name>.<output>` in parent |
| Cross workspace | Output block in producer, `data.terraform_remote_state` in consumer |

See `02_Modules` for the module boundary and cross-workspace examples.
