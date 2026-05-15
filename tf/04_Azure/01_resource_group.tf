# ─── AZURE: RESOURCE GROUP ────────────────────────────────────────────────────

variable "environment" { type = string  default = "dev" }
variable "location"    { type = string  default = "East US" }

resource "azurerm_resource_group" "main" {
  name     = "rg-recon-${var.environment}"
  location = var.location
  tags     = { environment = var.environment, managed_by = "terraform" }
}

output "resource_group_name" { value = azurerm_resource_group.main.name }
output "resource_group_id"   { value = azurerm_resource_group.main.id   }
