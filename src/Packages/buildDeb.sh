#!/bin/sh

# Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
# 
# This file is part of vdsf (Virtual Data Space Framework).
#
# This file may be distributed and/or modified under the terms of the
# GNU General Public License version 2 as published by the Free Software
# Foundation and appearing in the file COPYING included in the
# packaging of this library.
#
# This file is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

if [ "$TMPDIR" = "" ] ; then
   TMPDIR=/tmp
fi

mkdir -p $TMPDIR/debian/DEBIAN
cp control $TMPDIR/debian/DEBIAN

cd ../..
make DESTDIR=/tmp/debian install-strip 
#prefix=$TMPDIR/debian/usr/local
mkdir -p $TMPDIR/debian/usr/local/share/doc/vdsf
install -m 644 README $TMPDIR/debian/usr/local/share/doc/vdsf/README
install -m 644 COPYING $TMPDIR/debian/usr/local/share/doc/vdsf/copyright
install -m 644 ChangeLog $TMPDIR/debian/usr/local/share/doc/vdsf/changelog
install -m 644 ChangeLog $TMPDIR/debian/usr/local/share/doc/vdsf/changelog.Debian
gzip $TMPDIR/debian/usr/local/share/doc/vdsf/changelog
gzip $TMPDIR/debian/usr/local/share/doc/vdsf/changelog.Debian
install -m 644 doc/refman.pdf $TMPDIR/debian/usr/local/share/doc/vdsf/refman.pdf

cd $TMPDIR
fakeroot dpkg-deb --build debian