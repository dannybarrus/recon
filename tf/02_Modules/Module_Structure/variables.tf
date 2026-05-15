# ─── MODULE: network ─────────────────────────────────────────────────────────
# File: modules/network/variables.tf
#
# Input variables are the public interface of this module.
# Callers must supply values for variables with no default.
# Callers may override variables that have defaults.

variable "resource_group_name" {
  type        = string
  description = "Name of the resource group to deploy into"
}

variable "location" {
  type        = string
  description = "Azure region"
}

variable "environment" {
  type        = string
  description = "Deployment environment: dev, staging, prod"
}

variable "vnet_address_space" {
  type        = list(string)
  description = "Address space for the virtual network"
  default     = ["10.0.0.0/16"]
}

variable "subnet_count" {
  type        = number
  description = "Number of subnets to create"
  default     = 3
}

variable "tags" {
  type        = map(string)
  description = "Tags to apply to all resources"
  default     = {}
}
