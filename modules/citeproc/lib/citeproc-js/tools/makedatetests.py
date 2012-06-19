#!/usr/bin/python
#-*- encoding: utf-8 -*-

import os,sys

mypath = os.path.split(sys.argv[0])[0]
if len(mypath):
    os.chdir(mypath)

datalst = open("../tools/dates.txt").read().decode("utf8").split('\n')

tests = []

template1 = '''
dojo.provide("citeproc_js.dateparse");

var sys = new RhinoTest();
var citeproc = new CSL.Engine(sys,"<style></style>");

var keycount = function(obj){
    var c=0;
    for (pos in obj) {
      c+=1;
    }
    return c;
};

doh.register("tests.dateparse", [%s
]);

'''

template2 = '''
        doh.assertEqual("%s", res["%s"]);
'''.rstrip()

template4 = '''
        doh.assertEqual(%d, keycount(res) );
'''.rstrip()

template3 = '''
    function test_dateparse%0.3d() {
        var res = citeproc.fun.dateparser.parse("%s");%s
    }
'''.strip()

asserts = 0

for line in datalst:
    line = line.strip()
    if not line: continue
    newline = "";
    #line = line.decode("utf-8")
    if line.startswith("#"): continue
    if line.startswith("-->"):
        line = line[3:].strip()
        line = line.split(",")
        for pos in range(len(line)-1,0,-1):
            if line[pos-1].endswith("\\"):
                line[pos-1] = "%s,%s" % (line[pos-1][:-1],line[pos])
                line.pop(pos)
        m = {}
        for chunk in line:
            #print "(%s)" %chunk
            t = chunk.split("=")
            #print chunk
            t[1] = t[1].strip('"')
            m[t[0]] = t[1]
        continue
    test = {}
    test["input"] = line
    test["result"] = m
    tests.append(test)

results3 = []

for pos in range(0, len(tests), 1):
    test = tests[pos]
    results2 = ''
    asserts = 0
    for key in test["result"].keys():
        asserts += 1
        results2 += template2 % (test["result"][key], key)
    results2 += template4 % asserts
    newinput = test["input"]
    #newinput = ""
    #for c in test["input"]:
    #    if ord(c) > 128:
    #        num = hex(ord(c))[2:]
    #        if len(num) < 4:
    #            num = '%s%s' % ('0'*(4-len(num)),num)
    #            newinput += '\u%s' % (num,)
    #    else:
    #        newinput += c
    results3.append( template3 % ((pos+1),newinput,results2) )

final = template1 % (',\n    '.join( results3 ),)

ofh = open("../tests/citeproc-js/dateparse.js","w+")

for char in final:
    if ord(char) > 256:
        c = hex(ord(char))[2:]
        while len(c) < 4:
            c = "0" + c
        
        ofh.write("\u%s" % (c,))
    else:
        ofh.write(char)

print "Done!"
