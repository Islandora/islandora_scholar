#!/bin/bash

set -e

cd $(dirname $0)
cd ..

if [ -d tmp ]; then
  rm -fR tmp
fi
if [ -f citeproc.js ]; then
  rm citeproc.js
fi
mkdir tmp

cd src

function makepaths (){
    filepaths=""
	for f in $1; do
	    filepaths="$filepaths tmp/$f.js"
	done
}



files="load queue util_locale util_processor util_disambig util_nodes build state util_integration cmd_update cmd_bibliography cmd_cite node_bibliography node_choose node_citation node_date node_datepart node_elseif node_else node_etal node_group node_if node_info node_institution node_institutionpart node_key node_label node_layout node_macro node_name node_namepart node_names node_number node_sort node_substitute node_text attributes xmle4x stack util_abbrev util_parallel obj_token obj_ambigconfig obj_blob obj_number util util_datenode util_institutions util_names util_dates util_sort util_substitute util_number util_page util_flipflop formatters formats registry disambig_names disambig_cites disambig_citations"

makepaths "${files}"

for i in $files; do
	cat ${i}.js | sed -e "0,/^\(CSL\|\/\)/{/^\(CSL\|\/\)/p;d;}"  > ../tmp/NEW
	mv ../tmp/NEW ../tmp/${i}.js
	cat ../tmp/${i}.js | sed -e "/^[[:space:]]*\/\*/,/^[[:space:]]*\*\//d"  > ../tmp/NEW
	mv ../tmp/NEW ../tmp/NEW.js
	cat ../tmp/NEW.js | sed -e "/^[[:space:]]*load/d"  > ../tmp/NEW
	mv ../tmp/NEW ../tmp/NEW.js
	cat ../tmp/NEW.js | sed -e "/^[[:space:]]*$/d"  > ../tmp/NEW
	mv ../tmp/NEW ../tmp/NEW.js
	cat ../tmp/NEW.js | sed -e "/^\/\/SNIP-START/,/^\/\/SNIP-END/d"  > ../tmp/NEW
	mv ../tmp/NEW ../tmp/NEW.js
	cat ../tmp/NEW.js | sed -e "/^[[:space:]]*\/\/.*$/d"  > ../tmp/NEW
	mv ../tmp/NEW ../tmp/NEW.js
	cat ../tmp/NEW.js >> ../citeproc.js
done
rm ../tmp/NEW*

cd ..
echo $(cat citeproc.js | wc -l) lines in product

#cp citeproc.js rpc-stuff/src-js/citeproc-js.js

#cd rpc-stuff
#./citeproc-js-rpc.py
#cd ..

#rm stripped.js

## JSDoc toolkit is available from: http://code.google.com/p/jsdoc-toolkit/

if [ "$1" == "--upload" ]; then

    if [ -f jsdoc/jsdoc.log ]; then
        rm jsdoc/jsdoc.log
    fi

    if [ "--help" == "$1" ]; then
        java -jar jsdoc/jsrun.jar jsdoc/app/run.js --help
    else
        java -jar jsdoc/jsrun.jar jsdoc/app/run.js \
          -p \
          --out=jsdoc/jsdoc.log \
          --directory=citeproc-js-doc \
          -t=jsdoc/templates/jsdoc \
          $filepaths
    fi

    tar cfz citeproc-js-doc.tar.gz citeproc-js-doc
    scp citeproc-js-doc.tar.gz gsl-nagoya-u.net:/http/pub
    ssh gsl-nagoya-u.net ./citeproc-js-doc.sh
    rm citeproc-js-doc.tar.gz
    rm -fR citeproc-js-doc
fi

rm -fR tmp


