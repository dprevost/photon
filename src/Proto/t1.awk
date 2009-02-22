BEGIN {
   if ( ARGC < 2 ) {
      print "Error: no input filename!" > "/dev/stderr"
      exit(1)
   }
}

/^typedef struct/ {
   next
}

/^struct/ {
   
   split($2, a, "_")
   print "#include <photon/" tolower(a[2]) a[4] ".h>"
   
   print "typedef struct " tolower(a[2]) a[4] " " a[2] "__" a[4] ";"

   
   while ( $0 !~ /};/ ) {
      if (getline <= 0) {
         msg = ("unexpected EOF/error: " ERRNO)
         print msg > "/dev/stderr"
         exit
      }
   }
   
   next
}

{ print }