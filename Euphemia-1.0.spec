#
# Euphemia RPM spec 
#
Summary: Design data structures and manage data.
Name: Euphemia
Icon: Euphemia.png
Version: 1.0
Release: 1
License: GPL
Group: Applications/Graphics
Source: https://www.codebydesign.com/releases/Euphemia-1.0.tar.gz
URL: https://www.codebydesign.com
Vendor: CodeByDesign
Packager: Peter Harvey <pharvey@codebydesign.com>

%description
Design data structures and manage data.

%prep
%setup

%build
qmake
make 

%install
mkdir -p %{buildroot}%{_bindir}
install -p -m 755 bin/euphemia %{buildroot}%{_bindir}/

%files
%doc LICENSE
%doc CREDITS
/usr/bin/euphemia

%changelog

%clean
make distclean

find . -type d -name '.tmp' -exec rm -rf {} +
find . -type d -name 'lib' -exec rm -rf {} +
find . -type d -name 'bin' -exec rm -rf {} +



