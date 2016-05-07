#!/bin/bash

rm -rf metamorphosisDataModel
rm -rf gen-py/

git clone https://github.com/tms-engineering/metamorphosisDataModel.git


echo "Generating python source code  ..."

for i in $(find -name \*.thrift); do # Not recommended, will break on whitespace
    thrift -r --gen py "$i"
done


echo "Done. Look for python source code in gen-py ..."
