# 02_Modules

Demonstrates the three ways Terraform shares values between configurations, from simplest to most decoupled.

## Subfolders

| Folder | Scenario | Mechanism |
|---|---|---|
| `00_same_directory` | Files in the same module | Direct reference — no wiring needed |
| `01_module_structure` | A reusable child module | `variables.tf` inputs, `outputs.tf` return values |
| `02_module_composition` | Parent consuming a child module | `module.<name>.<output_name>` |
| `03_cross_workspace` | Separate Terraform workspaces | `data.terraform_remote_state` |

## The Three Scenarios

**Same directory:** Terraform merges all `.tf` files into one namespace. Reference resources directly by their address. No outputs needed.

**Module boundary:** The child module exposes values via `output` blocks. The parent reads them via `module.<name>.<output>`. Internal resources of the child are not accessible — only what it explicitly exports.

**Cross workspace:** The producer workspace writes outputs to remote state. The consumer workspace reads them via the `terraform_remote_state` data source. Used when separate teams manage separate infrastructure roots.

## Module Structure Convention

A well-structured module always has three files at minimum:

```
module/
  variables.tf   # inputs -- the public interface
  main.tf        # resources -- the implementation
  outputs.tf     # outputs -- the return values
```
