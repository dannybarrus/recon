# ─── CROSS-WORKSPACE OUTPUT: CONSUMER ────────────────────────────────────────
# File: 03_cross_workspace/aks/main.tf
#
# This workspace reads outputs from the networking workspace via
# terraform_remote_state. The AKS team does not manage the VNet --
# they only consume the subnet ID that the networking team published.
#
# KEY CONCEPT: terraform_remote_state is a data source that reads the
#              state file of another Terraform workspace. It exposes
#              that workspace's outputs under data.terraform_remote_state
#              .<name>.outputs.<output_name>.

terraform {
  required_providers {
    azurerm = {
      source  = "hashicorp/azurerm"
      version = "~> 3.0"
    }
  }

  # This workspace's own remote state -- separate key from networking
  backend "azurerm" {
    resource_group_name  = "rg-tfstate"
    storage_account_name = "stfstaterecon"
    container_name       = "tfstate"
    key                  = "aks.tfstate"   # different key = different workspace
  }
}

provider "azurerm" { features {} }

# ─── Read networking workspace outputs ────────────────────────────────────────
#
# Points to the networking workspace's state file in the same backend.
# The config block mirrors the backend config of the producer workspace.

data "terraform_remote_state" "networking" {
  backend = "azurerm"

  config = {
    resource_group_name  = "rg-tfstate"
    storage_account_name = "stfstaterecon"
    container_name       = "tfstate"
    key                  = "networking.tfstate"   # the producer's state file key
  }
}

# ─── Use remote state outputs ─────────────────────────────────────────────────
#
# data.terraform_remote_state.networking.outputs.<output_name>
# refers to the output blocks defined in the networking producer.

locals {
  # Extract the values we need from the networking workspace
  aks_subnet_id      = data.terraform_remote_state.networking.outputs.aks_subnet_id
  networking_rg_name = data.terraform_remote_state.networking.outputs.resource_group_name
  vnet_id            = data.terraform_remote_state.networking.outputs.vnet_id
}

# ─── AKS resource group ───────────────────────────────────────────────────────

resource "azurerm_resource_group" "aks" {
  name     = "rg-aks-prod"
  location = "East US"
}

# ─── AKS cluster using networking team's subnet ───────────────────────────────

resource "azurerm_kubernetes_cluster" "main" {
  name                = "aks-prod"
  location            = azurerm_resource_group.aks.location
  resource_group_name = azurerm_resource_group.aks.name
  dns_prefix          = "aks-prod"

  default_node_pool {
    name   = "system"
    count  = 3
    vm_size = "Standard_D4s_v3"

    # Subnet ID from the networking workspace's remote state output.
    # This AKS workspace has no knowledge of how the subnet was created.
    # The contract is the output name: aks_subnet_id.
    vnet_subnet_id = local.aks_subnet_id
  }

  identity {
    type = "SystemAssigned"
  }

  network_profile {
    network_plugin = "azure"
  }
}

# ─── Outputs ──────────────────────────────────────────────────────────────────
#
# This workspace can in turn expose outputs to a third workspace.
# Example: an application workspace might consume the AKS cluster ID.

output "aks_cluster_id" {
  value       = azurerm_kubernetes_cluster.main.id
  description = "AKS cluster resource ID -- available for downstream workspaces"
}

output "aks_cluster_fqdn" {
  value       = azurerm_kubernetes_cluster.main.fqdn
  description = "AKS API server FQDN"
}

# ─── SUMMARY: the three output/input scenarios ────────────────────────────────
#
# 1. SAME DIRECTORY (same module):
#    Resources reference each other directly. No outputs needed.
#    azurerm_subnet.main references azurerm_virtual_network.main.name directly.
#    Terraform merges all .tf files in the directory into one namespace.
#
# 2. MODULE BOUNDARY (parent/child):
#    Child exposes values via output blocks in outputs.tf.
#    Parent reads them via module.<module_name>.<output_name>.
#    See 02_module_composition/main.tf.
#
# 3. SEPARATE WORKSPACES (cross-root):
#    Producer writes outputs to remote state via its backend.
#    Consumer reads them via data "terraform_remote_state" data source.
#    Accessed as data.terraform_remote_state.<name>.outputs.<output_name>.
#    This file demonstrates scenario 3.
