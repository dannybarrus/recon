# ─── RESOURCES ───────────────────────────────────────────────────────────────
#
# Resources are the core building blocks of Terraform. Each resource block
# declares one piece of infrastructure. Terraform manages its full lifecycle:
# create, update, destroy.
#
# Syntax: resource "<provider>_<type>" "<local_name>" { ... }
# Reference: <provider>_<type>.<local_name>.<attribute>

# ─── Basic resource ───────────────────────────────────────────────────────────

resource "azurerm_resource_group" "main" {
  name     = "rg-recon-dev"
  location = "East US"

  tags = {
    environment = "dev"
    managed_by  = "terraform"
  }
}

# ─── Resource referencing another resource ────────────────────────────────────
#
# azurerm_resource_group.main.name reads the name attribute from the resource
# above. Terraform automatically infers a dependency — the VNet will not be
# created until the resource group exists.

resource "azurerm_virtual_network" "main" {
  name                = "vnet-recon-dev"
  resource_group_name = azurerm_resource_group.main.name    # implicit dependency
  location            = azurerm_resource_group.main.location
  address_space       = ["10.0.0.0/16"]
}

# ─── META-ARGUMENTS ──────────────────────────────────────────────────────────

# depends_on: explicit dependency when Terraform cannot infer it automatically
resource "azurerm_role_assignment" "example" {
  scope                = azurerm_resource_group.main.id
  role_definition_name = "Contributor"
  principal_id         = "00000000-0000-0000-0000-000000000000"

  depends_on = [azurerm_resource_group.main]   # explicit — usually not needed here
}

# count: create multiple instances of a resource
resource "azurerm_subnet" "zones" {
  count = 3   # creates azurerm_subnet.zones[0], [1], [2]

  name                 = "subnet-zone-${count.index}"
  resource_group_name  = azurerm_resource_group.main.name
  virtual_network_name = azurerm_virtual_network.main.name
  address_prefixes     = ["10.0.${count.index}.0/24"]
}

# lifecycle: control resource replacement and deletion behaviour
resource "azurerm_storage_account" "main" {
  name                     = "strecondev001"
  resource_group_name      = azurerm_resource_group.main.name
  location                 = azurerm_resource_group.main.location
  account_tier             = "Standard"
  account_replication_type = "LRS"

  lifecycle {
    prevent_destroy       = true    # block terraform destroy for this resource
    ignore_changes        = [tags]  # do not react to tag changes made outside Terraform
    create_before_destroy = true    # create replacement before destroying original
  }
}
