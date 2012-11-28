#!/usr/bin/python
# -*- coding: utf-8 -*-

import os

def maj_file(filename):
    f = open(filename, 'r')
    c = f.read()
    f.close()

    c = c.split("\n")
    classe = None
    modified = False
    for i, line in enumerate(c):
        if line.startswith("classe "):
            classe = line.split(" ")[1]
        if classe == 'homothetie' and line.startswith('angle '):
            c[i] = line.replace("angle", "homo", 1)
            modified = True
    if modified:
        print "Modification de", filename
        f = open('xxx.maj_fichier_ne', 'w')
        f.write('\n'.join(c))
        f.close()
        os.rename(filename, filename + '~')
        os.rename('xxx.maj_fichier_ne', filename)
            
    

def maj(dummy_args, dirname, content):
    for filename in content:
        if filename.endswith('.ne'):
            maj_file(os.path.join(dirname, filename))


os.path.walk(".", maj, ())
