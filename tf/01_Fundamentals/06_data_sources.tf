# ─── DATA SOURCES ────────────────────────────────────────────────────────────
#
# Data sources read existing infrastructure. They let Terraform reference
# resources it did not create and does not manage. Read-only -- data sources
# never create, update, or destroy anything.
#
# Syntax: data "<provider>_<type>" "<local_name>" { ... }
# Reference: data.<provider>_<type>.<local_name>.<attribute>

# ─── Current Azure context ────────────────────────────────────────────────────
#
# Reads the current subscription and tenant from the authenticated session.
# Useful for building resource IDs without hardcoding subscription IDs.

data "azurerm_client_config" "current" {}

output "current_subscription_id" {
  value = data.azurerm_client_config.current.subscription_id
}

output "current_tenant_id" {
  value = data.azurerm_client_config.current.tenant_id
}

# ─── Existing resource group ──────────────────────────────────────────────────
#
# Reference a resource group created outside this Terraform configuration.
# Common when networking is managed by a separate team or workspace.

data "azurerm_resource_group" "networking" {
  name = "rg-networking-prod"
}

# ─── Existing virtual network ─────────────────────────────────────────────────

data "azurerm_virtual_network" "shared" {
  name                = "vnet-shared-prod"
  resource_group_name = data.azurerm_resource_group.networking.name
}

# ─── Existing subnet ──────────────────────────────────────────────────────────

data "azurerm_subnet" "aks" {
  name                 = "subnet-aks"
  virtual_network_name = data.azurerm_virtual_network.shared.name
  resource_group_name  = data.azurerm_resource_group.networking.name
}

# ─── Key Vault secret ─────────────────────────────────────────────────────────
#
# Read a secret from an existing Key Vault. The service principal running
# Terraform needs Get permission on the vault.

data "azurerm_key_vault" "platform" {
  name                = "kv-platform-prod"
  resource_group_name = "rg-platform-prod"
}

data "azurerm_key_vault_secret" "db_password" {
  name         = "db-admin-password"
  key_vault_id = data.azurerm_key_vault.platform.id
}

# Use the secret value in a resource (marked sensitive automatically)
# resource "azurerm_mssql_server" "main" {
#   administrator_login_password = data.azurerm_key_vault_secret.db_password.value
# }

# ─── Azure AD / Entra group ───────────────────────────────────────────────────
#
# Look up an existing AD group to use in role assignments.

data "azuread_group" "platform_engineers" {
  display_name     = "Platform Engineers"
  security_enabled = true
}

# ─── Latest AKS version ───────────────────────────────────────────────────────
#
# Dynamically resolve the latest stable Kubernetes version for a region
# rather than hardcoding a version that goes stale.

data "azurerm_kubernetes_service_versions" "current" {
  location        = var.location
  include_preview = false
}

output "latest_k8s_version" {
  value = data.azurerm_kubernetes_service_versions.current.latest_version
}

# ─── Using data sources in resources ─────────────────────────────────────────

resource "azurerm_kubernetes_cluster" "main" {
  name                = "aks-recon-dev"
  location            = var.location
  resource_group_name = azurerm_resource_group.main.name
  kubernetes_version  = data.azurerm_kubernetes_service_versions.current.latest_version

  default_node_pool {
    name           = "system"
    node_count     = 3
    vm_size        = "Standard_D4s_v3"
    vnet_subnet_id = data.azurerm_subnet.aks.id   # subnet from data source
  }

  identity {
    type = "SystemAssigned"
  }
}
