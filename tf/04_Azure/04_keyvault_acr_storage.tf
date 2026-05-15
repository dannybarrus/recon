# ─── AZURE: KEY VAULT ─────────────────────────────────────────────────────────

data "azurerm_client_config" "current" {}

resource "azurerm_key_vault" "main" {
  name                        = "kv-recon-${var.environment}"
  resource_group_name         = azurerm_resource_group.main.name
  location                    = azurerm_resource_group.main.location
  tenant_id                   = data.azurerm_client_config.current.tenant_id
  sku_name                    = "standard"
  soft_delete_retention_days  = 90
  purge_protection_enabled    = true   # required for CMK scenarios

  # Network ACL -- restrict to VNet service endpoints
  network_acls {
    default_action             = "Deny"
    bypass                     = "AzureServices"
    virtual_network_subnet_ids = [azurerm_subnet.aks.id]
  }

  tags = { environment = var.environment, managed_by = "terraform" }
}

# Grant the Terraform service principal admin access to manage secrets
resource "azurerm_key_vault_access_policy" "terraform" {
  key_vault_id = azurerm_key_vault.main.id
  tenant_id    = data.azurerm_client_config.current.tenant_id
  object_id    = data.azurerm_client_config.current.object_id

  secret_permissions = ["Get", "List", "Set", "Delete", "Recover", "Backup", "Restore"]
  key_permissions    = ["Get", "List", "Create", "Delete", "Recover"]
}

output "key_vault_id"  { value = azurerm_key_vault.main.id  }
output "key_vault_uri" { value = azurerm_key_vault.main.vault_uri }

# ─── AZURE: CONTAINER REGISTRY ────────────────────────────────────────────────

resource "azurerm_container_registry" "main" {
  # ACR names: 5-50 alphanumeric characters, globally unique
  name                = "acrrecon${var.environment}001"
  resource_group_name = azurerm_resource_group.main.name
  location            = azurerm_resource_group.main.location
  sku                 = var.environment == "prod" ? "Premium" : "Standard"
  admin_enabled       = false   # use managed identity, not admin credentials

  # Premium only: geo-replication
  dynamic "georeplications" {
    for_each = var.environment == "prod" ? ["West US 2"] : []
    content {
      location                  = georeplications.value
      zone_redundancy_enabled   = true
    }
  }

  tags = { environment = var.environment, managed_by = "terraform" }
}

# Grant AKS kubelet identity pull access to ACR
resource "azurerm_role_assignment" "aks_acr_pull" {
  scope                = azurerm_container_registry.main.id
  role_definition_name = "AcrPull"
  principal_id         = var.aks_kubelet_identity   # from AKS cluster output
}

output "acr_id"           { value = azurerm_container_registry.main.id             }
output "acr_login_server" { value = azurerm_container_registry.main.login_server   }

# ─── AZURE: STORAGE ACCOUNT ───────────────────────────────────────────────────

resource "azurerm_storage_account" "main" {
  name                     = "strecon${var.environment}001"
  resource_group_name      = azurerm_resource_group.main.name
  location                 = azurerm_resource_group.main.location
  account_tier             = "Standard"
  account_replication_type = var.environment == "prod" ? "ZRS" : "LRS"
  min_tls_version          = "TLS1_2"
  https_traffic_only       = true
  allow_nested_items_to_be_public = false   # block anonymous blob access

  network_rules {
    default_action             = "Deny"
    bypass                     = ["AzureServices"]
    virtual_network_subnet_ids = [azurerm_subnet.data.id]
  }

  blob_properties {
    versioning_enabled  = true
    delete_retention_policy {
      days = 30
    }
  }

  tags = { environment = var.environment, managed_by = "terraform" }

  lifecycle {
    prevent_destroy = true
  }
}

output "storage_account_id"                { value = azurerm_storage_account.main.id }
output "storage_primary_blob_endpoint"     { value = azurerm_storage_account.main.primary_blob_endpoint }
output "storage_primary_connection_string" {
  value     = azurerm_storage_account.main.primary_connection_string
  sensitive = true
}

variable "aks_kubelet_identity" { type = string  default = "" }
