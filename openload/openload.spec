# This spec file was generated using Kpp
# If you find any problems with this spec file please report
# the error to ian geiser <geiseri@msoe.edu>
Summary:   Web application load test tool
Name:      openload
Version:   0.1.2
Release:   1
Copyright: GPL
Vendor:    pjohnsen <pelle.johnsen@mail.dk>
Url:       openload.sourceforge.net

Packager:  pjohnsen <pelle.johnsen@mail.dk>
Group:     Development/Tools
Source:    openload-0.1.2.tar.gz
BuildRoot: rpmbuild

%description
Openload is a tool for load testing web applications. The goal is a tool that is easy to use and provides near real-time performance measurements of the application under test which is very usefull during optimization.

%prep
%setup
CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" ./configure \
                 \
                $LOCALFLAGS
%build
# Setup for parallel builds
numprocs=`egrep -c ^cpu[0-9]+ /proc/stat || :`
if [ "$numprocs" = "0" ]; then
  numprocs=1
fi

make -j$numprocs

%install
make install-strip DESTDIR=$RPM_BUILD_ROOT

cd $RPM_BUILD_ROOT
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' > $RPM_BUILD_DIR/file.list.openload
find . -type f | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.openload
find . -type l | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.openload

%clean
rm -rf $RPM_BUILD_ROOT/*
rm -rf $RPM_BUILD_DIR/openload
rm -rf ../file.list.openload


%files -f ../file.list.openload
