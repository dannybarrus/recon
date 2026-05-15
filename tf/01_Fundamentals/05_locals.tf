# ─── LOCALS ──────────────────────────────────────────────────────────────────
#
# Locals are computed values within a module. They avoid repeating expressions
# and give meaningful names to complex calculations. Unlike variables, locals
# are not exposed as inputs or outputs -- they are internal to the module.

locals {
  # ─── Naming convention ───────────────────────────────────────────────────
  # Centralise naming logic so all resources follow the same pattern.
  # Change the pattern once, all resource names update.

  prefix       = "recon"
  name_suffix  = "${local.prefix}-${var.environment}"

  resource_group_name = "rg-${local.name_suffix}"
  vnet_name           = "vnet-${local.name_suffix}"
  aks_name            = "aks-${local.name_suffix}"
  acr_name            = "acr${replace(local.name_suffix, "-", "")}"  # ACR names: alphanumeric only

  # ─── Tag merging ─────────────────────────────────────────────────────────
  # Merge caller-supplied tags with mandatory baseline tags.
  # Caller tags take precedence via the right-hand map.

  common_tags = merge(
    {
      environment = var.environment
      managed_by  = "terraform"
      team        = "platform"
      created_at  = timestamp()
    },
    var.tags   # caller overrides win
  )

  # ─── Computed values ─────────────────────────────────────────────────────

  is_production  = var.environment == "prod"
  replica_count  = local.is_production ? 3 : 1

  # ─── CIDR calculations ────────────────────────────────────────────────────

  vnet_cidr     = "10.0.0.0/16"
  subnet_cidrs  = [for i in range(3) : cidrsubnet(local.vnet_cidr, 8, i)]
  # cidrsubnet("10.0.0.0/16", 8, 0) = "10.0.0.0/24"
  # cidrsubnet("10.0.0.0/16", 8, 1) = "10.0.1.0/24"
  # cidrsubnet("10.0.0.0/16", 8, 2) = "10.0.2.0/24"

  # ─── Conditional configuration ────────────────────────────────────────────

  aks_sku = local.is_production ? "Standard" : "Free"

  node_pool_config = {
    vm_size           = local.is_production ? "Standard_D8s_v3" : "Standard_D2s_v3"
    node_count        = local.is_production ? 5 : 2
    enable_auto_scale = local.is_production
  }
}

# ─── Using locals in resources ────────────────────────────────────────────────

resource "azurerm_resource_group" "main" {
  name     = local.resource_group_name   # "rg-recon-dev"
  location = var.location
  tags     = local.common_tags
}

resource "azurerm_virtual_network" "main" {
  name                = local.vnet_name
  resource_group_name = azurerm_resource_group.main.name
  location            = azurerm_resource_group.main.location
  address_space       = [local.vnet_cidr]
  tags                = local.common_tags
}
