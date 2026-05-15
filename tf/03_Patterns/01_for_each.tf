# ─── PATTERN: for_each vs count ──────────────────────────────────────────────
#
# count creates resources indexed by integer: resource[0], resource[1]
# for_each creates resources indexed by key: resource["name"]
#
# Prefer for_each when:
#   - Items have natural names (not just numbers)
#   - Items may be added/removed without affecting others
#   - Removing item[0] with count shifts all subsequent items -- dangerous
#   - Removing a key with for_each only removes that one resource

variable "subnets" {
  type = map(object({
    cidr = string
    purpose = string
  }))
  default = {
    aks     = { cidr = "10.0.1.0/24", purpose = "AKS node pool" }
    ingress = { cidr = "10.0.2.0/24", purpose = "Ingress controller" }
    data    = { cidr = "10.0.3.0/24", purpose = "Data tier" }
  }
}

# for_each over a map -- each.key and each.value available inside
resource "azurerm_subnet" "named" {
  for_each = var.subnets

  name                 = "subnet-${each.key}"
  resource_group_name  = azurerm_resource_group.main.name
  virtual_network_name = azurerm_virtual_network.main.name
  address_prefixes     = [each.value.cidr]

  # Reference: azurerm_subnet.named["aks"].id
  # Reference: azurerm_subnet.named["ingress"].id
}

# for_each over a set of strings
variable "environments" {
  type    = set(string)
  default = ["dev", "staging", "prod"]
}

resource "azurerm_resource_group" "envs" {
  for_each = var.environments

  name     = "rg-recon-${each.value}"
  location = var.location
}
