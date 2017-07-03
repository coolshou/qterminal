#!/bin/bash

#To create a chroot suitable for cross-building:
#    mk-sbuild --target=armhf xenial
#    mk-sbuild --target=i386 xenial

#If this is the first time you have used sbuild on this machine
#    sbuild-update --keygen

#To attempt to cross-build a package (you may want to run this in a scratch directory, and use the -n option if you don't want to save the build log):
#    sbuild --build=amd64 --host=i386 -d xenial

#TODO: create source package
#1. get version
#2. tar acvf ../qtvt_2017.7.3.0.orig.tar.bz2 .

echo "build i386 debian package"
sbuild --host=i386

#debuild -ai386 -b -us -uc
#export DEB_BUILD_GNU_TYPE=i386-linux-gnu
#dpkg-architecture -A i386
#dpkg-buildpackage --target-arch i386 -b -us -uc
#dpkg-buildpackage DEB_HOST_GNU_TYPE=i386 -b -us -uc
#echo "build amd64 debian package"
#debuild -aamd64 -b -us -uc
#debuild  -b -us -uc
