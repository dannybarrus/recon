# ─── OUTPUTS ─────────────────────────────────────────────────────────────────
#
# Outputs expose values from a Terraform module. They serve three purposes:
#
#   1. Display values after apply (terraform output)
#   2. Return values from a child module to its parent (module.name.output_name)
#   3. Write values to remote state for consumption by other workspaces
#      via terraform_remote_state data source
#
# See 02_Modules/ for examples of outputs crossing module and workspace boundaries.

# ─── Basic output ─────────────────────────────────────────────────────────────

output "resource_group_name" {
  value       = azurerm_resource_group.main.name
  description = "Name of the primary resource group"
}

# ─── Output referencing a computed attribute ──────────────────────────────────
#
# id is assigned by Azure after the resource is created.
# Terraform resolves this at apply time, not plan time.

output "resource_group_id" {
  value       = azurerm_resource_group.main.id
  description = "Resource ID of the primary resource group"
}

# ─── Sensitive output ─────────────────────────────────────────────────────────
#
# Redacted in terminal output. Still readable by downstream modules
# and the remote state data source. Not encrypted in state.

output "storage_connection_string" {
  value       = azurerm_storage_account.main.primary_connection_string
  description = "Primary connection string for the storage account"
  sensitive   = true
}

# ─── Output from a list resource (count) ─────────────────────────────────────

output "subnet_ids" {
  value       = azurerm_subnet.zones[*].id
  description = "List of subnet IDs created across availability zones"
}

# ─── Transformed output ───────────────────────────────────────────────────────
#
# Outputs can apply expressions, not just raw attribute references.

output "subnet_map" {
  value = {
    for subnet in azurerm_subnet.zones :
    subnet.name => subnet.id
  }
  description = "Map of subnet name to subnet ID"
}

# ─── Conditional output ───────────────────────────────────────────────────────

output "monitoring_workspace_id" {
  value       = var.enable_monitoring ? azurerm_log_analytics_workspace.main[0].id : null
  description = "Log Analytics workspace ID (null if monitoring is disabled)"
}
