# ─── AZURE: AKS CLUSTER ──────────────────────────────────────────────────────
#
# Production-grade AKS cluster with:
#   - System and user node pools
#   - Azure CNI networking (pod-level networking in the VNet)
#   - Managed identity (no service principal)
#   - Azure Monitor integration
#   - RBAC and Azure AD integration
#   - Private cluster option

variable "resource_group_name" { type = string }
variable "location"            { type = string  default = "East US" }
variable "environment"         { type = string  default = "dev" }
variable "aks_subnet_id"       { type = string }
variable "log_analytics_id"    { type = string  default = "" }

locals {
  cluster_name = "aks-recon-${var.environment}"
  is_prod      = var.environment == "prod"
}

resource "azurerm_kubernetes_cluster" "main" {
  name                = local.cluster_name
  location            = var.location
  resource_group_name = var.resource_group_name
  dns_prefix          = "recon-${var.environment}"
  kubernetes_version  = var.kubernetes_version
  sku_tier            = local.is_prod ? "Standard" : "Free"

  # ─── System node pool ──────────────────────────────────────────────
  default_node_pool {
    name                = "system"
    node_count          = local.is_prod ? 3 : 2
    vm_size             = local.is_prod ? "Standard_D4s_v3" : "Standard_D2s_v3"
    vnet_subnet_id      = var.aks_subnet_id
    os_disk_size_gb     = 128
    type                = "VirtualMachineScaleSets"
    enable_auto_scaling = local.is_prod
    min_count           = local.is_prod ? 3 : null
    max_count           = local.is_prod ? 10 : null

    only_critical_addons_enabled = true   # system pool for system pods only

    node_labels = {
      "nodepool-type" = "system"
      "environment"   = var.environment
    }
  }

  # ─── Managed identity ──────────────────────────────────────────────
  identity {
    type = "SystemAssigned"
  }

  # ─── Azure CNI networking ──────────────────────────────────────────
  network_profile {
    network_plugin     = "azure"
    network_policy     = "azure"
    load_balancer_sku  = "standard"
    outbound_type      = "loadBalancer"
  }

  # ─── Azure AD RBAC ─────────────────────────────────────────────────
  azure_active_directory_role_based_access_control {
    managed            = true
    azure_rbac_enabled = true
  }

  # ─── Add-ons ───────────────────────────────────────────────────────
  oms_agent {
    log_analytics_workspace_id = var.log_analytics_id
  }

  azure_policy_enabled             = true
  http_application_routing_enabled = false   # use nginx ingress instead

  # ─── Maintenance window ────────────────────────────────────────────
  maintenance_window {
    allowed {
      day   = "Sunday"
      hours = [2, 3, 4]   # 2am-4am Sunday
    }
  }

  tags = { environment = var.environment, managed_by = "terraform" }
}

# ─── User node pool (workloads) ───────────────────────────────────────────────

resource "azurerm_kubernetes_cluster_node_pool" "user" {
  name                  = "user"
  kubernetes_cluster_id = azurerm_kubernetes_cluster.main.id
  vm_size               = local.is_prod ? "Standard_D8s_v3" : "Standard_D4s_v3"
  node_count            = local.is_prod ? 5 : 2
  vnet_subnet_id        = var.aks_subnet_id
  enable_auto_scaling   = true
  min_count             = 2
  max_count             = local.is_prod ? 20 : 5
  os_disk_size_gb       = 256

  node_labels = {
    "nodepool-type" = "user"
    "environment"   = var.environment
  }

  node_taints = ["workload=general:NoSchedule"]   # keep system pods off this pool
}

variable "kubernetes_version" {
  type    = string
  default = "1.28"
}

# ─── Outputs ──────────────────────────────────────────────────────────────────

output "cluster_id"             { value = azurerm_kubernetes_cluster.main.id }
output "cluster_fqdn"           { value = azurerm_kubernetes_cluster.main.fqdn }
output "kubelet_identity"       { value = azurerm_kubernetes_cluster.main.kubelet_identity[0].object_id }
output "kube_config"            { value = azurerm_kubernetes_cluster.main.kube_config_raw  sensitive = true }
output "node_resource_group"    { value = azurerm_kubernetes_cluster.main.node_resource_group }
