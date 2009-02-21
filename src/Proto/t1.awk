BEGIN {
   if ( ARGC < 2 ) {
      print "Error: no input filename!" > "/dev/stderr"
      exit(1)
   }
   prefix = "Photon/"
}

/^struct/ {
   
   split($2, a, "_")
   print "#include <photon/" prefix tolower(a[2]) a[4] ".h>"
   
   print "typedef struct " tolower(a[2]) a[4] " " $2

   
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