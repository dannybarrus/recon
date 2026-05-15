# ─── PATTERN: Dynamic blocks ─────────────────────────────────────────────────
#
# Dynamic blocks generate repeated nested blocks inside a resource.
# Useful when the number of nested blocks is determined at runtime.

variable "nsg_rules" {
  type = list(object({
    name                       = string
    priority                   = number
    direction                  = string
    access                     = string
    protocol                   = string
    source_port_range          = string
    destination_port_range     = string
    source_address_prefix      = string
    destination_address_prefix = string
  }))
  default = [
    {
      name                       = "allow-https"
      priority                   = 100
      direction                  = "Inbound"
      access                     = "Allow"
      protocol                   = "Tcp"
      source_port_range          = "*"
      destination_port_range     = "443"
      source_address_prefix      = "*"
      destination_address_prefix = "*"
    },
    {
      name                       = "allow-http"
      priority                   = 110
      direction                  = "Inbound"
      access                     = "Allow"
      protocol                   = "Tcp"
      source_port_range          = "*"
      destination_port_range     = "80"
      source_address_prefix      = "*"
      destination_address_prefix = "*"
    }
  ]
}

resource "azurerm_network_security_group" "dynamic" {
  name                = "nsg-dynamic-example"
  resource_group_name = var.resource_group_name
  location            = var.location

  # Dynamic block generates one security_rule block per item in var.nsg_rules
  dynamic "security_rule" {
    for_each = var.nsg_rules
    content {
      name                       = security_rule.value.name
      priority                   = security_rule.value.priority
      direction                  = security_rule.value.direction
      access                     = security_rule.value.access
      protocol                   = security_rule.value.protocol
      source_port_range          = security_rule.value.source_port_range
      destination_port_range     = security_rule.value.destination_port_range
      source_address_prefix      = security_rule.value.source_address_prefix
      destination_address_prefix = security_rule.value.destination_address_prefix
    }
  }
}

# ─── PATTERN: Conditionals ────────────────────────────────────────────────────
#
# count = 0 or 1 is the standard pattern for optional resources.
# The ternary operator drives the decision.

variable "enable_bastion" {
  type    = bool
  default = false
}

variable "environment" {
  type    = string
  default = "dev"
}

# Optional resource -- only created when enable_bastion is true
resource "azurerm_bastion_host" "main" {
  count = var.enable_bastion ? 1 : 0

  name                = "bastion-recon"
  resource_group_name = var.resource_group_name
  location            = var.location

  ip_configuration {
    name                 = "configuration"
    subnet_id            = var.bastion_subnet_id
    public_ip_address_id = azurerm_public_ip.bastion[0].id
  }
}

resource "azurerm_public_ip" "bastion" {
  count = var.enable_bastion ? 1 : 0

  name                = "pip-bastion-recon"
  resource_group_name = var.resource_group_name
  location            = var.location
  allocation_method   = "Static"
  sku                 = "Standard"
}

# Reference optional resource safely
# Use one() to convert a list of 0 or 1 to a single value or null
output "bastion_dns_name" {
  value       = one(azurerm_bastion_host.main[*].dns_name)
  description = "Bastion host DNS name (null if bastion is disabled)"
}

# ─── PATTERN: Environment-specific sizing ─────────────────────────────────────

locals {
  is_prod = var.environment == "prod"

  aks_node_count = local.is_prod ? 5 : 2
  aks_vm_size    = local.is_prod ? "Standard_D8s_v3" : "Standard_D2s_v3"
  aks_sku        = local.is_prod ? "Standard" : "Free"
}

variable "resource_group_name" { type = string }
variable "location"            { type = string }
variable "bastion_subnet_id"   { type = string  default = "" }
