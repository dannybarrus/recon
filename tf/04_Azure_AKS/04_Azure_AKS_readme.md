# 04_Azure_AKS

Azure Kubernetes Service — a dedicated deep-dive into AKS configuration. The broad AKS resource in `04_Azure` is intentionally minimal; this folder covers production-grade configuration in full.

## Files

| File | Covers |
|---|---|
| `01_variables.tf` | Environment, location, system node pool config, user node pool map, authorized IP ranges, admin groups |
| `02_locals.tf` | Cluster naming, prod flag, dynamic Kubernetes version resolution, SKU tier, API server access rules |
| `03_cluster.tf` | Cluster resource — system pool, managed identity, Azure CNI, Azure AD RBAC, monitoring, maintenance windows, auto-upgrade |
| `04_supporting.tf` | Log Analytics workspace, diagnostic settings, ACR pull role assignment, cluster admin role assignment |
| `05_outputs.tf` | Cluster ID, FQDN, kubeconfig, kubelet identity, OIDC issuer, node pool names |

## Design Decisions

**System node pool:** `only_critical_addons_enabled = true` prevents user workloads from scheduling on system nodes. Mirrors the AKS best practice of separating system and user workloads.

**User node pools via `for_each`:** Any number of node pools can be defined by adding entries to the `user_node_pools` map variable. No Terraform code changes required.

**Dynamic monitoring block:** The `oms_agent` block is only included when a Log Analytics workspace ID is supplied. When omitted, a workspace is created automatically by `04_supporting.tf`.

**Auto-upgrade:** `stable` channel in prod, `rapid` in non-prod. Node OS images updated independently via `NodeImage` channel.

**OIDC issuer:** Exposed as an output for workload identity configuration downstream.
