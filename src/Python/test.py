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

status = pso.ObjStatus()
#print status.obj_type
print dir(status)

rc = s.get_status( 'test1', status )
print 'rc = ', rc
print status.obj_type, ' ', status.free_bytes

pso.exit()
