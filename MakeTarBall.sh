#!/bin/bash

cd Euphemia-1.0

echo "Removing build artifacts..."
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

echo "Removing git and vs files..."
find . -type f -name '*.gitignore' -exec rm {} +
find . -type f -name '*.vpj' -exec rm {} +
find . -type f -name '*.vpw' -exec rm {} +
find . -type f -name '*.vpwhistu' -exec rm {} +
find . -type f -name '*.vtg' -exec rm {} +
find . -type d -name '.git' -exec rm -rf {} +

echo "Removing remaining extra files..."
find . -type f -name '*.spec' -exec rm {} +

cd ..

echo "Creating source tar-ball..."
tar -czf Euphemia-1.0.tar.gz Euphemia-1.0
