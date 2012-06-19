#!/usr/bin/env python
#-*- encoding: utf-8 -*-
""" Convert options to attributes in full test styles.

"""
import sys,os,re
from xml.etree import ElementTree

namespace = 'xmlns="http://purl.org/net/xbiblio/csl"'

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


def format_attributes(tag,str):
    m = re.match("(?m)(?s).*?(\s+|)<%s\s+([^>]*)>.*" % tag,str)
    if m:
        attribs = re.split("\s+", m.group(2))
        j = "\n      " + " " * len(m.group(1))                
        attribs = j + j.join(attribs)
        if tag == "style":
            attribs = j + namespace + attribs
        str = re.sub("(<"+tag+"\s+)[^>]*(>)","\\1"+attribs+"\\2",str)
    return str

def fix_source(mycsl):
    """ Convert options to attributes, write back to source file.
    """
    et = ElementTree.fromstring(mycsl)
    for tagname in ["citation", "bibliography"]:
        opts = et.findall(".//%s/option" % tagname)
        tag = et.findall(".//%s" % tagname)
        if tag and len(tag):
            tag = tag[0]
            for opt in opts:
                tag.attrib[opt.attrib["name"]] = opt.attrib["value"]
                tag.remove(opt)
    indent(et)
    str = ElementTree.tostring(et).strip()
    for tagname in ["style", "citation", "bibliography"]:
        str = format_attributes(tagname,str)
    return str

if __name__ == "__main__":

    for filename in os.listdir("./style"):
        print filename
        str = open("./style/%s" %filename).read()
        str = str.replace(namespace,"")
        str = fix_source(str)
        open("./style/%s" %filename,"w+").write(str)
