#ifndef DCL_API_H
#define DCL_API_H 1
#include "dcpref.h"
#include <zmq.h>
#include <semaphore.h>
#include "vilist.h"
#include <map>

#define DCPORT_CUP 52001
#define DCPORT_ADJ 100

#define DCXL_ROWMIN 7
#define DCXL_ROWMAX 206

typedef struct dclmsg {
  vilist_t node;
  int npart;
  uint64_t nid;
  zmq_msg_t part0;
  zmq_msg_t part1;
  zmq_msg_t part2;
  void *dcl;
} dclmsg_t;

typedef struct dclop {
  vilist_t node;
  vilnode_t lnode;
  dclmsg_t *rmsg;
  char *p;
  int plen;
  char *p2;
  int p2len;
  char mbuf[64*1024];
  char *rbuf;
  uint32_t rbuflen;
  char *rbuf2;
  uint32_t rbuf2len;
  uint32_t hash;
  uint32_t dbuflen;
  uint64_t ser;
  sem_t sem;
  void *dlr;
  int ret;
} dclop_t;


typedef struct dclcab {
  uint64_t cab64;
  uint32_t prov;
  uint32_t city;
  uint32_t cnty;
  uint32_t era;
  uint32_t realm;
  uint32_t sect;
  uint32_t nset;
} dclcab_t;

#if 0
typedef struct dclnid {
    uint64_t cab64;
    uint32_t az;
    uint32_t nid;
    uint32_t ext;
    uint32_t nset;
} dclnid_t;
#endif
typedef struct dclnid {
    uint64_t cab64;
    uint32_t prov;
    uint32_t city;
    uint32_t cnty;
    uint32_t era;
    uint32_t realm;
    uint32_t sect;
    uint32_t nset;
} dclnid_t;

extern std::map<std::string, int>  g_dclmapop16;

void *dclInit(char *serverip, uint64_t servernid, uint64_t mynid, char *routerip);
void *dclInitCb(char *serverip, uint64_t servernid, uint64_t mynid, char *routerip, void (*cb)(dclmsg_t *msg));
void dclSendop(void *dcl, dclop_t *op);
void dclFreeop(void *dcl, dclop_t *op);
void dclFreeMsg(dclmsg_t *msg);
dclop_t *dclGetop(void *dcl);
void dclCabExpand(dclcab_t *lcab, char *scab);
void dclNidExpand(dclnid_t *lnid, char *snid);
std::string dclTrim(const std::string& s);
#endif
