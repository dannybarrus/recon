# ─── AWS: PROVIDER ───────────────────────────────────────────────────────────
#
# AWS analog to 04_Azure/01_providers.tf
#
# Credentials are never hardcoded. Terraform reads from:
#   - Environment variables: AWS_ACCESS_KEY_ID, AWS_SECRET_ACCESS_KEY
#   - AWS CLI profile: ~/.aws/credentials
#   - IAM role (when running in EC2, ECS, Lambda, or GitHub Actions OIDC)
#
# Recommended: use OIDC in CI/CD and IAM roles in AWS compute.
# Never store AWS_SECRET_ACCESS_KEY in GitHub secrets if OIDC is available.

terraform {
  required_version = ">= 1.5.0"

  required_providers {
    aws = {
      source  = "hashicorp/aws"
      version = "~> 5.0"
    }
    random = {
      source  = "hashicorp/random"
      version = ">= 3.0"
    }
  }

  # Remote state: AWS S3 backend with DynamoDB locking
  # Analog to Azure Storage Account backend
  backend "s3" {
    bucket         = "tfstate-recon"           # globally unique S3 bucket
    key            = "prod/terraform.tfstate"
    region         = "us-east-1"
    dynamodb_table = "tfstate-lock-recon"      # DynamoDB table for state locking
    encrypt        = true                       # encrypt state at rest with KMS
  }
}

provider "aws" {
  region = var.region

  default_tags {
    tags = {
      environment = var.environment
      managed_by  = "terraform"
      team        = "platform"
    }
  }
}

# ─── Alias: multi-region ──────────────────────────────────────────────────────
# AWS analog to the azurerm provider alias for secondary region

provider "aws" {
  alias  = "secondary"
  region = var.secondary_region
}

variable "region" {
  type    = string
  default = "us-east-1"
}

variable "secondary_region" {
  type    = string
  default = "us-west-2"
}

variable "environment" {
  type    = string
  default = "dev"
}
