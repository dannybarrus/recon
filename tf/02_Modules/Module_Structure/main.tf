# ─── MODULE: network ─────────────────────────────────────────────────────────
# File: modules/network/main.tf
#
# This is a reusable network module. It creates a VNet and a configurable
# number of subnets. The parent module supplies inputs via variables.tf
# and reads results via outputs.tf.
#
# Within this directory, all .tf files share the same namespace.
# Resources in main.tf can reference variables from variables.tf directly.
# No import or wiring needed between files in the same module.

locals {
  name_prefix = "recon-${var.environment}"
}

resource "azurerm_virtual_network" "main" {
  name                = "vnet-${local.name_prefix}"
  resource_group_name = var.resource_group_name
  location            = var.location
  address_space       = var.vnet_address_space
  tags                = var.tags
}

resource "azurerm_subnet" "main" {
  count = var.subnet_count

  name                 = "subnet-${local.name_prefix}-${count.index}"
  resource_group_name  = var.resource_group_name
  virtual_network_name = azurerm_virtual_network.main.name
  address_prefixes     = [cidrsubnet(var.vnet_address_space[0], 8, count.index)]
}

resource "azurerm_network_security_group" "main" {
  name                = "nsg-${local.name_prefix}"
  resource_group_name = var.resource_group_name
  location            = var.location
  tags                = var.tags
}

resource "azurerm_subnet_network_security_group_association" "main" {
  count = var.subnet_count

  subnet_id                 = azurerm_subnet.main[count.index].id
  network_security_group_id = azurerm_network_security_group.main.id
}
