#!/usr/bin/env python

import sys,os,re

def setpath():
    mypath = os.path.split(sys.argv[0])[0]
    if len(mypath):
        os.chdir(mypath)

def getfiles():
    files = []
    for f in os.listdir("locale"):
        if not f.endswith(".xml"):
            continue
        files.append( "locale/%s" % f )
    return files

top = """
<locale xml:lang="%s" xmlns="http://purl.org/net/xbiblio/csl">
  <terms>
""".strip()

bottom = """
  </terms>
</locale>
""".strip()

def conv(path):
    str = open(path).read()
    m = re.match("(?m)(?s).*<terms.*<locale[^>]*xml:lang=\"([^\"]+)\"[^>]*>.*",str)
    if m:
        lang = m.group(1)
        print lang
        str = re.sub("(?m)(?s)<terms.*<locale[^>]+>",top %lang,str)
        str = re.sub("(?m)(?s)</locale>.*</terms>",bottom,str)
        #print str
        open(path,"w+").write(str)
    else:
        print "Oops.  term/locale elements not found."
        sys.exit()

if __name__ == "__main__":
    setpath()
    for path in getfiles():
        n = conv(path)
        #break
    print "Done."
