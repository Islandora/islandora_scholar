#!/usr/bin/python


import re,os

rex = re.compile("(.*\s*)(<terms>)(\s*\n\s*)(<locale [^>]+>)(.*)(</locale>)(\s*\n\s*)(</terms>)(.*)",re.M|re.S)

for filename in os.listdir("./locale"):
    if not filename.endswith(".xml"): continue

    fulltext = open("locale/%s" % filename).read()
    groups = rex.match(fulltext)

    if groups:
        print filename
        fulltext = "%s%s%s%s%s%s%s%s%s" % (groups.group(1),
                                       groups.group(4),
                                       groups.group(3),
                                       groups.group(2),
                                       groups.group(5),
                                       groups.group(8),
                                       groups.group(7),
                                       groups.group(6),
                                       groups.group(9))

        open("locale/%s" % filename,"w+").write(fulltext)
