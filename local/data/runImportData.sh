#!/bin/bash

get_abs_filename() {
  # $1 : relative filename
  echo "$(cd "$(dirname "$0")" && pwd)/$(basename "$0")"
}

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo  $SCRIPT_DIR

cd $SCRIPT_DIR/sampleData

rm -rf *.data

find . -type f -name '*schedule*.dump' -exec cat {} + >> schedules.data
find . -type f -name '*affiliation*.dump' -exec cat {} + >> affiliations.data
find . -type f -name '*lineup*.dump' -exec cat {} + >> lineups.data
find . -type f -name '*market*.dump' -exec cat {} + >> markets.data
find . -type f -name '*station*.dump' -exec cat {} + >> stations.data
find . -type f -name '*image*.dump' -exec cat {} + >> images.data
find . -type f -name '*provider*.dump' -exec cat {} + >> providers.data

cd $SCRIPT_DIR

export PGPASSWORD=${1}
psql -h localhost -d postgres -U catapult_admin  -a -w -f import_data.sql;


