xset s off
make distclean
rm -rf bin lib
rm -rf SDK/lib
find . -type f -name 'Makefile' -exec rm {} +
find . -type f -name 'qmake.stash' -exec rm {} +
find . -type d -name '.tmp' -exec rm -rf {} +
find . -type f -name 'qrc_*.cpp' -exec rm -rf {} +
qmake
make


