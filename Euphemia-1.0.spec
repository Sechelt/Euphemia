#
# Euphemia RPM spec 
#
Summary: A pixel editor.
Name: Euphemia
Icon: Euphemia.xpm
Version: 1.0
Release: 1
License: GPL
Group: Applications/Graphics
Source: https://www.codebydesign.com/releases/Euphemia-1.0.tar.gz
URL: https://www.codebydesign.com
Vendor: CodeByDesign
Packager: Peter Harvey <pharvey@codebydesign.com>

%description
For editing of images with an emphasis on detailed 
pixel manipulation.

%prep
%setup

%build
cd Classic-SDK
qmake-qt5
make 
cd ../Euphemia-SDK
qmake-qt5
make
cd ../Euphemia
qmake-qt5
make
cd ..

%install
mkdir -p %{buildroot}%{_bindir}
install -p -m 755 Euphemia/bin/euphemia %{buildroot}%{_bindir}/

%files
%doc Euphemia/LICENSE
%doc Euphemia/CREDITS
/usr/bin/euphemia

%changelog

%clean
cd Classic-SDK
make distclean
cd ../Euphemia-SDK
make distclean
cd ../Euphemia
make distclean
cd ..

find . -type d -name '.tmp' -exec rm -rf {} +
find . -type d -name 'lib' -exec rm -rf {} +
find . -type d -name 'bin' -exec rm -rf {} +



