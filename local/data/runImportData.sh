#!/bin/bash

get_abs_filename() {
  # $1 : relative filename
  echo "$(cd "$(dirname "$0")" && pwd)/$(basename "$0")"
}

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo  $SCRIPT_DIR

cd $SCRIPT_DIR/sampleData

rm -rf *.data

find . -type f -name '*metamorphosis.station*.dump' -exec cat {} + >> metamorphosis.station.data
find . -type f -name '*metamorphosis.lineup*.dump' -exec cat {} + >> metamorphosis.lineup.data
find . -type f -name '*metamorphosis.image*.dump' -exec cat {} + >> metamorphosis.image.data
find . -type f -name '*metamorphosis.person*.dump' -exec cat {} + >> metamorphosis.person.data
find . -type f -name '*metamorphosis.program.tmsId*.dump' -exec cat {} + >> metamorphosis.program.tmsId.data
find . -type f -name '*metamorphosis.affiliation*.dump' -exec cat {} + >> metamorphosis.affiliation.data
find . -type f -name '*metamorphosis.market*.dump' -exec cat {} + >> metamorphosis.market.data
find . -type f -name '*metamorphosis.schedule.day*.dump' -exec cat {} + >> metamorphosis.schedule.day.data
find . -type f -name '*metamorphosis.provider*.dump' -exec cat {} + >> metamorphosis.provider.data
find . -type f -name '*metamorphosis.headend*.dump' -exec cat {} + >> metamorphosis.headend.data
find . -type f -name '*metamorphosis.rating.body*.dump' -exec cat {} + >> metamorphosis.rating.body.data
find . -type f -name '*metamorphosis.rating.value*.dump' -exec cat {} + >> metamorphosis.rating.value.data
find . -type f -name '*metamorphosis.animation*.dump' -exec cat {} + >> metamorphosis.animation.data
find . -type f -name '*metamorphosis.award.type*.dump' -exec cat {} + >> metamorphosis.award.type.data
find . -type f -name '*metamorphosis.award.body*.dump' -exec cat {} + >> metamorphosis.award.body.data
find . -type f -name '*metamorphosis.genre*.dump' -exec cat {} + >> metamorphosis.genre.data
find . -type f -name '*metamorphosis.production.company*.dump' -exec cat {} + >> metamorphosis.production.company.data
find . -type f -name '*metamorphosis.advisory*.dump' -exec cat {} + >> metamorphosis.advisory.data
find . -type f -name '*metamorphosis.holiday*.dump' -exec cat {} + >> metamorphosis.holiday.data
find . -type f -name '*metamorphosis.playoff*.dump' -exec cat {} + >> metamorphosis.playoff.data
find . -type f -name '*metamorphosis.team*.dump' -exec cat {} + >> metamorphosis.team.data
find . -type f -name '*metamorphosis.venue*.dump' -exec cat {} + >> metamorphosis.venue.data
find . -type f -name '*metamorphosis.warning*.dump' -exec cat {} + >> metamorphosis.warning.data
find . -type f -name '*metamorphosis.conference*.dump' -exec cat {} + >> metamorphosis.conference.data
find . -type f -name '*metamorphosis.university*.dump' -exec cat {} + >> metamorphosis.university.data

cd $SCRIPT_DIR

export PGPASSWORD=${1}
psql -h localhost -d postgres -U catapult_admin  -a -w -f import_data.sql;


