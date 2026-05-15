# ─── AWS: VPC AND SUBNETS ────────────────────────────────────────────────────
#
# AWS analog to 04_Azure/02_vnet_subnet.tf
#
# Azure concept -> AWS concept
# Resource Group -> No direct equivalent (tags serve a similar purpose)
# Virtual Network -> VPC (Virtual Private Cloud)
# Subnet          -> Subnet (same concept, scoped to an Availability Zone)
# NSG             -> Security Group / Network ACL
# Service Endpoints -> VPC Endpoints

# ─── VPC ─────────────────────────────────────────────────────────────────────

resource "aws_vpc" "main" {
  cidr_block           = "10.0.0.0/16"
  enable_dns_hostnames = true   # required for EKS
  enable_dns_support   = true

  tags = {
    Name = "vpc-recon-${var.environment}"
    # EKS requires these tags on the VPC to discover it
    "kubernetes.io/cluster/eks-recon-${var.environment}" = "shared"
  }
}

# ─── Internet Gateway ────────────────────────────────────────────────────────
# Analog to Azure's built-in internet routing.
# AWS requires an explicit Internet Gateway for public subnet routing.

resource "aws_internet_gateway" "main" {
  vpc_id = aws_vpc.main.id
  tags   = { Name = "igw-recon-${var.environment}" }
}

# ─── Data: Availability Zones ────────────────────────────────────────────────
# Dynamically fetch AZs for the current region.
# Analog to Azure availability zones being implicit in region config.

data "aws_availability_zones" "available" {
  state = "available"
}

# ─── Public Subnets (one per AZ) ─────────────────────────────────────────────
# Host load balancers and NAT gateways.
# Analog to Azure's ingress subnet.

resource "aws_subnet" "public" {
  count = 3

  vpc_id                  = aws_vpc.main.id
  cidr_block              = cidrsubnet("10.0.0.0/16", 8, count.index)
  availability_zone       = data.aws_availability_zones.available.names[count.index]
  map_public_ip_on_launch = true

  tags = {
    Name = "subnet-public-${count.index}-recon-${var.environment}"
    # Required for EKS to discover and use these subnets for load balancers
    "kubernetes.io/cluster/eks-recon-${var.environment}" = "shared"
    "kubernetes.io/role/elb"                             = "1"
  }
}

# ─── Private Subnets (one per AZ) ────────────────────────────────────────────
# Host EKS nodes and application workloads.
# Analog to Azure's AKS subnet.

resource "aws_subnet" "private" {
  count = 3

  vpc_id            = aws_vpc.main.id
  cidr_block        = cidrsubnet("10.0.0.0/16", 8, count.index + 10)
  availability_zone = data.aws_availability_zones.available.names[count.index]

  tags = {
    Name = "subnet-private-${count.index}-recon-${var.environment}"
    # Required for EKS to discover these subnets for node groups
    "kubernetes.io/cluster/eks-recon-${var.environment}" = "shared"
    "kubernetes.io/role/internal-elb"                    = "1"
  }
}

# ─── NAT Gateway ─────────────────────────────────────────────────────────────
# Allows private subnet resources to reach the internet outbound.
# Analog to Azure's default outbound routing via load balancer.
# Note: NAT Gateways are expensive -- one per AZ for HA in prod.

resource "aws_eip" "nat" {
  count  = local.is_prod ? 3 : 1
  domain = "vpc"
  tags   = { Name = "eip-nat-${count.index}-recon-${var.environment}" }
}

resource "aws_nat_gateway" "main" {
  count = local.is_prod ? 3 : 1

  allocation_id = aws_eip.nat[count.index].id
  subnet_id     = aws_subnet.public[count.index].id
  tags          = { Name = "nat-${count.index}-recon-${var.environment}" }
}

# ─── Route Tables ─────────────────────────────────────────────────────────────
# Explicit route tables required in AWS -- no implicit routing like Azure.

resource "aws_route_table" "public" {
  vpc_id = aws_vpc.main.id
  route {
    cidr_block = "0.0.0.0/0"
    gateway_id = aws_internet_gateway.main.id
  }
  tags = { Name = "rt-public-recon-${var.environment}" }
}

resource "aws_route_table_association" "public" {
  count          = 3
  subnet_id      = aws_subnet.public[count.index].id
  route_table_id = aws_route_table.public.id
}

resource "aws_route_table" "private" {
  count  = local.is_prod ? 3 : 1
  vpc_id = aws_vpc.main.id
  route {
    cidr_block     = "0.0.0.0/0"
    nat_gateway_id = aws_nat_gateway.main[count.index].id
  }
  tags = { Name = "rt-private-${count.index}-recon-${var.environment}" }
}

resource "aws_route_table_association" "private" {
  count          = 3
  subnet_id      = aws_subnet.private[count.index].id
  route_table_id = aws_route_table.private[local.is_prod ? count.index : 0].id
}

locals {
  is_prod = var.environment == "prod"
}

# ─── Outputs ─────────────────────────────────────────────────────────────────

output "vpc_id"             { value = aws_vpc.main.id                      }
output "public_subnet_ids"  { value = aws_subnet.public[*].id              }
output "private_subnet_ids" { value = aws_subnet.private[*].id             }
