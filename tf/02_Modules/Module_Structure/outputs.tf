# ─── MODULE: network ─────────────────────────────────────────────────────────
# File: modules/network/outputs.tf
#
# Outputs are the return values of this module. The parent module reads
# these values via: module.<module_name>.<output_name>
#
# Example: module.network.vnet_id
#          module.network.subnet_ids
#
# KEY CONCEPT: outputs cross the module boundary.
# Resources within this module reference each other directly (no outputs needed).
# Only values the PARENT needs to consume require an output block.

output "vnet_id" {
  value       = azurerm_virtual_network.main.id
  description = "Resource ID of the virtual network"
}

output "vnet_name" {
  value       = azurerm_virtual_network.main.name
  description = "Name of the virtual network"
}

output "subnet_ids" {
  value       = azurerm_subnet.main[*].id
  description = "List of subnet resource IDs"
}

output "subnet_names" {
  value       = azurerm_subnet.main[*].name
  description = "List of subnet names"
}

output "subnet_map" {
  value = {
    for subnet in azurerm_subnet.main :
    subnet.name => subnet.id
  }
  description = "Map of subnet name to subnet ID -- useful for named lookups"
}

output "nsg_id" {
  value       = azurerm_network_security_group.main.id
  description = "Resource ID of the network security group"
}
