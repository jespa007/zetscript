echo LOCAL=${LOCALSOURCEDIR}


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

cd ${LOCALSOURCEDIR} && \
wget -c http://kcat.strangesoft.net/openal-releases/openal-soft-1.15.1.tar.bz2 && \
cd ${LOCALBUILDDIR} && \
tar xjf ${LOCALSOURCEDIR}/openal-soft-1.15.1.tar.bz2 && \
cd openal-soft-1.15.1 && \
cd build && \
cmake -DCMAKE_INSTALL_PREFIX:PATH=${LOCALDESTDIR} -G "MSYS Makefiles" .. && \
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
wget -c http://downloads.sourceforge.net/project/pdcurses/pdcurses/3.4/PDCurses-3.4.tar.gz  && \
cd ${LOCALBUILDDIR} && \
tar xzf ${LOCALSOURCEDIR}/PDCurses-3.4.tar.gz  && \
cd PDCurses-3.4  && \
cd win32  && \
cat mingwin32.mak | sed 's|\\exp\-|\/exp\-|g' > mingwin32.new  && \
mv mingwin32.new mingwin32.mak  && \
cat mingwin32.mak | sed 's|type|cat|g' > mingwin32.new  && \
mv mingwin32.new mingwin32.mak  && \
cat mingwin32.mak | sed 's|copy|cp|g' > mingwin32.new  && \
mv mingwin32.new mingwin32.mak  && \
cat mingwin32.mak | sed 's|del|rm|g' > mingwin32.new  && \
mv mingwin32.new mingwin32.mak  && \
make -f mingwin32.mak libs  && \
install pdcurses.a ${LOCALDESTDIR}/lib/libpdcurses.a  && \
make -f mingwin32.mak clean  && \
make -f mingwin32.mak DLL=Y libs  && \
install pdcurses.dll ${LOCALDESTDIR}/bin  && \
cd ..  && \
install curses.h ${LOCALDESTDIR}/include


cd ${LOCALSOURCEDIR} && \
wget -c http://www.tortall.net/projects/yasm/releases/yasm-1.1.0.tar.gz && \
cd ${LOCALBUILDDIR} && \
tar xzf ${LOCALSOURCEDIR}/yasm-1.1.0.tar.gz && \
cd yasm-1.1.0 && \
./configure --prefix=${LOCALDESTDIR} && \
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
wget -c http://zlib.net/zlib-1.2.8.tar.gz && \
cd ${LOCALBUILDDIR} && \
tar xf ${LOCALSOURCEDIR}/zlib-1.2.8.tar.gz && \
cd zlib-1.2.8 && \
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
wget -c jespada.com/jeme/giflib-5.1.0.tar.bz2 && \
cd ${LOCALBUILDDIR} && \
tar xf ${LOCALSOURCEDIR}/giflib-5.1.0.tar.bz2 && \
cd giflib-5.1.0 && \
./configure --enable-shared --prefix=${LOCALDESTDIR} && \
make && \
make install

cd ${LOCALSOURCEDIR} && \
wget -c https://github.com/ChaiScript/ChaiScript/archive/v5.4.0.tar.gz -O ChaiScript-5.4.0.tar.gz --no-check-certificate && \
cd ${LOCALBUILDDIR} && \
tar xf ${LOCALSOURCEDIR}/ChaiScript-5.4.0.tar.gz && \
cd ChaiScript-5.4.0 && \
cmake -DCMAKE_INSTALL_PREFIX:PATH=${LOCALDESTDIR} -G "MSYS Makefiles" && \
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