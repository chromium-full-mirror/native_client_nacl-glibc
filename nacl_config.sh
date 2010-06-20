#!/bin/bash

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

if ! which nacl-gcc >/dev/null; then
  echo Error: nacl-gcc not found on PATH
  exit 1
fi

# Since we are taking Linux glibc as a starting point we need the
# Linux kernel headers.
# TODO(mseaborn): Remove this dependency.
rm -rf kernel-headers
mkdir kernel-headers
cp -a /usr/include/asm /usr/include/asm-generic /usr/include/linux \
    kernel-headers/

source_dir=`pwd`

cflags="-march=i486 -pipe -fno-strict-aliasing -O2 -mno-tls-direct-seg-refs -g"

mkdir -p build
cd build
$source_dir/configure \
    --prefix=/usr \
    --host=i486-linux-gnu \
    CC="`which nacl-gcc` -L$source_dir/nacl/dyn-link " \
    CFLAGS="$cflags" \
    --with-headers=$source_dir/kernel-headers \
    --enable-kernel=2.2.0
