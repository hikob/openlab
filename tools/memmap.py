#! /usr/bin/env python
##
## memmap.py
## Copyright HiKoB 2012
## Author: Antoine Fraboulet
##
## Generates charts for .text, .rodata, .bss and .data sections
##
##

import os, sys
from pylab import *

from optparse import OptionParser
from subprocess import Popen, PIPE, STDOUT

import jstreemap

## ##################################################
## ##################################################

import re
pretty_re = re.compile("([^/(]+)(?:\((.+)\))?$")

def pretty(s):
  g = pretty_re.search(s).groups()
  return g[1] or g[0]

def pretty2(s):
  return os.path.basename(s)

## ##################################################
## ##################################################

def draw_chart(fig, name, labels, sizes, png):
    # make a square figure and axes
    matplotlib.pyplot.figure(fig, figsize=(10,10))
    ax = matplotlib.pyplot.axes([0.1, 0.1, 0.8, 0.8])
    
    matplotlib.pyplot.pie(sizes, labels=labels,
                          colors= ('#707070', '#808080', '#909090', '#A0A0A0', '#B0B0B0', '#C0C0C0',
                                   '#101010', '#202020', '#303030', '#404040', '#505050', '#606060',
                                   '#D0D0D0', '#E0E0E0', '#F0F0F0'),
                          autopct='%1.1f%%',
                          #pctdistance = 1.1,
                          #labeldistance = 1.2,
                          shadow=False)
    
    matplotlib.pyplot.title('Memory map for ' + name + " (%d bytes)" % sum(sizes))
    # , bbox={'facecolor':'0.8', 'pad':5})
    # matplotlib.pyplot.autumn()
    matplotlib.pyplot.savefig(png)
    return


## ##################################################
## ##################################################



def draw_chart_flat(fig, section, sizes, png):
    """ draw_chart_flat( section, sizes, png)
    section : section name
    sizes   : dictionary (file,size)
    png     : image name
    """
    labels = sizes.keys()
    fracs = []
    for k in labels:
      fracs.append( sizes.get(k) )
    labels = map(pretty2, labels)
    draw_chart(fig, section, labels, fracs, png)




def draw_chart_toplevel(fig, section, tmap, png):
    """ draw_chart_toplevel(section, tmap, png)
    section : section name
    tmap    : tmap is [[name, parent, size], ... ]
    png     : image name
    """
    #[[row[i] for row in tmap] for i in range(3)]
    print "============== %s" % section
    dict = {}
    for l in tmap:
      name   = l[0]
      parent = l[1] 
      size   = l[2]
      dict[parent] = dict.get(parent,0) + size
    print dict
    draw_chart_flat(fig, section, dict, png)




def draw_chart_library(fig, section, tmap, png):
    """
    """
    # get all lib names
    i = 0
    dict = {}
    for l in tmap:
      name   = l[0]
      parent = l[1] 
      size   = l[2]
      dict[parent] = dict.get(parent,0) + size

    # for each lib
    for lib in dict.keys():
      libdict = {}
      for m in tmap:
        file   = m[0]
        parent = m[1]
        size   = m[2]
        if lib == parent: # parent
          if file != None:
            libdict[file] = libdict.get(file,0) + size
          else:
            libdict[parent] = libdict.get(parent,0) + size
        
      print "============== %s,%s" % (section,lib)
      draw_chart_flat (fig + i,".bss.%s" % lib,  libdict,    "mem.bss.%s.png" % lib)
      i += 1
    
## ##################################################
## ##################################################
    
def build_flat(map, section):
    sizes = {}
    for l in map:
      if l.startswith(" " + section + " "):
        ls = l.split()
        size = int(ls[2],16)
        if size != 0:
          sizes[ls[3]] = sizes.get(ls[3],0) + size
    print "%s %d lines" % (section,len(sizes))
    return sizes

## ##################################################
## ##################################################

def build_treemap(flat):
    tmap = []

    fullnames = flat.keys()

    for k in fullnames:
      g = pretty_re.search(k).groups()
      name   = g[1]
      parent = g[0]
      size   = flat.get(k)
      tmap.append ( [ name, parent, size ] )
    
    return tmap

## ##################################################
## ##################################################

def build(map, section):
    flat  = build_flat(map, section)
    tmap  = build_treemap(flat)
    return (flat, tmap)

## ##################################################
## ##################################################

parser = OptionParser()

parser.add_option("-e", "--elf",   help="select ELF file",    action="store", type="string", dest="file_elf", metavar="FILE")
parser.add_option("-m", "--map",   help="select MAP file",    action="store", type="string", dest="file_map", metavar="FILE")
parser.add_option("-o", "--out",   help="select output file", action="store", type="string", dest="file_out", metavar="FILE")
parser.add_option("-t", "--title", help="html title",         action="store", type="string", dest="title")

parser.add_option("-j", "--jscript",   action="store_true", dest="javascript")
parser.add_option("-c", "--camembert", action="store_true", dest="camembert" )
parser.add_option("-v", "--verbose",   action="store_true", dest="verbose"   )

