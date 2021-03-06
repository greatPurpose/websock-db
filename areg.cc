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
int offset=OFFSET_DCOP_AREG;

char *g_serverip = (char *)"tcp://127.0.0.1:50000";
//char *g_routerip = (char *)"tcp://*:50303";
uint64_t g_servernid = 0, g_mynid = 64 * 1024 + OFFSET_DCOP_AREG;
uint64_t g_myport = 50301;

void doMsg(dclmsg_t *msg) {
  fprintf(stderr, "doMsg found msg\n");
  dclFreeMsg(msg);
}

int main() {
  char obuf[sizeof(dcpref_t)  + 1024];
  dcpref_t *pref = (dcpref_t *)obuf;

  memset((void *)pref, 0, sizeof(dcpref_t));
  pref->op = DCOP_ALERTREG;
  // pref->stamp = (uint64_t)(1) << 32;
  // 20_02_00_00_00_01_00_00
  pref->stamp = (uint64_t) 0x1402000000010000;
  pref->ser = 101;
  pref->off = 1;

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

  void *dcl = dclInitCb(g_serverip, g_servernid, g_mynid, g_routerip, doMsg);
  if (!dcl) {
    fprintf(stderr, "cannot connect to %s\n", g_serverip);
    _exit(1);
  } else
    fprintf(stderr, "connected to g_serverip %s\n", g_serverip);
  dclop_t *op = dclGetop(dcl);
  op->p = obuf;
  op->plen = sizeof(dcpref_t);

    fprintf(stderr, "\ndclSendop stamp=0x%lx %ld\n", pref->stamp, pref->stamp);
  dclSendop(dcl, op);
  fprintf(stderr, "areg ret %d\n", (int)(op->ret));
  for (;;) usleep(5000000);

}
