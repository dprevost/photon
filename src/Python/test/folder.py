#!/usr/bin/env python

import pso

try:
    pso.init("10701", False )
    
except pso.error:
    print "Is Quasar running? Does the shared-memory file exists?"
    raise
    
s = pso.Session()

f = pso.Folder(s, '/')
print dir(f)

for e in f:
    print 'e.name,', e

f2 = pso.Folder(s)
try:
    print f2.name
except:
    pass

f2.__init__(s,'/test1')
print f2.name

print 'after loop'
print dir(f)

status = s.get_status( 'test1' )
print 'status = ', status
print status.obj_type, ' ', status.free_bytes

pso.exit()
