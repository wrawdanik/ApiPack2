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


for i in $(find -name \*.thrift); do
    sed -i 's/ major;/ major_;/g' "$i"
    sed -i 's/ major,/ major_,/g' "$i"
    sed -i 's/ operator,/ operator_,/g' "$i"
    thrift -r --gen cpp  "$i"
done

echo "Done. Look for c++ source code in gen-cpp  ..."
