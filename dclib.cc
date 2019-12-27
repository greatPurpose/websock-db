#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <map>
#include <string>
#include <algorithm>
#include <time.h>
#include "dclapi.h"
#include "dclib.h"

using namespace std;

map<std::string, int>  g_dclmapop16;

const std::string DCL_WHITESPACE = " \n\r\t\f\v";

std::string dclLtrim(const std::string& s)
{
	size_t start = s.find_first_not_of(DCL_WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}

std::string dclRtrim(const std::string& s)
{
	size_t end = s.find_last_not_of(DCL_WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string dclTrim(const std::string& s)
{
	return dclRtrim(dclLtrim(s));
}

void dclXlinit (dcl_t *dcl) {
  g_dclmapop16["LBL"] = DCKOP_LBL;
  g_dclmapop16["NID"] = DCKOP_NID;
  g_dclmapop16["REG"] = DCKOP_REG;
  g_dclmapop16["SECT"] = DCKOP_SECT;
  g_dclmapop16["PLEX"] = DCKOP_PLEX;
  g_dclmapop16["DEV"] = DCKOP_DEV;
  g_dclmapop16["DEF"] = DCKOP_DEF;
  g_dclmapop16["ID"] = DCKOP_ID;
  g_dclmapop16["META"] = DCKOP_META;
  g_dclmapop16["STAT"] = DCKOP_STAT;
  // g_dclmapop16["CLBL"] = DCKOP_LBL + DCKOP_CHG;
  // g_dclmapop16["CNID"] = DCKOP_NID + DCKOP_CHG;
  // g_dclmapop16["CREG"] = DCKOP_REG + DCKOP_CHG;
  // g_dclmapop16["CSECT"] = DCKOP_SECT + DCKOP_CHG;
  // g_dclmapop16["CPLEX"] = DCKOP_PLEX + DCKOP_CHG;
  g_dclmapop16["CDEV"] = DCKOP_DEV + DCKOP_CHG;
  g_dclmapop16["CDEF"] = DCKOP_DEF + DCKOP_CHG;
  g_dclmapop16["CID"] = DCKOP_ID + DCKOP_CHG;
  g_dclmapop16["CMETA"] = DCKOP_META + DCKOP_CHG;
  g_dclmapop16["CSTAT"] = DCKOP_STAT + DCKOP_CHG;
  // g_dclmapop16["DLBL"] = DCKOP_LBL + DCKOP_DEL;
  // g_dclmapop16["DNID"] = DCKOP_NID + DCKOP_DEL;
  // g_dclmapop16["DREG"] = DCKOP_REG + DCKOP_DEL;
  // g_dclmapop16["DSECT"] = DCKOP_SECT + DCKOP_DEL;
  // g_dclmapop16["DPLEX"] = DCKOP_PLEX + DCKOP_DEL;
  g_dclmapop16["DDEV"] = DCKOP_DEV + DCKOP_DEL;
  g_dclmapop16["DDEF"] = DCKOP_DEF + DCKOP_DEL;
  g_dclmapop16["DID"] = DCKOP_ID + DCKOP_DEL;
  g_dclmapop16["DMETA"] = DCKOP_META + DCKOP_DEL;
  g_dclmapop16["DSTAT"] = DCKOP_STAT + DCKOP_DEL;

}

void dclCabExpand(dclcab_t *lcab, char *scab) {
  memset ((void *)lcab, 0, sizeof(dclcab_t));
	lcab->nset = sscanf(scab, "%u_%u_%u_%u_%u_%u", &lcab->prov, &lcab->city, &lcab->cnty, &lcab->era, &lcab->realm, &lcab->sect);
	lcab->cab64 = ((uint64_t)(lcab->prov) << 56) + ((uint64_t)(lcab->city) << 48) + ((uint64_t)(lcab->cnty) << 40) +
					((uint64_t)(lcab->era) << 32) + ((uint64_t)(lcab->realm) << 16) + ((uint64_t)(lcab->sect));

	#if 0
  lcab->nset = sscanf(scab, "%u_%u_%u_%u_%u_%u_%u_%u", &lcab->az, &lcab->reg, &lcab->sect, &lcab->zn, &lcab->area,
      &lcab->flr, &lcab->dev, &lcab->ext);
  lcab->cab64 = DCCAB_AZ2CAB(lcab->az) + DCCAB_REG2CAB(lcab->reg) + DCCAB_SECT2CAB(lcab->sect) +
      DCCAB_ZN2CAB(lcab->zn) + DCCAB_AREA2CAB(lcab->area) + DCCAB_FLR2CAB(lcab->flr) + DCCAB_DEV2CAB(lcab->dev);
			#endif
}

#if 0
void dclNidExpand(dclnid_t *lnid, char *snid) {
    memset ((void *)lnid, 0, sizeof(dclnid_t));
  lnid->nset = sscanf(snid, "%u_%u_%u", &lnid->az, &lnid->nid, &lnid->ext);
  fprintf(stderr, "nidex  %x %x\n", lnid->az, lnid->nid);
  lnid->cab64 = DCCAB_AZ2CAB(lnid->az) + lnid->nid;
}
#endif
void dclNidExpand(dclnid_t *lnid, char *snid) {
    memset ((void *)lnid, 0, sizeof(dclnid_t));
  lnid->nset = sscanf(snid, "%u_%u_%u_%u_%u_%u", &lnid->prov, &lnid->city, &lnid->cnty, &lnid->era, &lnid->realm, &lnid->sect);
	lnid->cab64 = ((uint64_t)(lnid->prov) << 56) + ((uint64_t)(lnid->city) << 48) + ((uint64_t)(lnid->cnty) << 40) +
					((uint64_t)(lnid->era) << 32) + ((uint64_t)(lnid->realm) << 16) + ((uint64_t)(lnid->sect));
}

// get a free message structure, usleep if all in use (should not happen)
dclmsg_t *dclMsgget(dcl_t *dcl) {
  vilist_t *node;
  for (;;) {
    DCL_QLOCK(dcl);
    node = dcl->msgs.prev;
    if (!node) {
      DCL_QUNLOCK(dcl);
      usleep(10*1000);
      continue;
    }
    vilistDel(node, &dcl->msgs);
    DCL_QUNLOCK(dcl);
    break;
  }
  return (dclmsg_t *)node;
}

void dclMsgclose(dclmsg_t *msg) {
  dcl_t *dcl = (dcl_t *)(msg->dcl);
  zmq_msg_close(&msg->part0);
  zmq_msg_close(&msg->part1);
  if (msg->npart > 1) zmq_msg_close(&msg->part2);
  DCL_QLOCK(dcl);
  vilistAdd(&msg->node, &dcl->msgs);
  DCL_QUNLOCK(dcl);
}

// router thread
// only RESP messaeges fpr now
void *dclRouter(void *p) {
  dcl_t *dcl = (dcl_t *)p;
  vilist_t *node, *hnode;
  dclzmq_t *zmq = &dcl->zmq;
  dclmsg_t *msg;
  dcpref_t *pref;
  zmq_msg_t *part1, *part2, *part0;
  int64_t more;

  size_t more_size = sizeof(more);
  void *rsock =  zmq_socket (zmq->ctx, ZMQ_ROUTER);
  int rc = zmq_bind(rsock, dcl->routerip);
  if (rc) {
    fprintf(stderr, "router bind failure");
    exit(1);
  }
  dcl->router = rsock;
  node = NULL;
	fprintf(stderr, "starting router loop on %s %p\n", dcl->routerip, rsock);
  for (;;) {
    if (!node) {
      DCL_QLOCK(dcl);
      if ((node = dcl->msgs.prev)) vilistDel(node, &dcl->msgs);
      DCL_QUNLOCK(dcl);
    }
    if (!node) {
      usleep(10*1000);
      continue;
    }
		fprintf(stderr, "router before msg recv\n");
    // TODO: lun64 part is missing
    msg = (dclmsg_t *)node;
    part0 = &msg->part0;
    int ret = zmq_msg_recv (part0, dcl->router, 0);
    if (ret < 0) {
			fprintf(stderr, "recv err %d\n", errno);
			continue;
		}
		fprintf(stderr, "router found msg\n");
    char *p = (char *)zmq_msg_data(part0);
    sscanf(p, "%lx", &msg->nid);
    msg->npart = 1;
    part1 = &msg->part1;
    (void)zmq_msg_recv (part1, dcl->router, 0);
    more = 0;
    (void) zmq_getsockopt (dcl->router, ZMQ_RCVMORE, &more, &more_size);
    if (more) {
      part2 = &msg->part2;
      (void)zmq_msg_recv (part2, dcl->router, 0);
      msg->npart = 2;
    }
    char *rbuf = (char *)zmq_msg_data(part1);
    int rlen = (int)zmq_msg_size(part1);
    if (rlen < sizeof(dcpref_t)) {
      dclMsgclose(msg);
      node = NULL;
      continue;
    }

    pref = (dcpref_t *)rbuf;
		fprintf(stderr, "router op %d ser %d\n", (int)pref->op, (int)pref->ser);

    // not a response message
    if ((pref->op & DCOP_RESP) == 0) {
      if (!dcl->cb)dclMsgclose(msg);
			else dcl->cb(msg);
      node = NULL;
      continue;
    }
		fprintf(stderr, "router matching %d ser %d\n", (int)pref->op, (int)pref->ser);
    //match the return message to the original request (by pref->ser), and wake up the op
    uint64_t ser = pref->ser;
    uint32_t hash = DCL_OPHASH(ser);    /// see where is ophash

    DCL_QLOCK(dcl);
    int found = 0;
    for (hnode = dcl->ophash[hash].prev; hnode; hnode = hnode->next) {
      dclop_t *op = (dclop_t *)hnode;
      if (op->ser == ser) {
        vilistDel(hnode, &dcl->ophash[hash]);
        found = 1;
        DCL_QUNLOCK(dcl);
        op->rmsg = msg;
        op->rbuf = rbuf;
        op->rbuflen = rlen;
        op->rbuf2len = 0;
        if (more) {
          op->rbuf2 = (char *)zmq_msg_data(part2);
          op->rbuf2len = (int)zmq_msg_size(part2);
        }
        sem_post(&op->sem);
        break;
      }
    }
    if (!found) DCL_QUNLOCK(dcl);
    node = NULL;
    if (!hnode) dclMsgclose(msg);		// msg did not match an op in the hash
  }
  return NULL;
}

// get op from lru, malloc a new one if they are all in use (highly unlikely)
dclop_t *dclGetop(void *p) {
  dcl_t *dcl = (dcl_t *)p;
  dclop_t *op = NULL;
  vilist_t *node;
  vilnode_t *lnode;
  for (;;) {
    DCL_QLOCK(dcl);
    if ((node = dcl->oplru.prev)) {
      vilistDel(node, &dcl->oplru);
      lnode = (vilnode_t *)node;
      op = (dclop_t *)lnode;
    }
    DCL_QUNLOCK(dcl);
    if (op) return op;
    op = (dclop_t *)calloc(1, sizeof(dclop_t));
    DCL_QLOCK(dcl);
    op->lnode.p = (void *)op;
    vilistAdd((vilist *)(&op->lnode), &dcl->oplru);
    op = NULL;
    DCL_QUNLOCK(dcl);
  }
}

void dclFreeMsg(dclmsg_t *msg) {
	dcl_t *dcl = (dcl_t *)(msg->dcl);
	zmq_msg_close(&msg->part0);
	zmq_msg_close(&msg->part1);
	if (msg->npart > 1) zmq_msg_close(&msg->part2);
	DCL_QLOCK(dcl);
	if (msg) vilistAdd(&msg->node, &dcl->msgs);
	DCL_QUNLOCK(dcl);
}

// put completed op back on lru - free rmsg if attached
void dclFreeop(void *p, dclop_t *op) {
  dcl_t *dcl = (dcl_t *)p;
  dclmsg_t *msg = op->rmsg;
  if (msg) {
    zmq_msg_close(&msg->part0);
    zmq_msg_close(&msg->part1);
    if (msg->npart > 1) zmq_msg_close(&msg->part2);
  }

  DCL_QLOCK(dcl);
  vilistAdd((vilist_t *)(&op->lnode),&dcl->oplru);
  if (msg) vilistAdd(&msg->node, &dcl->msgs);
  DCL_QUNLOCK(dcl);
}

// an op can override the default dealer, which is the first one
// local utils cannot do this
// the op wakes op on a resp message or timeout
// the op is off the hash q, but not back on the lru
void dclSendop(void *p, dclop_t *op) {
  dcl_t *dcl = (dcl_t *)p;
  vilist_t *hnode;
  dclop_t *hop;
  dcldlr_t *dlr = (dcldlr_t *)op->dlr;
  uint64_t ser;
  uint32_t hash;
	int ret = 0;
  if (!dlr) dlr = &dcl->dlr;
  //dcpref_t *pref = (dcpref_t *)(&op->mbuf);
  //int dlen = sizeof(dcpref_t) + pref->len;
  sem_init(&op->sem, 0, 0);
  op->ret = 0;
  op->rmsg = NULL;
  DCL_QLOCK(dcl);
  ser = op->ser = ++dcl->nser;
	dcpref_t *pref = (dcpref_t *)(op->p);
	pref->ser = ser;
  hash = DCL_OPHASH(ser);
  vilistAdd(&op->node, &dcl->ophash[hash]);
  DCL_QUNLOCK(dcl);
  zmq_send(dlr->sock, (void *)op->p, op->plen, 0);
	dcl->swait.tv_sec = time(NULL) + 20;
//  int ret = sem_timedwait(&op->sem, &dcl->swait);
fprintf(stderr, "dclSendop waiting\n");
ret = sem_timedwait(&op->sem, &dcl->swait);
	//sem_wait(&op->sem);
  if (ret) {
		fprintf(stderr, "dclSendop timeout\n");
      ret = 0;
      DCL_QLOCK(dcl);
      for (hnode = dcl->ophash[hash].prev; hnode; hnode = hnode->next) {
        dclop_t *hop = (dclop_t *)hnode;
        if (hop->ser == ser) {
            vilistDel(hnode, &dcl->ophash[hash]);
            ret = EIO;
            break;
        }
      DCL_QUNLOCK(dcl);
    }
  }
  op->ret = ret;
}

// create a pool of blank op structures
void dclOpinit(dcl_t *dcl) {
  dclop_t *op;
  int i;
  char *cp;
  int mlen = DCL_NOPHASH * sizeof(dclop_t);
  void *p = malloc(mlen);
  char *ep = (char *)p + mlen;
  for ((cp = (char *)p), i = 0; i < DCL_NOPHASH; cp += sizeof(dclop_t), i++) {
    op = (dclop_t *)cp;
    op->ser = 0;
    op->node.prev = op->node.next = op->lnode.prev = op->lnode.next = 0;
    op->lnode.p = (void *)op;
    sem_init(&op->sem, 0, 0);
    //vilistAdd(&op->node, &g_nextophash[i]);
    vilistAdd((vilist *)(&op->lnode), &dcl->oplru);
  }
}


void dclMsginit (dcl_t *dcl) {
  int i;
  void *p = calloc(DCL_NOPHASH, sizeof(dclmsg_t));
  dclmsg_t *msg = (dclmsg_t *)p;
  for (i = 0; i < DCL_NOPHASH; i++,msg++) {
    (void)zmq_msg_init(&msg->part0);
    (void)zmq_msg_init(&msg->part1);
    (void)zmq_msg_init(&msg->part2);
    msg->dcl = (void *)dcl;
    vilistAdd(&msg->node, &dcl->msgs);
  }
}

void *dclInitCb(char *serverip, uint64_t servernid, uint64_t mynid, char *routerip, void (*cb)(dclmsg_t *msg) ) {
	void *p = dclInit(serverip, servernid, mynid, routerip);
	 dcl_t *dcl = (dcl_t *)p;
	 dcl->cb = cb;
	 return p;
}

void *dclInit(char *serverip, uint64_t servernid, uint64_t mynid, char *routerip ) {
  char idbuf[32];
  void *p = calloc(1, sizeof(dcl_t));
  dcl_t *dcl = (dcl_t *)p;
  pthread_spin_init(&dcl->flock, 0);
	strcpy(dcl->routerip, routerip);
  dcl->swait.tv_sec = 2;
  dcl->swait.tv_nsec = 0;
  dclMsginit(dcl);
  dclOpinit(dcl);
  dclXlinit (dcl);
  dcl->mynid = mynid;
  fprintf(stderr, "before dealer sock\n");

  dcldlr_t *dlr = &dcl->dlr;
  dlr->nid = servernid;
  int stimeo = 900;
  dclzmq_t *zmq = &dcl->zmq;
  zmq->ctx = zmq_ctx_new();
  void *sock = zmq_socket (zmq->ctx, ZMQ_DEALER);
  sprintf(idbuf, "%lx", mynid);
  (void)zmq_setsockopt (sock, ZMQ_IDENTITY, (const void *)idbuf, strlen(idbuf) +1);
  (void)zmq_setsockopt (sock, ZMQ_SNDTIMEO, (const void *)&stimeo, sizeof(int));
  (void)zmq_connect(sock, serverip);
  dlr->sock = sock;
  fprintf(stderr, "aft dealer sock %p %s\n", dlr->sock, idbuf);
	pthread_t thread;
	(void)pthread_create(&thread, nullptr, dclRouter, p);
	usleep(500000);
  return p;
}
