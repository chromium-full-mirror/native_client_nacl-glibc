
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


# TODO(mseaborn): Move these tests into the NaCl Scons build.
# However, some of these tests are rather esoteric.

import os
import subprocess
import tempfile
import unittest


# This is an alternative to subprocess.PIPE, which can cause deadlocks
# if the pipe buffer gets filled up.
def make_fh_pair():
    # Make read/write file handle pair.  This is like creating a pipe
    # FD pair, but without a pipe buffer limit.
    fd, filename = tempfile.mkstemp(prefix="nacl_test_")
    try:
        write_fh = os.fdopen(fd, "w", 0)
        read_fh = open(filename, "r")
    finally:
        os.unlink(filename)
    return write_fh, read_fh


hellow_message = ("Hello world via write()\n"
                  "Hello world via printf(), 1234\n")


class GlibcTests(unittest.TestCase):

    def test_01_glibc_static(self):
        write_fh, read_fh = make_fh_pair()
        subprocess.check_call(["sel_ldr", "-s", "build/hellow-static"],
                              stdout=write_fh)
        self.assertEquals(read_fh.read(), hellow_message)

    def test_02_running_ldso(self):
        write_fh, read_fh = make_fh_pair()
        rc = subprocess.call(["sel_ldr", "-s", "build/elf/runnable-ld.so"],
                             stderr=write_fh)
        self.assertEquals(rc, 127)
        output = read_fh.read()
        # Check for ld.so's help message.
        assert ("You have invoked `ld.so', the helper program "
                "for shared library executables." in output)

    def test_03_running_libcso(self):
        write_fh, read_fh = make_fh_pair()
        subprocess.check_call(
            ["env", "NACLDYNCODE=1", "NACL_DANGEROUS_ENABLE_FILE_ACCESS=1",
             "sel_ldr", "-s", "build/elf/runnable-ld.so",
             "build/libc.so"],
            stdout=write_fh)
        output = read_fh.read()
        assert "GNU C Library stable release version 2.9" in output

    def test_04_glibc_dynamic(self):
        write_fh, read_fh = make_fh_pair()
        subprocess.check_call(
            ["env", "NACLDYNCODE=1", "NACL_DANGEROUS_ENABLE_FILE_ACCESS=1",
             "sel_ldr", "-s", "build/elf/runnable-ld.so",
             "--", "--library-path", "build",
             "build/hellow-dynamic"],
            stdout=write_fh)
        self.assertEquals(read_fh.read(), hellow_message)


if __name__ == "__main__":
    subprocess.check_call(["./nacl/make.sh"])
    unittest.main()