def main():
    ##
    ## usage:
    ##
    ##     ./memmap.py -j [-m out.map] [-o out.html] > [file.html]
    ## 
    (options, args) = parser.parse_args()
    title = ""
    
    ##
    ##
    if options.verbose:
      print "example: memmap.py -j -t title -m out.map  -o memmap.html"
      print "example: memmap.py -j -t title -e prog.elf -o memmap.html"
      print "firefox file://`pwd`/memmap.html"

    ##
    ##
    if options.file_map:
      title = "Memory map for %s" % options.file_map
      map  = file(options.file_map).read().split("\n")

    ##
    ##
    if options.file_elf:
      title = "Memory map for %s" % options.file_elf
      cmd = ["arm-none-eabi-ld", "-M", options.file_elf]
      command = Popen(cmd, stdout=PIPE, stderr=STDOUT)
      stdout, stderr = command.communicate()
      map = stdout.split("\n")

    ##
    ##
    outfile = "memmap"
    if options.file_out:
      outfile = options.file_out

    ##
    ##
    if options.title:
      title = options.title

    ##
    ## Load structures and build treemap
    ##  
    (bss,    tm_bss)    = build(map,".bss")
    (data,   tm_data)   = build(map,".data")
    (text,   tm_text)   = build(map,".text")
    (rodata, tm_rodata) = build(map,".rodata")

    ##
    ## js-treemap
    ##

    tm1 = """ new TreeParentNode( "root", [
                 new TreeParentNode("Flash", [
                    new TreeParentNode(".text", [
    """
    tm2 = """       ] ),
                    new TreeParentNode(".rodata", [
    """
    tm3 = """       ] )
                 ] ),
                new TreeParentNode("RAM", [
                    new TreeParentNode(".data", [
    """
    tm4 = """       ] ),
                    new TreeParentNode(".bss", [
    """
    tm5 = """       ] )
                 ] )
              ] );
    """

    tm =      tm1.split('\n')
    ## tm = tm + tm_text.getnodes()
    for i in tm_text:
      tm.append("new TreeNode (\" %s \", %d)%s" % (i[0], i[2], ","))
    tm = tm + tm2.split('\n')
    ## tm = tm + rodata.getnodes()
    for i in tm_rodata:
      tm.append("new TreeNode (\" %s \", %d)%s" % (i[0], i[2], ","))
    tm = tm + tm3.split('\n')
    ## tm = tm + tm_data.getnodes()
    for i in tm_data:
      tm.append("new TreeNode (\" %s \", %d)%s" % (i[0], i[2], ","))
    tm = tm + tm4.split('\n')
    ## tm = tm + tm_bss.getnodes()
    for i in tm_bss:
      tm.append("new TreeNode (\" %s \", %d)%s" % (i[0], i[2], ","))
    tm = tm + tm5.split('\n')

    ##
    ## JavaScript / JsTreeMap
    ##
    if options.javascript:
      fd = open(outfile, 'w')
      for l in jstreemap.html(tm,title):
        fd.write("%s\n" % l)
      fd.close()

      
    ##
    ## Camembert section
    ##
    # draw_chart_flat(section,  flat,  "mem" + section + ".png")
    # print_html(tmbss, tmdata, tmtext)
    if options.camembert:
      draw_chart_toplevel(1,".text",   tm_text,   outfile + ".tl.text.png")
      draw_chart_toplevel(2,".bss",    tm_bss,    outfile + ".tl.bss.png")
      draw_chart_toplevel(3,".data",   tm_data,   outfile + ".tl.data.png")
      draw_chart_toplevel(4,".rodata", tm_rodata, outfile + ".tl.rodata.png")

    return 0
  

if __name__ == "__main__":
    main()
    

## ##################################################
## ##################################################

#  tm = """
#  new TreeParentNode( "root", [
# 	new TreeNode( "a", 1 ),
# 	new TreeNode( "BBBBBBBB", 2 ),
# 	new TreeNode( "CCCCCC", 3 ),
# 	new TreeNode( "d", 4 ),
# 	new TreeNode( "e", 5 ),
# 	new TreeNode( "f", 6 ),
# 	new TreeNode( "g", 7 ),
# 	new TreeNode( "h", 8 ),
# 	new TreeNode( "i", 9 ),
# 	new TreeNode( "j", 10 ),
# 	new TreeNode( "k", 11 ),
# 	new TreeNode( "l", 13 ),
# 	new TreeNode( "m", 14 ),
# 	new TreeParentNode( "n", [
#                 new TreeNode( "Alpha", 7 ),
#                 new TreeParentNode( "Beta", [
# 		   new TreeNode( "apples", 1 ),
# 		   new TreeNode( "pears", 2 ),
# 		   new TreeNode( "bananas", 3 )  
# 	        ] )
#             ] )
#         ,
# 	new TreeNode( "o", 16 ),
# 	new TreeNode( "p", 17 ), 
# 	new TreeNode( "q", 18 )
# ] );
# """
