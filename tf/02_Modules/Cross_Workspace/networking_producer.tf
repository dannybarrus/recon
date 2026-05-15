# ─── CROSS-WORKSPACE OUTPUT: PRODUCER ────────────────────────────────────────
# File: 03_cross_workspace/networking/main.tf
#
# SCENARIO: The networking team manages VNets and subnets in their own
#           Terraform workspace. The AKS team needs the subnet IDs but
#           should not manage the network infrastructure.
#
# SOLUTION: The networking workspace writes its outputs to remote state.
#           The AKS workspace reads them via terraform_remote_state.
#
# This file is the PRODUCER -- it creates the infrastructure and exposes
# outputs that get written to the Azure Storage remote state backend.

terraform {
  required_providers {
    azurerm = {
      source  = "hashicorp/azurerm"
      version = "~> 3.0"
    }
  }

  # Remote state backend -- state is stored in Azure Blob Storage.
  # The outputs defined below are written into this state file.
  backend "azurerm" {
    resource_group_name  = "rg-tfstate"
    storage_account_name = "stfstaterecon"
    container_name       = "tfstate"
    key                  = "networking.tfstate"   # unique key per workspace
  }
}

provider "azurerm" { features {} }

# ─── Networking infrastructure ────────────────────────────────────────────────

resource "azurerm_resource_group" "networking" {
  name     = "rg-networking-prod"
  location = "East US"
}

resource "azurerm_virtual_network" "main" {
  name                = "vnet-shared-prod"
  resource_group_name = azurerm_resource_group.networking.name
  location            = azurerm_resource_group.networking.location
  address_space       = ["10.0.0.0/8"]
}

resource "azurerm_subnet" "aks" {
  name                 = "subnet-aks-prod"
  resource_group_name  = azurerm_resource_group.networking.name
  virtual_network_name = azurerm_virtual_network.main.name
  address_prefixes     = ["10.0.1.0/24"]
}

resource "azurerm_subnet" "ingress" {
  name                 = "subnet-ingress-prod"
  resource_group_name  = azurerm_resource_group.networking.name
  virtual_network_name = azurerm_virtual_network.main.name
  address_prefixes     = ["10.0.2.0/24"]
}

# ─── Outputs written to remote state ─────────────────────────────────────────
#
# These outputs are available to any workspace that reads this state file
# via the terraform_remote_state data source. See the consumer file.

output "vnet_id" {
  value       = azurerm_virtual_network.main.id
  description = "Shared VNet resource ID -- consumed by AKS, App Gateway workspaces"
}

output "vnet_name" {
  value       = azurerm_virtual_network.main.name
  description = "Shared VNet name"
}

output "aks_subnet_id" {
  value       = azurerm_subnet.aks.id
  description = "AKS node pool subnet ID"
}

output "ingress_subnet_id" {
  value       = azurerm_subnet.ingress.id
  description = "Ingress controller subnet ID"
}

output "resource_group_name" {
  value       = azurerm_resource_group.networking.name
  description = "Networking resource group name"
}
