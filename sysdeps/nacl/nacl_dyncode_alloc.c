/*
 * Copyright 2010 The Native Client Authors.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <sys/mman.h>
#include <unistd.h>

#include <ldsodefs.h>
#include <nacl_dyncode.h>


extern char __etext[]; /* Defined by the linker script */

static char *nacl_next_code;
static char *nacl_next_data;

static size_t round_up_to_pagesize (size_t val)
{
  return (val + GLRO(dl_pagesize) - 1) & ~(GLRO(dl_pagesize) - 1);
}

/* Allocate space for code and data simultaneously.
   This is a simple allocator that doesn't know how to deallocate.
   TODO(mseaborn): Track the location of the fixed-address executable
   too, so that we can skip past it if necessary. */
void *nacl_dyncode_alloc (size_t code_size, size_t data_size,
                          size_t data_offset)
{
  if (nacl_next_code == NULL)
    {
      /* Place data after whatever brk() heap has been allocated so
         far.  This will mean the brk() heap cannot be extended any
         further.  TODO(mseaborn): Ideally place ld.so and brk() heap
         at a high address so that library data can be mapped below
         and not get in the way of the brk() heap. */
      nacl_next_code = __etext;
      nacl_next_data = (char *) round_up_to_pagesize((size_t) __sbrk(0));
    }

  code_size = round_up_to_pagesize (code_size);
  data_size = round_up_to_pagesize (data_size);

  if (data_size != 0)
    {
      size_t last_offset = nacl_next_data - nacl_next_code;
      if (data_offset > last_offset)
        {
          /* Leaves unused space in the data area. */
          nacl_next_data += data_offset - last_offset;
        }
      else if (data_offset < last_offset)
        {
          /* Leaves unused space in the code area. */
          nacl_next_code += last_offset - data_offset;
        }
      assert (nacl_next_code + data_offset == nacl_next_data);

      /* Check whether the data space is available and reserve it by
         using an address hint without MAP_FIXED.
         TODO(mseaborn): Retry on failure or avoid failure by
         reserving a big chunk of address space at startup. */
      void *mapped = __mmap (nacl_next_data, data_size, PROT_NONE,
                             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      if (mapped == MAP_FAILED)
        {
          return NULL;
        }
      if (mapped != nacl_next_data)
        {
          __munmap (nacl_next_data, data_size);
          return NULL;
        }
    }

  void *code_addr = nacl_next_code;
  nacl_next_data += data_size;
  nacl_next_code += code_size;
  return code_addr;
}
