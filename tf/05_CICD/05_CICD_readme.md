# 05_CICD

GitHub Actions workflows for automating Terraform plan, apply, and drift detection.

## Files

| File | Trigger | Purpose |
|---|---|---|
| `01_github_actions_plan.yml` | Pull request to `main` | Runs `terraform plan`, posts output as a PR comment |
| `02_github_actions_apply_and_drift.yml` | Merge to `main` / daily cron | Runs `terraform apply` on merge; detects drift on a schedule |

## Authentication

Both workflows use OIDC (OpenID Connect) rather than storing long-lived credentials in GitHub secrets. The GitHub Actions runner assumes an Azure or AWS IAM role via federated identity — no `ARM_CLIENT_SECRET` or `AWS_SECRET_ACCESS_KEY` required.

## Plan Workflow

- Runs `terraform fmt -check` to enforce formatting
- Runs `terraform validate` for syntax and schema checks
- Runs `terraform plan -out=tfplan` and saves the artifact
- Posts the plan output as a PR comment for team review
- Fails the PR check if the plan errors

## Apply Workflow

- Runs on merge to `main` — requires approval via GitHub Environments
- Applies the configuration directly (not the saved plan artifact, to keep it simple)
- Opens a GitHub issue automatically if apply fails

## Drift Detection

- Runs daily at 6am UTC via cron schedule
- Uses `terraform plan -detailed-exitcode` — exit code 2 means changes detected
- Opens a GitHub issue when drift is found, skipping if one is already open
- Drift occurs when infrastructure is changed outside Terraform (portal, CLI, another tool)
