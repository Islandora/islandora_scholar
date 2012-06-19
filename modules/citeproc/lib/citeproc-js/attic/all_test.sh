#!/bin/bash

cd $(dirname $0)

set -e

cd ..

if [ ! -d all-styles ]; then
  mkdir all-styles
  echo "** Created subdirectory ./all-styles in the citeproc-js root directory"
fi

cd all-styles

if [ ! -d csl ]; then
  echo "** Script error: style sources not found"
  echo "-- Enter the all-styles subdirectory and issue the following"
  echo "-- command: "
  echo 
  echo "     svn co https://www.zotero.org/svn/csl"
  echo
  exit 1
fi

cd ../tests

echo "** Generating test file tests/test_load_all_styles.js"

cat > test_load_all_styles.js <<EOF
dojo.provide("tests.test_load_all_styles");


var tryStyle = function(style){
	try {
		var sty = readFile("all-styles/csl/"+style+".csl");
		if (!sty){
			throw "Did not find style file: all-styles/csl/"+style+".csl";
		}
		var builder = new CSL.Core.Build(sty);
		var res = builder.build();
	} catch(e) {
		print("oops: "+e);
	}
	return res;
}

doh.register("tests.load_styles", [
EOF

for i in $(ls ../all-styles/csl/*.csl); do
  STYLENAME=$(basename $i .csl)
  cat >> test_load_all_styles.js <<EOF
 function(){
		var res = tryStyle("${STYLENAME}");
		doh.assertTrue( res );
	},
EOF
done

cat >> test_load_all_styles.js <<EOF
]);
EOF
