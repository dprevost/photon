#!/usr/bin/env python

import pso

print dir(pso)

rc = pso.init("10701", False )
print 'rc = ', rc, ', ', pso.FOLDER

s = pso.Session()
print s
print dir(s)

status = pso.ObjStatus()
#print status.obj_type
print dir(status)

rc = s.get_status( 'test1', status )
print 'rc = ', rc
print status.obj_type, ' ', status.free_bytes

pso.exit()
