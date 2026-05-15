# ─── VARIABLES ───────────────────────────────────────────────────────────────
#
# Input variables are the parameters of a Terraform module. They make
# configurations reusable and environment-agnostic.
#
# Values are supplied via:
#   - terraform.tfvars or *.auto.tfvars files
#   - -var="key=value" on the command line
#   - TF_VAR_<name> environment variables
#   - Variable defaults (fallback when nothing else is supplied)

# ─── String variable ──────────────────────────────────────────────────────────

variable "environment" {
  type        = string
  description = "Deployment environment: dev, staging, or prod"
  default     = "dev"

  validation {
    condition     = contains(["dev", "staging", "prod"], var.environment)
    error_message = "Environment must be one of: dev, staging, prod."
  }
}

# ─── String with no default (required) ───────────────────────────────────────

variable "location" {
  type        = string
  description = "Azure region for all resources"
  # no default — caller must supply this
}

# ─── Number variable ──────────────────────────────────────────────────────────

variable "node_count" {
  type        = number
  description = "Number of nodes in the AKS default node pool"
  default     = 3

  validation {
    condition     = var.node_count >= 1 && var.node_count <= 100
    error_message = "node_count must be between 1 and 100."
  }
}

# ─── Boolean variable ─────────────────────────────────────────────────────────

variable "enable_monitoring" {
  type        = bool
  description = "Whether to enable Azure Monitor and Log Analytics"
  default     = true
}

# ─── List variable ────────────────────────────────────────────────────────────

variable "allowed_ip_ranges" {
  type        = list(string)
  description = "CIDR ranges allowed to access the cluster API server"
  default     = []
}

# ─── Map variable ─────────────────────────────────────────────────────────────

variable "tags" {
  type        = map(string)
  description = "Tags applied to all resources"
  default = {
    managed_by = "terraform"
    team       = "platform"
  }
}

# ─── Object variable ─────────────────────────────────────────────────────────

variable "aks_config" {
  type = object({
    vm_size           = string
    node_count        = number
    os_disk_size_gb   = number
    enable_auto_scale = bool
    min_count         = optional(number, 1)
    max_count         = optional(number, 10)
  })
  description = "AKS node pool configuration"
  default = {
    vm_size           = "Standard_D4s_v3"
    node_count        = 3
    os_disk_size_gb   = 128
    enable_auto_scale = true
  }
}

# ─── Sensitive variable ───────────────────────────────────────────────────────
#
# Marked sensitive: value is redacted in plan output and logs.
# Does NOT encrypt the value in state — use Key Vault for real secrets.

variable "admin_password" {
  type        = string
  description = "Administrator password for the jump host"
  sensitive   = true
}
