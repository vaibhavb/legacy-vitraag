#!/usr/local/bin/python

import cgi
import os

form = cgi.FormContent()

print 'Content-type: text/html\n\n'

print 'Thanks you submitted:<br>'

for var in form.keys():
	print '  <strong>%s</strong>: %s<br>' % (var, form[var])
 
os.system('Mail -s subject to < opinion') or die

print '<a href="../opinions.html">Back</a>'

