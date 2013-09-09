#! /bin/sed -f

s/^ \* \([[:alnum:]]*\)\. \([hc]\)[[:space:]]*/ \* \1\.\2/g

s#<pre>#\\code #g
s#</pre>#\\endcode #g

