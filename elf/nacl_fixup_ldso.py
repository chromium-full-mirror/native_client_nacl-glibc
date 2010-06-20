
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


# Modifies ld.so's ELF Program Headers to get a version that NaCl's
# sel_ldr will run.
#
# For a discussion, see:
# http://code.google.com/p/nativeclient/issues/detail?id=156
# http://code.google.com/p/nativeclient/issues/detail?id=558

import struct
import sys


offset_e_type = 16
offset_e_phnum = 44

ET_EXEC = 2
ET_DYN = 3


def main(args):
    if len(args) != 1:
        raise Exception("Usage: fixup <filename>")
    filename = args[0]
    fh = open(filename, "r+")
    
    def check(ty, offset, expected):
        fh.seek(offset)
        data = fh.read(struct.calcsize(ty))
        got = struct.unpack(ty, data)[0]
        if got != expected:
            raise AssertionError("Expected %s, got %s" % (expected, got))

    def replace(ty, offset, value):
        fh.seek(offset)
        fh.write(struct.pack(ty, value))

    # sel_ldr only accepts ELF objects with e_type=ET_EXEC.
    check("B", offset_e_type, ET_DYN)
    replace("B", offset_e_type, ET_EXEC)

    # sel_ldr rejects ELF Program Headers other than PT_LOAD.
    # Drop PT_DYNAMIC, PT_GNU_STACK and PT_TLS.
    check("H", offset_e_phnum, 6)
    replace("H", offset_e_phnum, 3)

    fh.close()


if __name__ == "__main__":
    main(sys.argv[1:])
