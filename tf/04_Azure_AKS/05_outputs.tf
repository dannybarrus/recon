# ─── AKS: OUTPUTS ────────────────────────────────────────────────────────────
#
# Outputs expose cluster values to the parent module or remote state consumers.
# Within this directory, resources reference each other directly -- no outputs
# needed for internal sharing (see 03_cluster.tf referencing locals from
# 02_locals.tf as a live example).

output "cluster_id" {
  value       = azurerm_kubernetes_cluster.main.id
  description = "AKS cluster resource ID"
}

output "cluster_name" {
  value       = azurerm_kubernetes_cluster.main.name
  description = "AKS cluster name"
}

output "cluster_fqdn" {
  value       = azurerm_kubernetes_cluster.main.fqdn
  description = "AKS API server FQDN"
}

output "kube_config_raw" {
  value       = azurerm_kubernetes_cluster.main.kube_config_raw
  description = "Raw kubeconfig -- use with kubectl or helm providers"
  sensitive   = true
}

output "kube_config" {
  value = {
    host                   = azurerm_kubernetes_cluster.main.kube_config[0].host
    client_certificate     = azurerm_kubernetes_cluster.main.kube_config[0].client_certificate
    client_key             = azurerm_kubernetes_cluster.main.kube_config[0].client_key
    cluster_ca_certificate = azurerm_kubernetes_cluster.main.kube_config[0].cluster_ca_certificate
  }
  description = "Structured kubeconfig attributes"
  sensitive   = true
}

output "kubelet_identity_object_id" {
  value       = azurerm_kubernetes_cluster.main.kubelet_identity[0].object_id
  description = "Kubelet managed identity object ID -- used for ACR pull and Key Vault access"
}

output "node_resource_group" {
  value       = azurerm_kubernetes_cluster.main.node_resource_group
  description = "Auto-generated resource group containing node pool VMs"
}

output "oidc_issuer_url" {
  value       = azurerm_kubernetes_cluster.main.oidc_issuer_url
  description = "OIDC issuer URL -- used for workload identity federation"
}

output "kubernetes_version" {
  value       = azurerm_kubernetes_cluster.main.kubernetes_version
  description = "Kubernetes version running on the cluster"
}

output "log_analytics_workspace_id" {
  value       = local.resolved_workspace_id
  description = "Log Analytics workspace ID used for cluster monitoring"
}

output "node_pool_names" {
  value       = keys(azurerm_kubernetes_cluster_node_pool.user)
  description = "Names of all user node pools"
}
