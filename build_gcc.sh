#!/bin/bash
cmake -H. -Bbuild/gcc -DTARGET=release -DFLOAT_PRECISION=DOUBLE
