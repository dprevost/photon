#!/usr/bin/env python
#
# Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

import pso

# Warning: the tests in here are not complete. They are meant to
# proove that the Python interface works as intended; not to
# provide an alternate test suite.

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

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

def destroy_test():
    global s
    
    try:
        s.destroy_object( 'sess_folder1/folder3/queue1' )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['OBJECT_IS_IN_USE']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

    s.commit()
    
    try:
        s.destroy_object( '' )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['INVALID_LENGTH']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

    try:
        s.destroy_object( 'sess_not_exists' )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['NO_SUCH_OBJECT']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

    try:
        s.destroy_object( 'sess_not_exists/object1' )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['NO_SUCH_FOLDER']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

    try:
        s.destroy_object( 'sess_folder1' )
    except pso.error, (msg, errcode):
        if errcode != pso.errs['FOLDER_IS_NOT_EMPTY']:
            print 'errcode = ', pso.err_names[errcode]
            print 'message = ', msg
            raise
    else:
        raise pso.error, 'failed'

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

def get_def_test():
    global s

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

try:
    pso.init("10701", False )
    
except pso.error:
    print "Is Quasar running? Does the shared-memory file exists?"
    raise
    
try:
    s = pso.Session()

    create_test()

    s.create_object( 'sess_folder1', pso.BaseDef(pso.FOLDER, 0) )
    s.create_object( 'sess_folder1/folder2', pso.BaseDef(pso.FOLDER, 0) )
    s.create_object( 'sess_folder1/folder3', pso.BaseDef(pso.FOLDER, 0) )
    s.create_object( 'sess_folder1/folder3/queue1', pso.BaseDef(pso.QUEUE, 3),
        [ pso.FieldDefinition('field1', pso.STRING,    20, 0,   0, 0, 0),
          pso.FieldDefinition('field2', pso.INTEGER,    4, 0,   0, 0, 0),
          pso.FieldDefinition('field3', pso.VAR_BINARY, 0, 0, 100, 0, 0)] )

    destroy_test()

    s.destroy_object( 'sess_folder1/folder2' )
    s.commit()
    
    s.destroy_object( 'sess_folder1/folder3/queue1' )    
    s.rollback()
    
    get_def_test()
    
    base, fields = s.get_definition( 'sess_folder1/folder3/queue1' )
    print base
    print fields[1]
    i = s.get_info()
    print i
    status = s.get_status( 'sess_folder1/folder3/queue1' )
    print 'status = ', status

except pso.error, (msg, errcode):
    print 'pso message = ', msg
    raise
except:
    print 'The test failed on a non-pso exception'
    raise 

pso.exit()

#print 'status = ', dir(status)

pso.exit()
