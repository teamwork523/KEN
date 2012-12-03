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

/* Example illustrating use of C++ STL with Ken.  This file contains
   both server and client code; the latter should be compiled as
   ordinary Ken C code, not C++.

   Server implements simple key-value store.  Read commands in
   messages or inputs and reply or print results, respectively.
   Formats:

       "P" key value  # put
       "G" key "x"    # get
       "D" key "x"    # delete

   The key and value fields are alphanumeric strings; the literal "x"
   on Get & Delete commands simplifies parsing.  Only a Get elicits
   a reply.
*/

#define BLEN 1024

#ifndef APP04_CLIENT

#include <cassert>
#include <cstdio>  /* for sscanf(), snprintf() */
#include <map>
#include <new>
#include <string>
extern "C" {
#include "ken.h"
#include "kenapp.h"
}

/* begin overloading new/delete etc. */
static void * mynew (size_t s) {
  assert(0 != ken_heap_ready);
  void * m = ken_malloc(s);
  assert(NULL != m);
  return m;
}
void * operator new   (size_t s) { return mynew(s); }
void * operator new[] (size_t s) { return mynew(s); }
static void myfree(void * p) {
  assert(0 != ken_heap_ready && NULL != p);
  ken_free(p);
}
void operator delete   (void * p) { myfree(p); }
void operator delete[] (void * p) { myfree(p); }
/* end overloading of new/delete etc. */

using namespace std;

int64_t ken_handler(void *msg, int32_t len, kenid_t sender) {
  map<string, string> *stp;
  if (0 == ken_id_cmp(sender, kenid_NULL)) {
    stp = new map<string, string>;
    assert(NULL != stp);
    ken_set_app_data(stp);
  }
  else {
    map<string, string>::const_iterator it;
    int r;
    char C[BLEN], K[BLEN], W1[2], V[BLEN], W2[2], M[2 * BLEN];
    assert(BLEN > len);
    stp = (map<string, string>*)ken_get_app_data();
    assert(NULL != stp);
    r = sscanf((const char *)msg, "%1[PGD]%1[ ]%[a-zA-Z0-9]%1[ ]%[a-zA-Z0-9]\n",
                                   C,     W1,  K,          W2,  V);
    assert(5 == r && ' ' == W1[0] && '\0' == W1[1] && ' ' == W2[0] && '\0' == W2[1]);
    assert('P' == C[0] || ('x' == V[0] && '\0' == V[1]));
    switch (C[0]) {
    case 'G':
      it = stp->find(K);
      r = snprintf(M, sizeof M, "R %s %s\n", K, (it == stp->end() ? "?" : it->second.c_str()));
      assert(5 < r && sizeof M > (size_t)r);
      (void)ken_send((0 == ken_id_cmp(kenid_stdin, sender) ? kenid_stdout : sender), M, r);
      break;
    case 'P':  (*stp)[K] = V;                                  break;
    case 'D':  if (stp->find(K) != stp->end()) stp->erase(K);  break;
    default:   assert(0);                                      break;
    }
  }
  return -1;
}

#else   /* #ifndef APP04_CLIENT */

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include "ken.h"
#include "kenapp.h"

static void mysend(const kenid_t srv, const char typ, const int64_t C) {
  char M[BLEN], A[] = "0123456789";
  int r = snprintf(M, sizeof M, "%c key%" PRId64 " %c\n", typ, C,
                   'P' == typ ? A[C % (sizeof A - 1)] : 'x');
  assert(5 < r && sizeof M > (size_t)r);
  (void)ken_send(srv, M, r);
}

int64_t ken_handler(void *msg, int32_t len, kenid_t sender) {
  typedef struct { int64_t C, T; } S;   /* counter & alarm time */
  S *p;
  kenid_t server;
  assert(3 == ken_argc());
  server = ken_id_from_string(ken_argv(2));
  if (0 == ken_id_cmp(kenid_NULL, sender)) {
    p = (S *)ken_malloc(sizeof *p);
    assert(NULL != p);
    p->C = p->T = 0;
    ken_set_app_data(p);
  }
  else {
    p = (S *)ken_get_app_data();
    assert(NULL != p);
    if (0 == ken_id_cmp(server, sender)) {
      (void)ken_send(kenid_stdout, msg, len);
    }
    else {
      assert(0 == ken_id_cmp(kenid_alarm, sender));
      mysend(server, 'G', p->C);
      mysend(server, 'P', p->C);
      mysend(server, 'G', p->C);
      mysend(server, 'D', p->C);
      mysend(server, 'G', p->C);
      (p->C)++;
      p->T = ken_current_time() + 1000 * 1000;
    }
  }
  return p->T;
}

#endif   /* #ifndef APP04_CLIENT */

