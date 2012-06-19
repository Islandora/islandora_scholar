#!/usr/bin/env python

import sys,os,re

mystr = open("bluebook-demo.csl").read()

mystr = re.sub("\n","",mystr)
mystr = re.sub('"','\\"',mystr)

mynew = open("loadcsl.src").read()
mynew = mynew.replace("%%bluebook_demo%%",mystr)
open("loadcsl.js","w+").write(mynew)
