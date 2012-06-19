
#!/bin/sh

if [ $(echo ${SHELL} | grep -c "bash")  -eq "1" ]; then
  echo LANG in bash
  export LANG="en_US.UTF-8"
else
  echo LANG in something else
  setenv LANG "en_US.UTF-8"
fi

START="$(date) <--------------START"
cd $(dirname $0)
TRACEMONKEY=/home/bennett/src/jslibs/Linux_32_opt/jshost
DOJO="${PWD}"/dojo-sm/dojo/dojo.js
DOH="${PWD}"/dojo-sm/util/doh/

TARGET="${PWD}"/tests/runners/tracemonkey.js

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

"${TRACEMONKEY}" -u "${TARGET}" 

rm -f run-opt.js
rm -f run-opt.jsxdr

echo $START
echo $(date) \<--------------END
