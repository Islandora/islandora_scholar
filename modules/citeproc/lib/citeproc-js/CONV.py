#!/usr/bin/python

import sys

text = open(sys.argv[1]).read().decode("utf8")

for char in text:
    if ord(char) > 256:
        c = hex(ord(char))[2:]
        while len(c) < 4:
            c = "0" + c
        
        sys.stdout.write("\u%s" % (c,))
    else:
        sys.stdout.write(char)
        
        
    
