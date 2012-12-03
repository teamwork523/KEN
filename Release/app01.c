#if 0

    Copyright (c) 2011-2012, Hewlett-Packard Development Co., L.P.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the
      distribution.

    * Neither the name of the Hewlett-Packard Company nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
    OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
    AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
    WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

#endif

/* "Take a number" */

#include <string.h>
#include <inttypes.h>
#include "ken.h"
#include "kenapp.h"

typedef int64_t * ptype;

int64_t ken_handler(void *msg, int32_t len, kenid_t sender) {
  ptype p;
  if (NULL == msg) {
    assert(0 == len);
    p = (ptype)ken_malloc(sizeof *p);
    *p = 0;
    ken_set_app_data(p);
  }
  else {
    assert(0 < len);
    p = (ptype)ken_get_app_data();
    (*p)++;
    if (0 == ken_id_cmp(sender, kenid_stdin)) {
      int r;
      char buf[22];
      FP1("got input on stdin\n");
      r = snprintf(buf, sizeof buf, "%09" PRId64 "\n", *p);
      assert((int)sizeof buf > r);
      (void)ken_send(kenid_stdout, buf, strlen(buf));
    }
    else {
      char buf[KEN_ID_BUF_SIZE];
      ken_id_to_string(sender, buf, sizeof buf);
      FP2("got message from %s\n", buf);
      (void)ken_send(sender, p, sizeof *p);
    }
  }
  return -1;
}

