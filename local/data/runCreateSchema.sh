#!/bin/bash

export PGPASSWORD=${1}

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
psql -h localhost -d postgres -U postgres  -a -w -f $SCRIPT_DIR/schema.sql

