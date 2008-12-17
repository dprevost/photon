#!/usr/bin/env python

import pso

try:
    pso.init("10701", False )
    
except pso.error:
    print "Is Quasar running? Does the shared-memory file exists?"
    raise
    
s = pso.Session()

s.create_object( 'folder1', pso.BaseDef(pso.FOLDER, 0) )
s.create_object( 'folder1/folder2', pso.BaseDef(pso.FOLDER, 0) )
s.create_object( 'folder1/folder3', pso.BaseDef(pso.FOLDER, 0) )

f1 = pso.Folder(s, 'folder1')
print 'f1.name =', f1.name
f1.create_object( 'folder4', pso.BaseDef(pso.FOLDER, 0) )

try:
    f1.create_object( 'queue1', pso.BaseDef(pso.QUEUE, 0) )
except:
    pass
else:
    raise pso.error('failed')

f1.create_object( 'queue1', pso.BaseDef(pso.QUEUE, 1),
    [ pso.FieldDefinition('field1', pso.STRING, 20, 0, 0, 0, 0) ] )

# Loop on all the entries in folder1
for e in f1:
    print 'Folder entry:', e

# Create a new folder, not attached to a specific Photon folder
f2 = pso.Folder(s)

# Attemp to access the name -> exception
try:
    print f2.name
except:
    pass
else:
    raise pso.error('failed')

# Associate it with a real folder
f2.__init__(s,'/folder1')
print 'f2.name =', f2.name


# Attemp to access the name after a close -> exception
f2.close()
try:
    print f2.name
except:
    pass
else:
    raise pso.error('failed')

status = s.get_status( 'folder1' )
print 'status = ', status
print status.obj_type, ' ', status.free_bytes
status = f1.status()
print 'status = ', status
print status.obj_type, ' ', status.free_bytes

pso.exit()

