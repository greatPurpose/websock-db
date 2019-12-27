#include <iostream>
#include <xlnt/xlnt.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "dclapi.h"
#include "nidoffsets.h"

using namespace std;

char router[30];
char *g_routerip = router;
int offset=OFFSET_DCOP_APOST;

char *g_serverip = (char *)"tcp://127.0.0.1:50000";
//char *g_routerip = (char *)"tcp://*:50304";
uint64_t g_servernid = 0, g_mynid = 64 * 1024 + OFFSET_DCOP_APOST;
uint64_t g_myport = 50301;

int main() {
  char obuf[sizeof(dcpref_t)  + 1024];
  dcpref_t *pref = (dcpref_t *)obuf;
  dckmycdn_t *k = (dckmycdn_t *)((char *)pref + sizeof(dcpref_t));

  memset((void *)pref, 0, sizeof(dcpref_t));
  memset((void *)k, 0, sizeof(dckmycdn_t));
  pref->op = DCOP_ALERTSEND;
  pref->stamp = (uint64_t)(1) << 32;
  pref->ser = 101;
  pref->off = 1;
  pref->len = sizeof(dckmycdn_t);
  k->op16 = DCKOP_ALERT;
  k->cab64 = ((uint64_t)(177) << 32) + 0x10000;
  k->id64 = ((uint64_t)(200) << 32) + 0x10;
  k->ref64 = 0x101;
  k->clb16 = 0x101;
  sprintf(k->str256, "suspect %lx found at cam %lx\n", k->id64, k->cab64 );
  pref->len = sizeof(dckmycdn_t) + strlen(k->str256);

  if (offset <= 9)
        sprintf(g_routerip,"tcp://*:5030%d", offset);
  else if (offset <= 99)
        sprintf(g_routerip,"tcp://*:503%d", offset);
  else if (offset >=100){
        offset += 300;
        sprintf(g_routerip,"tcp://*:50%d", offset);
        fprintf(stderr,"\n\noffset > 99 if ERROR probably need more nids added to core\n\n");
  }
  fprintf(stderr,"\n offset=%d g_routerip=%s\n",  offset, g_routerip);

  void *dcl = dclInit(g_serverip, g_servernid, g_mynid, g_routerip);
  if (!dcl) {
    fprintf(stderr, "cannot connect to %s\n", g_serverip);
    _exit(1);
  }
  dclop_t *op = dclGetop(dcl);
  op->p = obuf;
  op->plen = sizeof(dcpref_t) + pref->len;
  dclSendop(dcl, op);
  fprintf(stderr, "apost ret %d\n", (int)(op->ret));

}
