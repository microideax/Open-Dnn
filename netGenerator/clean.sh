#!/bin/bash


rm -rf paramExtractor/*.txt
rm -rf dse/*.txt
rm -rf netGen/*.txt
rm -rf netGen/*.h
rm -rf *.h

rm -rf ../gen_proj
rm -rf ./dse/__pycache__/
rm -rf ./dse/.idea/
rm -rf ./netGen/.idea/
rm -rf ./paramExtractor/.idea/

echo "Cleaned all the intermediate files and newly generated file!!!"
