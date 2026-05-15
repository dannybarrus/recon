# ─── SAME-DIRECTORY SHARING: FILE A ─────────────────────────────────────────
# File: 01_network.tf
#
# KEY CONCEPT: Within the same Terraform module (directory), all .tf files
# are merged into one flat namespace at plan time. Resources, locals, and
# variables defined here are directly referenceable in any other .tf file
# in the same directory.
#
# No output blocks needed. No import statements. No wiring.
# Terraform merges everything -- the order of files does not matter.
#
# See 02_compute.tf to see how these resources are consumed.

variable "environment" {
  type    = string
  default = "dev"
}

variable "location" {
  type    = string
  default = "East US"
}

# ─── Resource group ───────────────────────────────────────────────────────────
# Defined here in 01_network.tf.
# Consumed directly in 02_compute.tf without any output block.

resource "azurerm_resource_group" "main" {
  name     = "rg-recon-${var.environment}"
  location = var.location
  tags     = { environment = var.environment, managed_by = "terraform" }
}

# ─── Virtual network ──────────────────────────────────────────────────────────
# Also consumed directly in 02_compute.tf.

resource "azurerm_virtual_network" "main" {
  name                = "vnet-recon-${var.environment}"
  resource_group_name = azurerm_resource_group.main.name
  location            = azurerm_resource_group.main.location
  address_space       = ["10.0.0.0/16"]
  tags                = azurerm_resource_group.main.tags
}

# ─── Subnet ───────────────────────────────────────────────────────────────────

resource "azurerm_subnet" "aks" {
  name                 = "subnet-aks"
  resource_group_name  = azurerm_resource_group.main.name
  virtual_network_name = azurerm_virtual_network.main.name
  address_prefixes     = ["10.0.1.0/24"]
}

# ─── Local values ─────────────────────────────────────────────────────────────
# Locals defined here are also directly available in 02_compute.tf.

locals {
  common_tags = {
    environment = var.environment
    managed_by  = "terraform"
    team        = "platform"
  }

  is_prod = var.environment == "prod"
}
