# 03_Patterns

Reusable patterns that appear repeatedly in production Terraform configurations.

## Files

| File | Pattern | Key Takeaway |
|---|---|---|
| `01_for_each.tf` | `for_each` vs `count` | Prefer `for_each` for named resources — removing an item does not shift indexes |
| `02_dynamic_and_conditionals.tf` | Dynamic blocks, conditional resources | `dynamic` generates repeated nested blocks; `count = var.enabled ? 1 : 0` creates optional resources |
| `03_remote_state_workspaces.tf` | Remote state backend, workspaces | S3 or Azure Blob for shared state; workspaces for environment isolation |

## When to Use Each

**`count`:** When resources are identical except for an index and the set is stable. A fixed number of subnets is fine with count.

**`for_each`:** When resources have natural names or when items may be added or removed independently. Removing item `[0]` with `count` shifts all subsequent indexes and destroys/recreates them. Removing a key with `for_each` only removes that one resource.

**Dynamic blocks:** When the number of nested configuration blocks is determined by a variable rather than hardcoded.

**Workspaces:** Lightweight environment isolation sharing the same codebase. For stronger isolation (separate subscriptions or accounts per environment), consider Terragrunt.
