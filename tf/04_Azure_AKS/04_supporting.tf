# ─── AKS: SUPPORTING RESOURCES ───────────────────────────────────────────────
#
# Resources that integrate with the cluster defined in 03_cluster.tf.
# Referenced directly -- same directory, same namespace.

# ─── Log Analytics Workspace ──────────────────────────────────────────────────
# Created here when log_analytics_workspace_id is not supplied.
# If an existing workspace is supplied via variable, this is skipped.

resource "azurerm_log_analytics_workspace" "aks" {
  count = var.log_analytics_workspace_id == "" ? 1 : 0

  name                = "law-${local.cluster_name}"
  resource_group_name = var.resource_group_name
  location            = var.location
  sku                 = "PerGB2018"
  retention_in_days   = local.is_prod ? 90 : 30
  tags                = local.common_tags
}

locals {
  # Use supplied workspace or the one created above
  resolved_workspace_id = var.log_analytics_workspace_id != "" \
    ? var.log_analytics_workspace_id \
    : one(azurerm_log_analytics_workspace.aks[*].id)
}

# ─── Diagnostic settings ──────────────────────────────────────────────────────
# Ships cluster control plane logs to Log Analytics.

resource "azurerm_monitor_diagnostic_setting" "aks" {
  name                       = "diag-${local.cluster_name}"
  target_resource_id         = azurerm_kubernetes_cluster.main.id
  log_analytics_workspace_id = local.resolved_workspace_id

  # Control plane logs
  enabled_log { category = "kube-apiserver"          }
  enabled_log { category = "kube-controller-manager" }
  enabled_log { category = "kube-scheduler"          }
  enabled_log { category = "kube-audit"              }
  enabled_log { category = "kube-audit-admin"        }
  enabled_log { category = "guard"                   }

  # Metrics
  metric {
    category = "AllMetrics"
    enabled  = true
  }
}

# ─── ACR pull role assignment ──────────────────────────────────────────────────
# Grants the cluster's kubelet identity permission to pull images from ACR.
# Requires the ACR resource ID as a variable.

variable "acr_id" {
  type        = string
  description = "ACR resource ID to grant AcrPull to the kubelet identity"
  default     = ""
}

resource "azurerm_role_assignment" "acr_pull" {
  count = var.acr_id != "" ? 1 : 0

  scope                = var.acr_id
  role_definition_name = "AcrPull"
  principal_id         = azurerm_kubernetes_cluster.main.kubelet_identity[0].object_id
}

# ─── Cluster admin role assignment ────────────────────────────────────────────
# Grants the current Terraform service principal cluster admin rights.
# Useful for post-deploy configuration via kubectl or helm.

resource "azurerm_role_assignment" "cluster_admin" {
  scope                = azurerm_kubernetes_cluster.main.id
  role_definition_name = "Azure Kubernetes Service RBAC Cluster Admin"
  principal_id         = data.azurerm_client_config.current.object_id
}
