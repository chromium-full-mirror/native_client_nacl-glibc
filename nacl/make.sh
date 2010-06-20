#!/bin/sh

# Copyright 2010 The Native Client Authors.  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

set -eu

srcdir=nacl
builddir=build

nacl-gcc -g -Wall $srcdir/hellow.c \
    -static -nostdlib -Wl,-T,$srcdir/elf_i386.x \
    '-Wl,-(' \
    -lgcc \
    $builddir/csu/crt1.o \
    $builddir/csu/crti.o \
    $builddir/csu/crtn.o \
    $builddir/libc.a \
    '-Wl,-)' \
    -o $builddir/hellow-static

nacl-gcc -g -Wall $srcdir/hellow.c \
    -nostdlib -L$srcdir/dyn-link \
    -lgcc \
    $builddir/csu/crt1.o \
    $builddir/csu/crti.o \
    $builddir/csu/crtn.o \
    $builddir/libc.so $builddir/libc_nonshared.a $builddir/elf/ld.so \
    -o $builddir/hellow-dynamic
