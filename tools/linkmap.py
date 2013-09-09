#! /usr/bin/env python

##
## Program taken from the Senstools repository.
##
## Generates a tree map of .text region for included files
##
## usage:
##      linkmap.py file.map > file.html
##
##

import os
import sys

is_cgi = "HTTP_USER_AGENT" in os.environ

if is_cgi:
  import cgi
  import cgitb
  cgitb.enable()

## ##################################################
## ##################################################

data = ""
debug = ""

load_list = []
depend_list = []
size_dict = {}

if is_cgi:
  cgifs = cgi.FieldStorage()

  if cgifs.getvalue("action") == "source":
    print "Content-Type: text/plain\n\n%s" % file(sys.argv[0]).read()
    sys.exit()
  else:
    print "Content-Type: text/html\n"

  lm = cgifs.getvalue("linkmap")

elif len(sys.argv) == 2:
  lm = file(sys.argv[1]).read()
else:
  print "\tUsage: %s <map_file>" % sys.argv[0]
  sys.exit(1)

## ##################################################
## ##################################################

import re
pretty_re = re.compile("([^/(]+)(?:\((.+)\))?$")

def pretty(s):
  g = pretty_re.search(s).groups()
  return g[1] or g[0]

## ##################################################
## ##################################################

class Codeobj:
  def __init__(self, name, size, parent):
    self.name = name
    self.size = size
    self.total_size = size
    self.parent = parent
    self.child_list = []

    if self.parent:
      self.parent.push_size(size)

  def print_label(self, total=1):
    if total:
      return '"%s (%d)"' % (pretty(self.name), self.total_size)
    return '" %s (%d)"' % (pretty(self.name), self.size)

  def print_one(self):
    parent_name = self.parent and self.parent.print_label() or 'null'
    ret = '[%s,%s,%d],\n' % (self.print_label(), parent_name, self.size)
    if self.size and self.child_list:
      ret += '[%s,%s,%d],\n' % (self.print_label(0), self.print_label(), self.size)
    return ret

  def print_tree(self):
    ret = self.print_one()
    for child in self.child_list:
      ret += child.print_tree()
    return ret

  def add_child(self, name):
    child_size = size_dict.get(name, 0)
    child = Codeobj(name, child_size, self)
    self.child_list.append(child)

    for x in (x for x in depend_list if x[1] == name):
      child.add_child(x[0])

  def push_size(self, size):
    self.total_size += size
    if self.parent:
      self.parent.push_size(size)

## ##################################################
## ##################################################

def get_section_data(section_name = ".text"):
  n = 0
  while n < lines and not lm[n].startswith(section_name):
    n += 1
  n += 1
  while n < lines and lm[n] != "":
    tmpsiz = lm[n].split()
    if lm[n].startswith(" ."):
      if len(tmpsiz) < 4:
	tmpsiz = (lm[n] + lm[n + 1]).split()
	n += 1
      if len(tmpsiz) == 4:
        size_dict[tmpsiz[3]] = size_dict.get(tmpsiz[3], 0) + int(tmpsiz[2], 16)
    n += 1

  # make data
  root_obj = Codeobj("binary", 0, None)
  for item in load_list:
    root_obj.add_child(item)

  return root_obj.print_tree()

## ##################################################
## ##################################################

if lm:

  lm = lm.split("\n")
  lines = len(lm)
  n = 0;

  # dependancy tree
  while n < lines and lm[n] != "Archive member included because of file (symbol)":
    n += 1
  n += 1
  if lm[n] != "": raise BaseException('"%s" is not empty' % lm[n])
  n += 1

  while n < lines and lm[n] != "":
    tmpdep = lm[n].split()
    if len(tmpdep) < 3:
      tmpdep = (lm[n] + lm[n + 1]).split()
      n += 1

    # if len(tmpdep) != 3: raise BaseException("%s does not contains 3 strings" % tmpdep)
    if len(tmpdep) == 3:
      depend_list.append(tmpdep)
    n += 1

  # LOAD files
  for line in lm:
    if line.startswith("LOAD"):
      load_list.append(line.split()[1])

  # .text size
  data = get_section_data(".text")

## ##################################################
## ##################################################

print """<html>
<head>
<script type="text/javascript" src="https://www.google.com/jsapi"></script>
<script type="text/javascript">
google.load("visualization", "1", {packages:["treemap"]});
google.setOnLoadCallback(drawChart);

// Create and populate the data table.
function drawChart() {
var data = new google.visualization.DataTable();
data.addColumn('string', 'Object');
data.addColumn('string', 'Parent');
data.addColumn('number', 'Size');
data.addRows([
%s]);

// Create and draw the visualization.
var tree = new google.visualization.TreeMap(document.getElementById('visualization'));
tree.draw(data, {
minColor: 'green',
midColor: 'yellow',
maxColor: 'red',
headerHeight: 15,
fontColor: 'black',
showScale: true});
}
</script>
</head>

<body>""" % data

if is_cgi:
  print """<div>
<form method="post" enctype="multipart/form-data">
<input type="file" name="linkmap" />
<input type="submit" />
</form>
<a href="?action=source">View Source</a>
</div>"""

print """<div><pre>%s</pre></div>
<div id="visualization" style="height: 80%%; width: 90%%;"></div>
</body>
</html>""" % debug

## ##################################################
## ##################################################
