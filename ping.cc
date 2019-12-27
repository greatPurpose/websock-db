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
using namespace xlnt;

int g_libslot = 0;
uint16_t g_port;

char router[30];
char *g_routerip = router;
int offset=OFFSET_DCOP_PING; 
char *g_serverip = (char *)"tcp://127.0.0.1:50000";
//char *g_routerip = (char *)"tcp://*:50303";


//uint64_t g_servernid = 0, g_mynid = 133001, g_myport = 50301;
uint64_t g_servernid = 0, g_mynid = 64 * 1024 + OFFSET_DCOP_PING;
uint64_t g_myport = 50301;

int main() {
  char obuf[sizeof(dcpref_t)  + 1024];
  dcpref_t *pref = (dcpref_t *)obuf;

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

  memset((void *)pref, 0, sizeof(dcpref_t));
  pref->op = DCOP_PING;
  pref->ser = 101;

char *cp = obuf + sizeof(dcpref_t);
strcpy(cp, "########################################");

    fprintf(stderr, "ip=%s server nid=0x%x %ld mynid=0x%x %ld rtr=%s\n", 
	g_serverip, (unsigned int)g_servernid, (long int)g_servernid, (int)g_mynid, (long int)g_mynid, g_routerip);

  void *dcl = dclInit(g_serverip, g_servernid, g_mynid, g_routerip);
  if (!dcl) {
    fprintf(stderr, "cannot connect to %s\n", g_serverip);
    _exit(1);
  }
  dclop_t *op = dclGetop(dcl);
  op->p = obuf;
  op->plen = sizeof(dcpref_t);
  dclSendop(dcl, op);
  fprintf(stderr, "ping ret %d\n", (int)(op->ret));

}
