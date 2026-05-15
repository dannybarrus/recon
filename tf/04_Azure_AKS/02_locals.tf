# ─── AKS: LOCALS AND DATA SOURCES ────────────────────────────────────────────

data "azurerm_client_config" "current" {}

# Dynamically resolve latest stable Kubernetes version if none specified
data "azurerm_kubernetes_service_versions" "current" {
  location        = var.location
  include_preview = false
}

locals {
  cluster_name    = "aks-recon-${var.environment}"
  is_prod         = var.environment == "prod"

  # Use supplied version or fall back to latest stable
  kubernetes_version = var.kubernetes_version != "" \
    ? var.kubernetes_version \
    : data.azurerm_kubernetes_service_versions.current.latest_version

  # SKU tier: Standard for prod (SLA-backed), Free for non-prod
  sku_tier = local.is_prod ? "Standard" : "Free"

  # API server access: restrict in prod, open in dev
  api_server_authorized_ip_ranges = length(var.authorized_ip_ranges) > 0 \
    ? var.authorized_ip_ranges \
    : local.is_prod ? [] : null   # null = unrestricted in dev

  common_tags = merge(
    {
      environment  = var.environment
      managed_by   = "terraform"
      cluster_name = local.cluster_name
    },
    var.tags
  )
}
