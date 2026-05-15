# ─── TESTING: MOCK PROVIDER ───────────────────────────────────────────────────
# File: 07_Testing/02_mock_provider.tf
#
# Terraform 1.7+ supports mock providers for unit testing modules without
# deploying real infrastructure. Tests run in milliseconds, no cloud costs.
#
# Mock providers return fake data for data sources and planned values for
# resources. Use them to test logic -- variable validation, locals,
# conditional resources, for_each -- without a real cloud connection.
#
# Run: terraform test

# ─── Test file: network_module.tftest.hcl ────────────────────────────────────
#
# (Terraform test files use .tftest.hcl extension)
# Place this alongside the module being tested.

# mock_provider "azurerm" {
#   mock_data "azurerm_client_config" {
#     defaults = {
#       subscription_id = "00000000-0000-0000-0000-000000000000"
#       tenant_id       = "11111111-1111-1111-1111-111111111111"
#       object_id       = "22222222-2222-2222-2222-222222222222"
#     }
#   }
# }
#
# # Test: default subnet count creates 3 subnets
# run "default_subnet_count" {
#   command = plan   # plan only, no apply
#
#   variables {
#     resource_group_name = "rg-test"
#     location            = "East US"
#     environment         = "dev"
#   }
#
#   assert {
#     condition     = length(azurerm_subnet.main) == 3
#     error_message = "Expected 3 subnets with default subnet_count"
#   }
# }
#
# # Test: prod environment creates correct naming
# run "prod_naming" {
#   command = plan
#
#   variables {
#     resource_group_name = "rg-test"
#     location            = "East US"
#     environment         = "prod"
#   }
#
#   assert {
#     condition     = azurerm_virtual_network.main.name == "vnet-recon-prod"
#     error_message = "VNet name should follow naming convention for prod"
#   }
# }
#
# # Test: invalid environment fails validation
# run "invalid_environment" {
#   command = plan
#
#   variables {
#     resource_group_name = "rg-test"
#     location            = "East US"
#     environment         = "production"   # invalid -- should fail
#   }
#
#   expect_failures = [var.environment]
# }

# ─── Why mock providers matter ────────────────────────────────────────────────
#
# Terratest (integration tests): deploys real infrastructure, takes 5-30 min,
#   costs money, requires credentials. Tests the full stack end to end.
#
# Mock providers (unit tests): no deployment, runs in seconds, free.
#   Tests logic: variable validation, locals, conditionals, for_each.
#
# Best practice: unit tests for logic, integration tests for critical modules.
# Run unit tests on every PR, integration tests nightly or pre-release.
