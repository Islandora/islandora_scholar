#!/bin/sh

if [ $(echo ${SHELL} | grep -c "bash")  -eq "1" ]; then
  export LANG="C"
else
  setenv LANG "en_US.UTF-8"
fi

START="$(date) <--------------START"
cd $(dirname $0)
RHINO="${PWD}"/rhino/js-1.7R2.jar
DOJO="${PWD}"/dojo/dojo/dojo.js
DOH="${PWD}"/dojo/util/doh/

TARGET="${PWD}"/tests/runners/rhino.js

if [ "$1" != "" ]; then
  if [ "$2" != "" ]; then
    OPT="CSL_OPTIONS={set:\"$1\", fixture:\"$2\"}"
    echo $OPT > run-opt.js
  else
    echo USAGE: $0 '[<"std"|"custom"> <test_name>]'
    exit 1
  fi
else
  echo "" > run-opt.js
fi

java -client -jar "${RHINO}" -opt 8 "${TARGET}" dojoUrl="${DOJO}"  testModule=""

rm -f run-opt.js
rm -f run-opt.jsxdr

echo $START
echo $(date) \<--------------END
