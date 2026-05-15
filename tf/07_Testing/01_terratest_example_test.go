// ─── TESTING: TERRATEST EXAMPLE ───────────────────────────────────────────────
// File: 07_Testing/network_module_test.go
//
// Terratest is a Go library for testing Terraform modules.
// It deploys real infrastructure, runs assertions, then destroys it.
//
// Requirements: Go 1.21+, Azure credentials, terraform in PATH
// Run: go test -v -timeout 30m ./07_Testing/...

package test

import (
	"testing"
	"strings"

	"github.com/gruntwork-io/terratest/modules/azure"
	"github.com/gruntwork-io/terratest/modules/random"
	"github.com/gruntwork-io/terratest/modules/terraform"
	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

// TestNetworkModule deploys the network module and validates the outputs.
func TestNetworkModule(t *testing.T) {
	t.Parallel()

	// Generate a unique suffix to avoid naming conflicts between parallel runs
	suffix := strings.ToLower(random.UniqueId())

	terraformOptions := &terraform.Options{
		TerraformDir: "../02_Modules/01_module_structure",

		Vars: map[string]interface{}{
			"resource_group_name": "rg-test-" + suffix,
			"location":            "East US",
			"environment":         "test",
			"subnet_count":        3,
		},

		// Retry on known transient errors
		MaxRetries:         3,
		TimeBetweenRetries: 5 * time.Second,
		RetryableTerraformErrors: map[string]string{
			".*ResourceNotFound.*": "Azure resource not yet available",
		},
	}

	// Ensure cleanup even if assertions fail
	defer terraform.Destroy(t, terraformOptions)

	// Deploy the infrastructure
	terraform.InitAndApply(t, terraformOptions)

	// ─── Assert outputs ────────────────────────────────────────────────────

	vnetId    := terraform.Output(t, terraformOptions, "vnet_id")
	subnetIds := terraform.OutputList(t, terraformOptions, "subnet_ids")

	assert.NotEmpty(t, vnetId,    "vnet_id output should not be empty")
	assert.Len(t, subnetIds, 3,   "should have created 3 subnets")

	// ─── Assert Azure resources actually exist ─────────────────────────────

	subscriptionId := azure.GetTargetAzureSubscription(t)

	vnetExists := azure.VirtualNetworkExists(t,
		"vnet-recon-test",
		"rg-test-"+suffix,
		subscriptionId)

	require.True(t, vnetExists, "VNet should exist in Azure")

	// Validate subnet address prefixes
	for i, subnetId := range subnetIds {
		assert.Contains(t, subnetId, "subnet-recon-test-"+string(rune('0'+i)),
			"subnet ID should contain expected name")
	}
}

// TestNetworkModuleValidation tests that invalid inputs are rejected.
func TestNetworkModuleValidation(t *testing.T) {
	t.Parallel()

	terraformOptions := &terraform.Options{
		TerraformDir: "../02_Modules/01_module_structure",

		Vars: map[string]interface{}{
			"resource_group_name": "rg-test-validation",
			"location":            "East US",
			"environment":         "invalid-env",   // should fail validation
		},
	}

	// Expect plan to fail due to variable validation
	_, err := terraform.InitAndPlanE(t, terraformOptions)
	assert.Error(t, err, "plan should fail for invalid environment value")
	assert.Contains(t, err.Error(), "validation", "error should mention validation")
}
