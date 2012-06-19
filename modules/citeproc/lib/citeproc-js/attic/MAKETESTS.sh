#!/bin/bash

set -e

cd $(dirname $0)
cd ..
#
# Be sure the machine side of the test suite is up to date
#
tests/grind.py


#
# Start Gunk
#
OLDIFS=$IFS
IFS=""
HEADER=`cat<<EOF
dojo.provide("std.::CATEGORY::");

doh.register("std.::CATEGORY::", [
EOF
`
FOOTER=`cat<<EOF
]);
EOF
`
IFS=$OLDIFS

header (){
  OLDIFS=$IFS
  IFS=""
  echo $HEADER
  IFS=$OLDIFS
}

footer (){
  OLDIFS=$IFS
  IFS=""
  echo $FOOTER
  IFS=$OLDIFS
}
#
# End Gunk
#

#
# Initialize files
#
rm -f tests/std/bundled/*.js
for i in tests/std/machines/*.json; do
    BASE=$(basename $i .json)
    CATEGORY=$(echo ${BASE} | sed -e "s/^\([^_]\+\)_.*/\\1/")
    if [ ! -f "tests/std/bundled/"${CATEGORY}".js" ]; then
        OLDIFS=$IFS
        IFS=""
        echo $(header) | sed -e "s/::CATEGORY::/${CATEGORY}/" > "tests/std/bundled/"${CATEGORY}".js"
        IFS=$OLDIFS
    fi
done

for i in tests/std/machines/*.json; do
    BASE=$(basename $i .json)
    CATEGORY=$(echo ${BASE} | sed -e "s/^\([^_]\+\)_.*/\\1/")
	echo '    function(){' >> "tests/std/bundled/"${CATEGORY}".js"
    echo '        var test = new StdRhinoTest("'${BASE}'");' >> "tests/std/bundled/"${CATEGORY}".js"
    echo '        doh.assertEqual(test.result, test.run());' >> "tests/std/bundled/"${CATEGORY}".js"
	echo '    },' >> "tests/std/bundled/"${CATEGORY}".js"
done

for i in tests/std/bundled/*.js; do
    OLDIFS=$IFS
    IFS=""
    echo $(footer) >> ${i}
    IFS=$OLDIFS
done

echo Bundling ./citeproc.js
tools/bundle.sh

echo Applying license text all over the place
tools/APPLY_LICENSE.py
