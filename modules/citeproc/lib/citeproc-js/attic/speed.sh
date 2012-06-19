#!/bin/sh
START="$(date) <--------------START"
cd $(dirname $0)
cd ..
RHINO="${PWD}"/rhino/js-1.7R1.jar
DOJO="${PWD}"/dojo/dojo/dojo.js
DOH="${PWD}"/dojo/util/doh/

TARGET="${PWD}"/tests/test_speed.js

java -client -jar "${RHINO}" "${TARGET}" dojoUrl="${DOJO}"  testModule="" 
echo $START
echo $(date) \<--------------END
