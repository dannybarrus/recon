# 07_Testing

Testing strategies for Terraform modules — from unit tests that run in milliseconds to integration tests that deploy real infrastructure.

## Files

| File | Type | Language | Speed |
|---|---|---|---|
| `01_terratest_example_test.go` | Integration test | Go | 5–30 minutes |
| `02_mock_provider.tf` | Unit test (mock provider) | HCL | Seconds |

## Terratest (Integration Tests)

Terratest is a Go library that deploys real infrastructure, runs assertions against it, and destroys it. Use it for:

- Validating that a module actually creates the expected resources in Azure or AWS
- Testing that outputs contain correct values
- End-to-end validation of critical modules before release

Key practices shown:
- `t.Parallel()` for concurrent test runs
- `defer terraform.Destroy()` to guarantee cleanup even on test failure
- Retry configuration for transient Azure/AWS API errors
- Assertions using both Terraform outputs and direct Azure SDK calls

## Mock Providers (Unit Tests)

Terraform 1.7+ supports mock providers that return fake data without making real cloud API calls. Use them for:

- Testing variable validation rules
- Testing local value calculations
- Testing conditional resource creation (`count = var.enabled ? 1 : 0`)
- Testing `for_each` logic

## When to Use Each

| Concern | Use |
|---|---|
| Does the logic produce the right plan? | Mock provider |
| Does the module actually work in Azure/AWS? | Terratest |
| Does a variable validation catch bad input? | Mock provider |
| Do the outputs contain the right resource IDs? | Terratest |

Run mock/unit tests on every PR. Run Terratest integration tests nightly or pre-release.
