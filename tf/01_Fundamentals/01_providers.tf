# ─── PROVIDERS ───────────────────────────────────────────────────────────────
#
# Providers are plugins that allow Terraform to interact with cloud platforms,
# SaaS services, and other APIs. Every resource belongs to a provider.
#
# terraform block: declares required providers and their version constraints.
# provider block:  configures a specific provider instance.

terraform {
  required_version = ">= 1.5.0"

  required_providers {
    azurerm = {
      source  = "hashicorp/azurerm"
      version = "~> 3.0"    # ~> 3.0 means >= 3.0.0, < 4.0.0
    }
    random = {
      source  = "hashicorp/random"
      version = ">= 3.0"
    }
  }
}

# ─── Azure provider ───────────────────────────────────────────────────────────
#
# Credentials are never hardcoded here. Terraform reads from:
#   - Environment variables: ARM_CLIENT_ID, ARM_CLIENT_SECRET, ARM_TENANT_ID, ARM_SUBSCRIPTION_ID
#   - Azure CLI: az login
#   - Managed Identity (when running in Azure)

provider "azurerm" {
  features {
    resource_group {
      prevent_deletion_if_contains_resources = true   # safety guard
    }
    key_vault {
      purge_soft_delete_on_destroy = false            # preserve for recovery
    }
  }

  # subscription_id = var.subscription_id  # or set ARM_SUBSCRIPTION_ID env var
}

# ─── Alias: multiple provider instances ──────────────────────────────────────
#
# Aliases allow multiple configurations of the same provider.
# Useful for multi-region or multi-subscription deployments.

provider "azurerm" {
  alias           = "secondary"
  subscription_id = var.secondary_subscription_id

  features {}
}

# ─── Random provider ─────────────────────────────────────────────────────────
#
# Generates random values for resource names, passwords, etc.
# No configuration required.

provider "random" {}

# ─── Variable referenced above ────────────────────────────────────────────────

variable "secondary_subscription_id" {
  type        = string
  description = "Azure subscription ID for the secondary region provider"
  default     = ""
}
