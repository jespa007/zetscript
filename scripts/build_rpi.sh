#!/bin/bash

# To get the rootfs which is required here, use:
# rsync -rl --delete-after --safe-links pi@192.168.1.PI:/{lib,usr} $HOME/rpi/rootfs

export RASPBIAN_ROOTFS=$HOME/rpi/rootfs
export PATH=/opt/cross-pi-gcc/bin:/opt/cross-pi-gcc/libexec/gcc/arm-linux-gnueabihf/8.3.0:$PATH
export RASPBERRY_VERSION=1

cmake -H.. -Bbuild/rpi -DCMAKE_TOOLCHAIN_FILE=scripts/toolchain-rpi.cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo
make -j -C ../build/rpi
