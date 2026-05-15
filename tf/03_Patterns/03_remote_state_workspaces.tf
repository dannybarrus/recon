# ─── PATTERN: Remote State ────────────────────────────────────────────────────
#
# Remote state stores the Terraform state file in shared, locking-capable
# storage rather than locally. Essential for team workflows.
#
# Benefits:
#   - Shared state visible to all team members
#   - State locking prevents concurrent applies corrupting state
#   - State history and versioning via blob versioning

terraform {
  backend "azurerm" {
    resource_group_name  = "rg-tfstate"
    storage_account_name = "stfstaterecon"     # globally unique, alphanumeric
    container_name       = "tfstate"
    key                  = "prod/terraform.tfstate"

    # State locking: azurerm backend uses blob leases automatically.
    # No additional configuration needed -- locking is on by default.

    # Credentials: set via environment variables or ARM_ prefix vars
    # ARM_CLIENT_ID, ARM_CLIENT_SECRET, ARM_TENANT_ID, ARM_SUBSCRIPTION_ID
    # Or use use_azure_ad_auth = true for Azure AD / Managed Identity
    use_azure_ad_auth = true
  }
}

# ─── Bootstrapping remote state storage ───────────────────────────────────────
#
# The storage account itself cannot be managed by the Terraform configuration
# that uses it as a backend -- chicken and egg problem.
# Create it once via Azure CLI or a separate bootstrap configuration.
#
# az group create --name rg-tfstate --location "East US"
# az storage account create \
#   --name stfstaterecon \
#   --resource-group rg-tfstate \
#   --sku Standard_LRS \
#   --encryption-services blob \
#   --min-tls-version TLS1_2
# az storage container create \
#   --name tfstate \
#   --account-name stfstaterecon

# ─── PATTERN: Workspaces ─────────────────────────────────────────────────────
#
# Workspaces allow multiple state files from the same configuration.
# The default workspace is named "default".
#
# Common use: one workspace per environment (dev, staging, prod)
# terraform workspace new dev
# terraform workspace new staging
# terraform workspace new prod
# terraform workspace select dev
# terraform workspace show   # prints current workspace name
#
# Each workspace gets its own state file:
#   tfstate/env:/dev/terraform.tfstate
#   tfstate/env:/staging/terraform.tfstate
#   tfstate/env:/prod/terraform.tfstate

locals {
  # terraform.workspace returns the current workspace name
  workspace = terraform.workspace

  # Use workspace to drive environment-specific configuration
  env_config = {
    dev = {
      node_count   = 2
      vm_size      = "Standard_D2s_v3"
      aks_sku      = "Free"
    }
    staging = {
      node_count   = 3
      vm_size      = "Standard_D4s_v3"
      aks_sku      = "Standard"
    }
    prod = {
      node_count   = 5
      vm_size      = "Standard_D8s_v3"
      aks_sku      = "Standard"
    }
  }

  # Falls back to dev config for unknown workspaces
  config = lookup(local.env_config, local.workspace, local.env_config["dev"])
}

resource "azurerm_resource_group" "main" {
  name     = "rg-recon-${terraform.workspace}"
  location = "East US"
}

resource "azurerm_kubernetes_cluster" "main" {
  name                = "aks-recon-${terraform.workspace}"
  location            = azurerm_resource_group.main.location
  resource_group_name = azurerm_resource_group.main.name
  dns_prefix          = "recon-${terraform.workspace}"
  sku_tier            = local.config.aks_sku

  default_node_pool {
    name       = "system"
    node_count = local.config.node_count
    vm_size    = local.config.vm_size
  }

  identity {
    type = "SystemAssigned"
  }
}

# ─── Workspace caveat ────────────────────────────────────────────────────────
#
# Workspaces are simple but have a scaling limitation: all environments
# share the same backend configuration and the same codebase.
# For strong isolation (separate subscriptions, separate backends per env),
# Terragrunt is the more robust solution. See Terragrunt pattern files.
