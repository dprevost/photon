#!/usr/bin/env python

import pso

print dir(pso)
print dir(pso.error)

try:
    pso.init("10701", False )
    
except pso.error:
    print "Is Quasar running? Does the shared-memory file exists?"
    raise
    
#print 'rc = ', rc, ', ', pso.FOLDER

s = pso.Session()
print s
print dir(s)

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

#status = pso.ObjStatus()
#print status.obj_type

status = s.get_status( 'test1' )
print 'status = ', status
print status.obj_type, ' ', status.free_bytes
print 'status = ', dir(status)

pso.exit()
