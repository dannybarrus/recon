# ─── EKS: VARIABLES ──────────────────────────────────────────────────────────
#
# Analog to 04_AKS/01_variables.tf
#
# AWS / EKS concept mapping:
#   Node Pool     -> Node Group (managed) or self-managed node group
#   System Pool   -> No direct equivalent -- use taints/tolerations
#   VM Size       -> Instance Type (e.g. m5.xlarge)
#   Managed Identity -> IAM Role with IRSA (IAM Roles for Service Accounts)
#   Azure Monitor -> CloudWatch Container Insights

variable "environment" {
  type    = string
  default = "dev"

  validation {
    condition     = contains(["dev", "staging", "prod"], var.environment)
    error_message = "Environment must be dev, staging, or prod."
  }
}

variable "region" {
  type    = string
  default = "us-east-1"
}

variable "cluster_name" {
  type        = string
  description = "EKS cluster name. Defaults to eks-recon-<environment>."
  default     = ""
}

variable "kubernetes_version" {
  type        = string
  description = "Kubernetes version. Must be a version supported by EKS."
  default     = "1.28"
}

variable "vpc_id" {
  type        = string
  description = "VPC ID to deploy the cluster into"
}

variable "private_subnet_ids" {
  type        = list(string)
  description = "Private subnet IDs for EKS node groups"
}

variable "public_subnet_ids" {
  type        = list(string)
  description = "Public subnet IDs for load balancers"
}

variable "cluster_role_arn" {
  type        = string
  description = "IAM role ARN for the EKS cluster"
}

variable "node_role_arn" {
  type        = string
  description = "IAM role ARN for EKS node groups"
}

variable "system_node_group" {
  type = object({
    instance_types  = list(string)
    desired_size    = number
    min_size        = number
    max_size        = number
    disk_size       = number
  })
  description = "System node group configuration"
  default = {
    instance_types = ["m5.large"]
    desired_size   = 2
    min_size       = 2
    max_size       = 4
    disk_size      = 50
  }
}

variable "user_node_groups" {
  type = map(object({
    instance_types = list(string)
    desired_size   = number
    min_size       = number
    max_size       = number
    disk_size      = number
    capacity_type  = optional(string, "ON_DEMAND")   # ON_DEMAND or SPOT
    labels         = optional(map(string), {})
    taints         = optional(list(object({
      key    = string
      value  = string
      effect = string
    })), [])
  }))
  description = "Map of user node group name to configuration"
  default = {
    general = {
      instance_types = ["m5.xlarge"]
      desired_size   = 3
      min_size       = 2
      max_size       = 10
      disk_size      = 100
      capacity_type  = "ON_DEMAND"
    }
  }
}

variable "enable_cloudwatch_logging" {
  type    = bool
  default = true
}

variable "cluster_log_types" {
  type    = list(string)
  default = ["api", "audit", "authenticator", "controllerManager", "scheduler"]
}

variable "endpoint_private_access" {
  type    = bool
  default = true
}

variable "endpoint_public_access" {
  type    = bool
  default = true   # set false for fully private cluster
}

variable "public_access_cidrs" {
  type    = list(string)
  default = ["0.0.0.0/0"]   # restrict in prod
}

variable "tags" {
  type    = map(string)
  default = {}
}
