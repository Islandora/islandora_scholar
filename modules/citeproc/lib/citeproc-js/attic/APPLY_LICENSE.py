#!/usr/bin/python

import re,os,sys

mypath = os.path.split(sys.argv[0])[0]
os.chdir(mypath)

REX = "(?sm)^^(/\*.*?^\s*\*/\n*)(.*)"

m = re.match(REX, open("../src/load.js").read())

if m:
    license = m.group(1).strip()
else:
    print "Oops, no license in csl.js"
    sys.exit()

print license

def process_file(path,file):
    filepath = "%s/%s" % (path,file)
    if not filepath.endswith(".js") and not filepath.endswith(".txt") and not filepath.endswith(".json") and not filepath.endswith("README.txt"): return
    text = open(filepath).read()
    oldtext = text
    m = re.match(REX,text)
    if m:
        text = "%s\n%s" % (license, m.group(2))
    else:
        text = "%s%s" % (license, text)
    if text.strip() != oldtext.strip():
        open("%s" %(filepath),"w+").write(text)

for path in ["..", "../src", "../tests/std", "../tests/std/humans","../tests/std/bundled", "../tests/std/machines","../tests/citeproc-js"]:
    for file in os.listdir( path ):
        process_file(path,file)

print "Done"

