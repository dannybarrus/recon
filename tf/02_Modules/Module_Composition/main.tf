# ─── MODULE COMPOSITION ──────────────────────────────────────────────────────
# File: 02_module_composition/main.tf
#
# This file demonstrates a parent module consuming outputs from a child module.
#
# SCENARIO: The network module creates a VNet and subnets.
#           The AKS cluster needs the subnet ID from that module.
#           The parent wires the two together via module outputs.
#
# KEY CONCEPT: module.<name>.<output> reads a child module's output.
#              The child module's internal resources are not directly accessible.
#              Only what the child exposes via outputs is available to the parent.

terraform {
  required_providers {
    azurerm = {
      source  = "hashicorp/azurerm"
      version = "~> 3.0"
    }
  }
}

provider "azurerm" {
  features {}
}

# ─── Variables ────────────────────────────────────────────────────────────────

variable "environment" {
  type    = string
  default = "dev"
}

variable "location" {
  type    = string
  default = "East US"
}

# ─── Shared resource group ────────────────────────────────────────────────────

resource "azurerm_resource_group" "main" {
  name     = "rg-recon-${var.environment}"
  location = var.location
  tags     = { managed_by = "terraform", environment = var.environment }
}

# ─── Child module: network ────────────────────────────────────────────────────
#
# The module block calls the network module defined in 01_module_structure/.
# Inputs are supplied here -- they become the values of the module's variables.

module "network" {
  source = "../01_module_structure"   # path to the module

  # Inputs -- map to variable blocks in the module's variables.tf
  resource_group_name = azurerm_resource_group.main.name
  location            = azurerm_resource_group.main.location
  environment         = var.environment
  vnet_address_space  = ["10.0.0.0/16"]
  subnet_count        = 3
  tags                = { managed_by = "terraform", environment = var.environment }
}

# ─── Child module: AKS (hypothetical second module) ───────────────────────────
#
# This AKS resource demonstrates consuming the network module's outputs.
# module.network.subnet_ids[0] reads the first subnet ID that the
# network module exposed via its outputs.tf file.

resource "azurerm_kubernetes_cluster" "main" {
  name                = "aks-recon-${var.environment}"
  location            = azurerm_resource_group.main.location
  resource_group_name = azurerm_resource_group.main.name
  dns_prefix          = "recon-${var.environment}"

  default_node_pool {
    name           = "system"
    node_count     = 3
    vm_size        = "Standard_D4s_v3"

    # Consuming output from the network module
    # The AKS cluster does not know or care how the subnet was created.
    # It only knows the subnet ID via the module's output contract.
    vnet_subnet_id = module.network.subnet_ids[0]
  }

  identity {
    type = "SystemAssigned"
  }

  network_profile {
    network_plugin    = "azure"
    load_balancer_sku = "standard"
  }

  tags = { managed_by = "terraform", environment = var.environment }
}

# ─── Parent-level outputs ─────────────────────────────────────────────────────
#
# The parent can re-export child module outputs, passing them further up
# the chain or writing them to remote state for other workspaces.

output "vnet_id" {
  value       = module.network.vnet_id
  description = "Re-exported from the network module"
}

output "subnet_ids" {
  value       = module.network.subnet_ids
  description = "Re-exported from the network module"
}

output "aks_cluster_id" {
  value       = azurerm_kubernetes_cluster.main.id
  description = "AKS cluster resource ID"
}

output "aks_kube_config" {
  value       = azurerm_kubernetes_cluster.main.kube_config_raw
  description = "Raw kubeconfig for the AKS cluster"
  sensitive   = true
}
