# ─── SAME-DIRECTORY SHARING: FILE B ─────────────────────────────────────────
# File: 02_compute.tf
#
# This file references resources defined in 01_network.tf directly.
# No imports. No outputs. No module wiring of any kind.
#
# Terraform merges all .tf files in the same directory into one namespace
# before planning. By the time Terraform processes any file, it already
# knows about every resource, local, and variable in every other file
# in the directory.
#
# WHAT THIS DEMONSTRATES:
#   azurerm_resource_group.main      -- defined in 01_network.tf, used here
#   azurerm_virtual_network.main     -- defined in 01_network.tf, used here
#   azurerm_subnet.aks               -- defined in 01_network.tf, used here
#   local.common_tags                -- defined in 01_network.tf, used here
#   local.is_prod                    -- defined in 01_network.tf, used here
#   var.environment                  -- defined in 01_network.tf, used here
#
# Terraform also infers dependencies automatically. The AKS cluster below
# depends on the subnet, which depends on the VNet, which depends on the
# resource group. Terraform resolves this graph and creates resources in
# the correct order without any explicit depends_on.

resource "azurerm_kubernetes_cluster" "main" {
  name      = "aks-recon-${var.environment}"           # var defined in 01_network.tf
  location  = azurerm_resource_group.main.location     # resource defined in 01_network.tf
  resource_group_name = azurerm_resource_group.main.name
  dns_prefix          = "recon-${var.environment}"
  sku_tier            = local.is_prod ? "Standard" : "Free"  # local defined in 01_network.tf

  default_node_pool {
    name           = "system"
    node_count     = local.is_prod ? 3 : 2             # local defined in 01_network.tf
    vm_size        = "Standard_D4s_v3"
    vnet_subnet_id = azurerm_subnet.aks.id             # resource defined in 01_network.tf
  }

  identity {
    type = "SystemAssigned"
  }

  network_profile {
    network_plugin = "azure"
  }

  tags = local.common_tags   # local defined in 01_network.tf
}

# ─── Outputs ──────────────────────────────────────────────────────────────────
# Outputs are only needed when a PARENT module or another WORKSPACE needs
# to read these values. Within this directory, they are not needed for sharing.

output "cluster_id" {
  value       = azurerm_kubernetes_cluster.main.id
  description = "AKS cluster ID -- exposed for parent module or remote state consumers"
}

output "vnet_name" {
  value       = azurerm_virtual_network.main.name    # from 01_network.tf, no output needed there
  description = "VNet name -- re-exposed for parent if needed"
}

# ─── SUMMARY ──────────────────────────────────────────────────────────────────
#
# SAME DIRECTORY (this example):
#   No output blocks needed between files.
#   Reference resources directly: azurerm_resource_group.main.name
#   Terraform merges all files -- one flat namespace.
#
# ACROSS MODULE BOUNDARY (see 02_module_composition/):
#   Child module exposes output blocks.
#   Parent reads via: module.<name>.<output_name>
#
# ACROSS WORKSPACES (see 03_cross_workspace/):
#   Producer exposes output blocks, written to remote state.
#   Consumer reads via: data.terraform_remote_state.<name>.outputs.<output_name>
