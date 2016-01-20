export LOCALSOURCEDIR=$HOME/Downloads
export LOCALBUILDDIR=$HOME/Downloads
export LOCALDESTDIR=/usr/local

sudo apt-get install automake libreadline-dev libglu1-mesa-dev cmake yasm mesa-common-dev libxext-dev sudo autotools-dev pkg-config libz-dev uuid-dev  libusb-1.0-0-dev -y



cd ${LOCALSOURCEDIR} && \
wget -c http://www.libsdl.org/release/SDL2-2.0.3.tar.gz && \
cd ${LOCALBUILDDIR} && \
tar xzf ${LOCALSOURCEDIR}/SDL2-2.0.3.tar.gz && \
cd SDL2-2.0.3 && \
./configure --prefix=${LOCALDESTDIR} && \
make && \
make install

cd ${LOCALSOURCEDIR} && \
wget -c http://www.libsdl.org/projects/SDL_net/release/SDL2_net-2.0.0.tar.gz && \
cd ${LOCALBUILDDIR} && \
tar xzf ${LOCALSOURCEDIR}/SDL2_net-2.0.0.tar.gz && \
cd SDL2_net-2.0.0 && \
./configure --prefix=${LOCALDESTDIR} && \
make && \
make install

cd ${LOCALSOURCEDIR} && \
wget -c "http://downloads.sourceforge.net/libpng/libpng-1.6.7.tar.gz" && \
cd ${LOCALBUILDDIR} && \
tar xzf ${LOCALSOURCEDIR}/libpng-1.6.7.tar.gz && \
cd libpng-1.6.7 && \
./configure --prefix=${LOCALDESTDIR} && \
make && \
make install

cd ${LOCALSOURCEDIR} && \
wget -c http://www.ijg.org/files/jpegsrc.v9.tar.gz && \
cd ${LOCALBUILDDIR} && \
tar xzf ${LOCALSOURCEDIR}/jpegsrc.v9.tar.gz && \
cd jpeg-9 && \
./configure --prefix=${LOCALDESTDIR} --enable-shared --enable-static && \
make && \
make install

cd ${LOCALSOURCEDIR} && \
wget -c ftp://ftp.remotesensing.org/pub/libtiff/tiff-4.0.3.tar.gz && \
cd ${LOCALBUILDDIR} && \
tar xzf ${LOCALSOURCEDIR}/tiff-4.0.3.tar.gz && \
cd tiff-4.0.3 && \
./configure --prefix=${LOCALDESTDIR} && \
make && \
make install

cd ${LOCALSOURCEDIR} && \
wget --no-check-certificate -c https://webp.googlecode.com/files/libwebp-0.3.1.tar.gz && \
cd ${LOCALBUILDDIR} && \
tar xzf ${LOCALSOURCEDIR}/libwebp-0.3.1.tar.gz && \
cd libwebp-0.3.1 && \
./configure --prefix=${LOCALDESTDIR} && \
make && \
make install

cd ${LOCALSOURCEDIR} && \
wget -c http://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.0.tar.gz && \
cd ${LOCALBUILDDIR} && \
tar xzf ${LOCALSOURCEDIR}/SDL2_image-2.0.0.tar.gz && \
cd SDL2_image-2.0.0 && \
./configure --prefix=${LOCALDESTDIR} && \
make && \
make install

cd ${LOCALBUILDDIR} && \
svn export svn://svn.icculus.org/smpeg/tags/release_2_0_0 smpeg-2.0.0 && \
cd smpeg-2.0.0 && \
./autogen.sh && \
./configure --prefix=${LOCALDESTDIR} --enable-mmx  && \
make && \
make install


cd ${LOCALSOURCEDIR} && \
wget -c http://curl.haxx.se/download/curl-7.33.0.tar.gz && \
cd ${LOCALBUILDDIR} && \
tar xzf ${LOCALSOURCEDIR}/curl-7.33.0.tar.gz && \
cd curl-7.33.0 && \
CFLAGS="-mms-bitfields -mthreads" ./configure  --prefix=${LOCALDESTDIR} --without-ssl  && \
make && \
make install

cd ${LOCALSOURCEDIR} && \
wget -c http://ffmpeg.org/releases/ffmpeg-2.3.tar.bz2 && \
cd ${LOCALBUILDDIR} && \
tar xf ${LOCALSOURCEDIR}/ffmpeg-2.3.tar.bz2 && \
cd ffmpeg-2.3 && \
./configure --enable-shared --prefix=${LOCALDESTDIR} && \
make && \
make install


cd ${LOCALSOURCEDIR} && \
wget -c jespada.com/jeme/sound/libxmp-4.2.0.tar.gz && \
cd ${LOCALBUILDDIR} && \
tar xf ${LOCALSOURCEDIR}/libxmp-4.2.0.tar.gz && \
cd libxmp-4.2.0 && \
./configure --enable-shared --prefix=${LOCALDESTDIR} && \
make && \
make install

cd ${LOCALSOURCEDIR} && \
wget -c jespada.com/jeme/freetype-2.5.2.tar.gz && \
cd ${LOCALBUILDDIR} && \
tar xf ${LOCALSOURCEDIR}/freetype-2.5.2.tar.gz && \
cd freetype-2.5.2 && \
./configure --enable-shared --prefix=${LOCALDESTDIR} && \
make && \
make install

cd ${LOCALSOURCEDIR} && \
wget -c jespada.com/jeme/glttf-0.0.3.tar.gz && \
cd ${LOCALBUILDDIR} && \
tar xf ${LOCALSOURCEDIR}/glttf-0.0.3.tar.gz && \
cd glttf-0.0.3 && \
./configure --enable-shared --prefix=${LOCALDESTDIR} --disable-freetypetest && \
make && \
make install

cd ${LOCALSOURCEDIR} && \
wget -c jespada.com/jeme/re2-20140304.tgz && \
cd ${LOCALBUILDDIR} && \
tar xf ${LOCALSOURCEDIR}/re2-20140304.tgz && \
cd re2 && \
#./configure --enable-shared --prefix=${LOCALDESTDIR} && \
make -f Makefile && \
make install

cd ${LOCALSOURCEDIR} && \
wget -c jespada.com/jeme/giflib-4.2.3.tar.gz && \
cd ${LOCALBUILDDIR} && \
tar xf ${LOCALSOURCEDIR}/giflib-4.2.3.tar.gz && \
cd giflib-4.2.3 && \
./configure --enable-shared --prefix=${LOCALDESTDIR} && \
make && \
make install

cd ${LOCALSOURCEDIR} && \
wget -c https://github.com/ChaiScript/ChaiScript/archive/v5.4.0.tar.gz -O ChaiScript-5.4.0.tar.gz --no-check-certificate && \
cd ${LOCALBUILDDIR} && \
tar xf ${LOCALSOURCEDIR}/ChaiScript-5.4.0.tar.gz && \
cd ChaiScript-5.4.0 && \
cmake -DCMAKE_INSTALL_PREFIX:PATH=${LOCALDESTDIR} && \
make && \
make install


cd ${LOCALSOURCEDIR} && \
wget -c jespada.com/jeme/SDL/SDL2_gfx-1.0.0.tar.gz && \
cd ${LOCALBUILDDIR} && \
tar xf ${LOCALSOURCEDIR}/SDL2_gfx-1.0.0.tar.gz && \
cd SDL2_gfx-1.0.0 && \
./configure --enable-shared --prefix=${LOCALDESTDIR} && \
make && \
make install