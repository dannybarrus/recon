# ─── AZURE: VNET AND SUBNETS ──────────────────────────────────────────────────

resource "azurerm_virtual_network" "main" {
  name                = "vnet-recon-${var.environment}"
  resource_group_name = azurerm_resource_group.main.name
  location            = azurerm_resource_group.main.location
  address_space       = ["10.0.0.0/16"]
  tags                = { environment = var.environment, managed_by = "terraform" }
}

resource "azurerm_subnet" "aks" {
  name                 = "subnet-aks"
  resource_group_name  = azurerm_resource_group.main.name
  virtual_network_name = azurerm_virtual_network.main.name
  address_prefixes     = ["10.0.1.0/24"]

  service_endpoints = ["Microsoft.ContainerRegistry", "Microsoft.KeyVault"]
}

resource "azurerm_subnet" "ingress" {
  name                 = "subnet-ingress"
  resource_group_name  = azurerm_resource_group.main.name
  virtual_network_name = azurerm_virtual_network.main.name
  address_prefixes     = ["10.0.2.0/24"]
}

resource "azurerm_subnet" "data" {
  name                 = "subnet-data"
  resource_group_name  = azurerm_resource_group.main.name
  virtual_network_name = azurerm_virtual_network.main.name
  address_prefixes     = ["10.0.3.0/24"]
  service_endpoints    = ["Microsoft.Sql", "Microsoft.Storage"]
}

output "vnet_id"          { value = azurerm_virtual_network.main.id   }
output "vnet_name"        { value = azurerm_virtual_network.main.name }
output "aks_subnet_id"    { value = azurerm_subnet.aks.id             }
output "ingress_subnet_id"{ value = azurerm_subnet.ingress.id         }
output "data_subnet_id"   { value = azurerm_subnet.data.id            }
