#!/usr/bin/python

import os,re
from xml.etree import ElementTree

def indent(elem, level=0):
    i = "\n" + level*"  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for elem in elem:
            indent(elem, level+1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i

bigret = {}
unique = {}

for filename in os.listdir("./all-styles/csl"):
    if not filename.endswith('.csl'): continue

    text = open("./all-styles/csl/%s" % filename).read()
    text = text.replace('xmlns="http://purl.org/net/xbiblio/csl"',"")
    et = ElementTree.fromstring(text)
    dates = et.findall(".//date")
    ret = {}
    for date in dates:
        kids = date.getchildren()
        if len(kids) > 0:
            if kids[0].attrib.has_key("prefix"):
                del kids[0].attrib["prefix"]
            if kids[-1].attrib.has_key("suffix"):
                del kids[-1].attrib["suffix"]
        for kid in kids:
            if kid.attrib["name"] == "month":
                if kid.attrib.has_key("form") and kid.attrib["form"] == "long":
                    del kid.attrib["form"]
            if kid.attrib["name"] == "year":
                if kid.attrib.has_key("form") and kid.attrib["form"] == "long":
                    del kid.attrib["form"]
            if kid.attrib["name"] == "day":
                if kid.attrib.has_key("form") and kid.attrib["form"] == "numeric":
                    del kid.attrib["form"]
        if date.attrib.has_key("font-weight"):
            del date.attrib["font-weight"]
        if date.attrib.has_key("font-style"):
            del date.attrib["font-style"]
        if date.attrib.has_key("variable"):
            del date.attrib["variable"]
        if date.attrib.has_key("delimiter"):
            del date.attrib["delimiter"]
        if date.attrib.has_key("prefix"):
            del date.attrib["prefix"]
        if date.attrib.has_key("suffix"):
            del date.attrib["suffix"]
        indent(date)
        key = ElementTree.tostring( date )
        ret[ key ] = True
        if unique.has_key(key):
            unique[ key ][1] += 1
            unique[ key ][0] = filename
        else:
            unique[ key ] = [filename, 1]
    ret = ret.keys()
    ret.sort()
    ret = "\n".join(ret)
    bigret[ ret ] = filename

for key in bigret.keys():
    open("date_forms/per_style/date_forms_%s" % bigret[key],"w+").write( key )

for key in unique.keys():
    open("date_forms/unique_forms/%0.2d_%s" % (unique[key][1],unique[key][0]),"w+").write(key)
