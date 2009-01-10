#!/usr/bin/env python

import pso


#pso.__dict__['INVALID_LENGTH'] = 8
print dir(pso)
#print dir(pso.error)

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

def create_test():
    global s
    
    try:
        s.create_object( '', pso.BaseDef(pso.FOLDER, 0) )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['INVALID_LENGTH']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

    try:
        s.create_object( 'test1', pso.BaseDef(0, 0) )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['WRONG_OBJECT_TYPE']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

    try:
        s.create_object( 'test1', pso.BaseDef(pso.FOLDER, 1) )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['INVALID_NUM_FIELDS']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

    try:
        s.create_object( 'queue1', pso.BaseDef(pso.QUEUE, 1),
            [ pso.FieldDefinition('', pso.STRING, 20, 0, 0, 0, 0)] )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['INVALID_FIELD_NAME']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

    try:
        s.create_object( 'queue1', pso.BaseDef(pso.QUEUE, 1),
            [ pso.FieldDefinition('field1', 0, 20, 0, 0, 0, 0)] )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['INVALID_FIELD_TYPE']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

    try:
        s.create_object( 'queue1', pso.BaseDef(pso.QUEUE, 1),
            [ pso.FieldDefinition('field1', pso.STRING, 0, 0, 0, 0, 0)] )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['INVALID_FIELD_LENGTH']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

    try:
        s.create_object( 'queue1', pso.BaseDef(pso.QUEUE, 1),
            [ pso.FieldDefinition('field1', pso.INTEGER, 6, 0, 0, 0, 0)] )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['INVALID_FIELD_LENGTH_INT']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

    try:
        s.create_object( 'queue1', pso.BaseDef(pso.QUEUE, 1),
            [ pso.FieldDefinition('field1', pso.VAR_BINARY, 0, 5, 4, 0, 0)] )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['INVALID_FIELD_LENGTH']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

    try:
        s.create_object( 'queue1', pso.BaseDef(pso.QUEUE, 1),
            [ pso.FieldDefinition('field1', pso.VAR_BINARY, 0, 0, 0, 0, 0)] )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['INVALID_FIELD_LENGTH']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

    try:
        s.create_object( 'queue1', pso.BaseDef(pso.QUEUE, 1),
            [ pso.FieldDefinition('field1', pso.VAR_BINARY, 0, 0, 0, 0, 0)] )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['INVALID_FIELD_LENGTH']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

    try:
        s.create_object( 'queue1', pso.BaseDef(pso.QUEUE, 2),
            [ pso.FieldDefinition('field1', pso.VAR_BINARY, 0, 0, 0, 0, 0),
              pso.FieldDefinition('field2', pso.INTEGER,    4, 0, 0, 0, 0)] )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['INVALID_FIELD_TYPE']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

    try:
        s.create_object( 'map1', pso.BaseDef(pso.HASH_MAP, 1),
            [ pso.FieldDefinition('field1', pso.STRING, 20, 0, 0, 0, 0)] )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['INVALID_FIELD_TYPE']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

    try:
        s.create_object( 'map1', 
            pso.BaseDef(pso.HASH_MAP, 1, pso.KeyDefinition(pso.KEY_STRING, 0, 0, 0)),
            [ pso.FieldDefinition('field1', pso.STRING, 20, 0, 0, 0, 0)] )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['INVALID_FIELD_LENGTH']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'


#
    try:
        s.create_object( 'queue1', pso.BaseDef(pso.QUEUE, 3),
            [ pso.FieldDefinition('field1', pso.STRING,    20, 0,   0, 0, 0),
              pso.FieldDefinition('field2', pso.INTEGER,    4, 0,   0, 0, 0),
              pso.FieldDefinition('field3', pso.VAR_BINARY, 0, 0, 100, 0, 0)] )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['INVALID_NUM_FIELDS']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

try:
    pso.init("10701", False )
    
except pso.error:
    print "Is Quasar running? Does the shared-memory file exists?"
    raise
    
s = pso.Session()

#print s
#print dir(s)

create_test()

s.create_object( 'sess_folder1', pso.BaseDef(pso.FOLDER, 0) )
s.create_object( 'sess_folder1/folder2', pso.BaseDef(pso.FOLDER, 0) )
s.create_object( 'sess_folder1/folder3', pso.BaseDef(pso.FOLDER, 0) )
s.create_object( 'sess_folder1/folder3/queue1', pso.BaseDef(pso.QUEUE, 3),
    [ pso.FieldDefinition('field1', pso.STRING,    20, 0,   0, 0, 0),
      pso.FieldDefinition('field2', pso.INTEGER,    4, 0,   0, 0, 0),
      pso.FieldDefinition('field3', pso.VAR_BINARY, 0, 0, 100, 0, 0)] )



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
