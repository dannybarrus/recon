# ─── AKS: VARIABLES ──────────────────────────────────────────────────────────

variable "environment" {
  type        = string
  description = "Deployment environment: dev, staging, prod"
  default     = "dev"

  validation {
    condition     = contains(["dev", "staging", "prod"], var.environment)
    error_message = "Environment must be dev, staging, or prod."
  }
}

variable "location" {
  type        = string
  description = "Azure region"
  default     = "East US"
}

variable "resource_group_name" {
  type        = string
  description = "Resource group to deploy into"
}

variable "aks_subnet_id" {
  type        = string
  description = "Subnet ID for the AKS node pools"
}

variable "log_analytics_workspace_id" {
  type        = string
  description = "Log Analytics workspace ID for Azure Monitor"
  default     = ""
}

variable "kubernetes_version" {
  type        = string
  description = "Kubernetes version. Leave empty to use latest stable."
  default     = ""
}

variable "system_node_pool" {
  type = object({
    vm_size         = string
    node_count      = number
    os_disk_size_gb = number
    max_pods        = number
  })
  description = "System node pool configuration"
  default = {
    vm_size         = "Standard_D4s_v3"
    node_count      = 3
    os_disk_size_gb = 128
    max_pods        = 110
  }
}

variable "user_node_pools" {
  type = map(object({
    vm_size           = string
    node_count        = number
    os_disk_size_gb   = number
    max_pods          = number
    enable_auto_scale = bool
    min_count         = optional(number, 2)
    max_count         = optional(number, 10)
    node_taints       = optional(list(string), [])
    node_labels       = optional(map(string), {})
  }))
  description = "Map of user node pool name to configuration"
  default = {
    general = {
      vm_size           = "Standard_D4s_v3"
      node_count        = 3
      os_disk_size_gb   = 256
      max_pods          = 110
      enable_auto_scale = true
      min_count         = 2
      max_count         = 10
    }
  }
}

variable "authorized_ip_ranges" {
  type        = list(string)
  description = "CIDR ranges authorized to access the API server. Empty = public."
  default     = []
}

variable "admin_group_object_ids" {
  type        = list(string)
  description = "Azure AD group object IDs granted cluster admin access"
  default     = []
}

variable "tags" {
  type        = map(string)
  description = "Tags applied to all resources"
  default     = {}
}
