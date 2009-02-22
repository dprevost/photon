BEGIN {
   run_end = 1
   read_proto = 0
   if ( ARGC < 4 ) {
      print "Error: missing arguments!" > "/dev/stderr"
      print "Usage: header_name --proto=proto_name --guard=guard" > "/dev/stderr"
      run_end = 0
      exit
   }
   split(ARGV[2], a, "=")
   if ( a[1] != "--proto" ) {
      print "Usage: header_name --proto=proto_name --guard=guard" > "/dev/stderr"
      run_end = 0
      exit
   }
   proto = a[2]
   split(ARGV[3], a, "=")
   if ( a[1] != "--guard" ) {
      print "Usage: header_name --proto=proto_name --guard=guard" > "/dev/stderr"
      run_end = 0
      exit
   }
   guard = toupper(a[2])

   delete ARGV[3]
   delete ARGV[2]
   
   print "/*"
   print " * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>"
   print " *"
   print " * This file is part of Photon (photonsoftware.org)."
   print " *"
   print " * This file may be distributed and/or modified under the terms of the"
   print " * GNU General Public License version 2 or version 3 as published by the "
   print " * Free Software Foundation and appearing in the file COPYING.GPL2 and "
   print " * COPYING.GPL3 included in the packaging of this software."
   print " *"
   print " * Licensees holding a valid Photon Commercial license can use this file" 
   print " * in accordance with the terms of their license."
   print " *"
   print " * This software is distributed in the hope that it will be useful,"
   print " * but WITHOUT ANY WARRANTY; without even the implied warranty of"
   print " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." 
   print " */"
   print ""
   print "/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */"
   print ""
   print "#ifndef " guard
   print "#define " guard
   print ""
   print "#include <google/protobuf-c/protobuf-c.h>"
   print ""
   print "/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */"
   print ""
   print "#ifdef __cplusplus"
   print "extern \"C\" {"
   print "#endif"
   print ""
}

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/^struct/ {

   # we do this before calling readProto since readProto() uses getline 
   # and will change $2.
   split($2, a, "_")

   if ( read_proto == 0 ) {
      readProto(proto)
      read_proto = 1
   }

   for (x = 1; x <= commentCount[a[4]]; x++) {
      print comments[a[4] x]
   }
   
   print "struct " tolower(a[2]) a[4] 
   print "{"
   
   while ( $0 !~ /};/ ) {
      if (getline <= 0) {
         msg = ("unexpected EOF/error: " ERRNO)
         print msg > "/dev/stderr"
         exit
      }
      if ( NF == 2 ) {
         var = stripVar()
         for (x = 1; x <= commentCount[var]; x++) {
            print comments[var x]
         }
         # The protobuf gen. files use 2 spaces indent. We use three...
         print " " $0
         print ""
      }
   }
   print "};"
   
   next
}

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

function readProto( proto )
{
   while ( (getline < proto) > 0 ) {
      if ( $0  ~ /\/\*\*/ ) {
         count = 1
         tmp[1] = $0
         while ( $NF !~ /\*\// ) {
            getline < proto
            count++
            tmp[count] = $0
         }
         do { 
            getline < proto
         } while ( NF < 3 )
         if ( $1 == "message" ) {
            
            for (x = 1; x <= count; x++) {
               comments[$2 x] = tmp[x]
            }
            commentCount[$2] = count
         }
         else {
            for (x = 1; x <= count; x++) {
               comments[$3 x] = tmp[x]
            }
            commentCount[$3] = count
         }
      }
   }
}

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

function stripVar()
{
   myVar = $2
   sub(/^\*/, "", myVar)
   sub(/;/, "", myVar)
   return myVar
}

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

END {
   if ( run_end == 0 ) exit(1)

   print ""
   print "/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */"
   print ""
   print "#ifdef __cplusplus"
   print "}"
   print "#endif"
   print ""
   print "#endif /* " guard " */"
}

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
