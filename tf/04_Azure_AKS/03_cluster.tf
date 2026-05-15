# ─── AKS: CLUSTER ────────────────────────────────────────────────────────────
#
# Production-grade AKS cluster.
# Variables, locals, and data sources are defined in 01_variables.tf
# and 02_locals.tf -- referenced directly here, no wiring needed.

resource "azurerm_kubernetes_cluster" "main" {
  name                = local.cluster_name
  location            = var.location
  resource_group_name = var.resource_group_name
  dns_prefix          = local.cluster_name
  kubernetes_version  = local.kubernetes_version
  sku_tier            = local.sku_tier

  # ─── System node pool ──────────────────────────────────────────────
  # Runs system pods (kube-dns, metrics-server, etc).
  # only_critical_addons_enabled keeps user workloads off this pool.

  default_node_pool {
    name                         = "system"
    vm_size                      = var.system_node_pool.vm_size
    node_count                   = var.system_node_pool.node_count
    os_disk_size_gb              = var.system_node_pool.os_disk_size_gb
    max_pods                     = var.system_node_pool.max_pods
    vnet_subnet_id               = var.aks_subnet_id
    type                         = "VirtualMachineScaleSets"
    only_critical_addons_enabled = true
    temporary_name_for_rotation  = "systemtemp"   # required for in-place node pool updates

    upgrade_settings {
      max_surge = "33%"   # allow 1/3 extra nodes during upgrades
    }

    node_labels = {
      "nodepool-type" = "system"
      "environment"   = var.environment
    }

    tags = local.common_tags
  }

  # ─── Managed identity ──────────────────────────────────────────────
  # SystemAssigned: identity managed by Azure, no credential rotation needed.

  identity {
    type = "SystemAssigned"
  }

  # ─── Azure CNI networking ──────────────────────────────────────────
  # Pods get IPs from the VNet subnet directly.
  # Required for network policies and private cluster access.

  network_profile {
    network_plugin     = "azure"
    network_policy     = "azure"     # or "calico" for cross-platform
    load_balancer_sku  = "standard"
    outbound_type      = "loadBalancer"
    service_cidr       = "172.16.0.0/16"
    dns_service_ip     = "172.16.0.10"
  }

  # ─── Azure AD RBAC ─────────────────────────────────────────────────
  # Managed Azure AD integration: no service principals to rotate.
  # azure_rbac_enabled uses Azure RBAC for Kubernetes authorization.

  azure_active_directory_role_based_access_control {
    managed                = true
    azure_rbac_enabled     = true
    admin_group_object_ids = var.admin_group_object_ids
  }

  # ─── API server access ─────────────────────────────────────────────
  # Restrict API server to known CIDR ranges in prod.

  api_server_access_profile {
    authorized_ip_ranges = local.api_server_authorized_ip_ranges
  }

  # ─── Azure Monitor ─────────────────────────────────────────────────

  dynamic "oms_agent" {
    for_each = var.log_analytics_workspace_id != "" ? [1] : []
    content {
      log_analytics_workspace_id      = var.log_analytics_workspace_id
      msi_auth_for_monitoring_enabled = true
    }
  }

  # ─── Add-ons ───────────────────────────────────────────────────────

  azure_policy_enabled             = true
  http_application_routing_enabled = false   # use nginx or AGIC instead
  image_cleaner_enabled            = true    # remove unused images from nodes
  image_cleaner_interval_hours     = 48

  # ─── Maintenance window ────────────────────────────────────────────
  # Schedule node upgrades during low-traffic periods.

  maintenance_window {
    allowed {
      day   = "Sunday"
      hours = [2, 3, 4]
    }
  }

  maintenance_window_node_os {
    frequency  = "Weekly"
    interval   = 1
    day_of_week = "Sunday"
    start_time = "02:00"
    utc_offset = "+00:00"
    duration   = 4
  }

  # ─── Auto-upgrade ─────────────────────────────────────────────────

  automatic_channel_upgrade         = local.is_prod ? "stable" : "rapid"
  node_os_channel_upgrade           = "NodeImage"

  tags = local.common_tags

  lifecycle {
    ignore_changes = [
      kubernetes_version,       # managed by automatic_channel_upgrade
      default_node_pool[0].node_count  # managed by autoscaler
    ]
  }
}

# ─── AKS: USER NODE POOLS ─────────────────────────────────────────────────────
#
# for_each over var.user_node_pools creates one node pool per map entry.
# Each pool is independently scalable and configurable.

resource "azurerm_kubernetes_cluster_node_pool" "user" {
  for_each = var.user_node_pools

  name                  = each.key
  kubernetes_cluster_id = azurerm_kubernetes_cluster.main.id
  vm_size               = each.value.vm_size
  node_count            = each.value.node_count
  os_disk_size_gb       = each.value.os_disk_size_gb
  max_pods              = each.value.max_pods
  vnet_subnet_id        = var.aks_subnet_id
  enable_auto_scaling   = each.value.enable_auto_scale
  min_count             = each.value.enable_auto_scale ? each.value.min_count : null
  max_count             = each.value.enable_auto_scale ? each.value.max_count : null
  node_taints           = each.value.node_taints
  node_labels           = merge(each.value.node_labels, {
    "nodepool-type" = each.key
    "environment"   = var.environment
  })
  mode = "User"

  upgrade_settings {
    max_surge = "33%"
  }

  tags = local.common_tags
}
