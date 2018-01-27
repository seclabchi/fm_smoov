PACKAGE_TARBALL_NAME=fltk-1.3.4-2-source.tar.gz
PACKAGE_PATH=http://fltk.org/pub/fltk/1.3.4/$PACKAGE_TARBALL_NAME

mkdir fltk
cd fltk
wget $PACKAGE_PATH .
tar -xzvf $PACKAGE_TARBALL_NAME
cd fltk-1.3.4-2
./configure --enable-threads --enable-shared
make
sudo make install
cd ../..
rm -rf fltk
echo "Done!"
