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

/* ennui:  simple illustration of alarms & kenvar interface */

#include <inttypes.h>
#include "ken.h"
#include "kenapp.h"
#include "kenvar.h"

typedef int * ptype;

int64_t ken_handler(void *msg, int32_t len, kenid_t sender) {
  ptype c;
  if (0 == ken_id_cmp(sender, kenid_NULL)) {
    assert(0 == len && NULL == msg);
    FP1("\n\nthe world is exciting and new!\n");
    c = (ptype)ken_malloc(sizeof *c);
    assert(NULL != c);
    *c = 0;
    kenvar_set("myvar", c);
  }
  else {
    c = (ptype)kenvar_get("myvar");
    assert(NULL != c);
    if (0 == ken_id_cmp(sender, kenid_alarm)) {
      FP3("i'm bored.  it's %" PRId64 ".  i've gotten %d messages\n",
          ken_current_time(), *c);
    }
    else {
      (*c)++;
      FP2("news just arrived!  message # %d\n", *c);
    }
  }
  return ken_current_time() + 1000 * 1000;
}

