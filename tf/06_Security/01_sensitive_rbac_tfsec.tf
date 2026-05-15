# ─── SECURITY: SENSITIVE VARIABLES AND RBAC ──────────────────────────────────

data "azurerm_client_config" "current" {}

# ─── Sensitive variables ──────────────────────────────────────────────────────
#
# sensitive = true redacts values in plan output and logs.
# It does NOT encrypt values in state -- state itself must be protected.
# Use Azure Storage with encryption, RBAC, and private access for state.

variable "admin_password" {
  type      = string
  sensitive = true   # redacted in terraform plan / apply output
}

# Sensitive outputs -- redacted in terminal, accessible to downstream modules
output "storage_connection_string" {
  value     = azurerm_storage_account.main.primary_connection_string
  sensitive = true
}

# ─── Secrets management: Key Vault, not variables ─────────────────────────────
#
# Never store real secrets in variables or tfvars files committed to source.
# The correct pattern: store in Key Vault, read via data source.

data "azurerm_key_vault_secret" "db_password" {
  name         = "db-admin-password"
  key_vault_id = azurerm_key_vault.main.id
}

# resource "azurerm_mssql_server" "main" {
#   administrator_login_password = data.azurerm_key_vault_secret.db_password.value
# }

# ─── RBAC ASSIGNMENTS ─────────────────────────────────────────────────────────

variable "platform_team_group_id" { type = string  default = "" }
variable "app_team_group_id"      { type = string  default = "" }

# Platform team: Contributor on the platform resource group
resource "azurerm_role_assignment" "platform_contributor" {
  scope                = azurerm_resource_group.main.id
  role_definition_name = "Contributor"
  principal_id         = var.platform_team_group_id
}

# App team: Reader on the platform resource group
resource "azurerm_role_assignment" "app_reader" {
  scope                = azurerm_resource_group.main.id
  role_definition_name = "Reader"
  principal_id         = var.app_team_group_id
}

# AKS: RBAC admin for platform team at cluster scope
resource "azurerm_role_assignment" "aks_cluster_admin" {
  scope                = azurerm_kubernetes_cluster.main.id
  role_definition_name = "Azure Kubernetes Service RBAC Cluster Admin"
  principal_id         = var.platform_team_group_id
}

# Storage: Blob Contributor for the app identity
resource "azurerm_role_assignment" "app_storage_blob" {
  scope                = azurerm_storage_account.main.id
  role_definition_name = "Storage Blob Data Contributor"
  principal_id         = azurerm_kubernetes_cluster.main.kubelet_identity[0].object_id
}

# ─── TFSEC CONFIGURATION ──────────────────────────────────────────────────────
# File: .tfsec/config.yml
#
# tfsec is a static analysis tool for Terraform. It scans for security issues
# before plan or apply. Run it in CI alongside terraform validate.
#
# Install: brew install tfsec  or  go install github.com/aquasecurity/tfsec/cmd/tfsec@latest
# Run:     tfsec ./tf

# Ignore specific rules where you have accepted the risk
# (put this in .tfsec/config.yml, not in a .tf file)
#
# exclude:
#   - AVD-AZU-0017   # Storage account not using CMK -- acceptable for non-sensitive data
#
# severity_overrides:
#   AVD-AZU-0040: LOW   # downgrade severity for a specific check
