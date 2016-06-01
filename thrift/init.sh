#!/bin/bash

rm -rf metamorphosisDataModel
rm -rf gen-py/
rm -rf gen-cpp/

git clone https://github.com/tms-engineering/metamorphosisDataModel.git


echo "Generating python source code  ..."

for i in $(find -name \*.thrift); do
    thrift -r --gen py "$i"
done


echo "Done. Look for python source code in gen-py ..."


echo "Generating c++ source code  ..."

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

mkdir gen-cpp

mkdir gen-cpp/TMSId-Programs
for i in $(find  metamorphosisDataModel/TMSId-Programs/*.thrift); do
    sed -i 's/ major;/ major_;/g' "$i"
    sed -i 's/ major,/ major_,/g' "$i"
    sed -i 's/ operator,/ operator_,/g' "$i"
    thrift -out $SCRIPT_DIR/gen-cpp/TMSId-Programs -r --gen cpp  "$i"
done

mkdir gen-cpp/core
for i in $(find  metamorphosisDataModel/core/*.thrift); do
    sed -i 's/ major;/ major_;/g' "$i"
    sed -i 's/ major,/ major_,/g' "$i"
    sed -i 's/ operator,/ operator_,/g' "$i"
    thrift -out $SCRIPT_DIR/gen-cpp/core -r --gen cpp  "$i"
done


mkdir gen-cpp/baseline
for i in $(find  metamorphosisDataModel/baseline/*.thrift); do
    sed -i 's/ major;/ major_;/g' "$i"
    sed -i 's/ major,/ major_,/g' "$i"
    sed -i 's/ operator,/ operator_,/g' "$i"
    thrift -out $SCRIPT_DIR/gen-cpp/baseline -r --gen cpp  "$i"
done

mkdir gen-cpp/merckx
for i in $(find  metamorphosisDataModel/merckx/*.thrift); do
    sed -i 's/ major;/ major_;/g' "$i"
    sed -i 's/ major,/ major_,/g' "$i"
    sed -i 's/ operator,/ operator_,/g' "$i"
    thrift -out $SCRIPT_DIR/gen-cpp/merckx -r --gen cpp  "$i"
done

echo "Done. Look for c++ source code in gen-cpp/core gen-cpp/baseline gen-cpp/merckx gen-cpp/TMSId-Programs  ..."
