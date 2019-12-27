#ifndef DCLIB_H
#define DCLIB_H 1
#include <semaphore.h>
#include <zmq.h>
#include <time.h>
#include "vilist.h"
#include "dcpref.h"
#include "dclapi.h"


#define DCL_NOPHASH 64
#define DCL_OPHASH(ser) (ser & (DCL_NOPHASH - 1))


typedef struct dclzmq {
  void *ctx;
  zmq_pollitem_t poller;
  void *rsock;
} dclzmq_t;


// a dealer socket for a nid connection - in a pool
typedef struct dcldlr {
  vilist_t node;
  uint64_t nid;
  void *sock;
  int tick;
} dcldlr_t;

//control structure for operations sent to DC server
// each op has a unique ser, and the response to the router matches the ser with the original


typedef struct dcl {
  uint64_t nser;
  uint64_t mynid;
  dclzmq_t zmq;
  dcldlr_t dlr;
  vilist_t oplru;
  vilist_t msgs;
  vilist_t fmsgs;
  pthread_spinlock_t flock;
  char routerip[32];
  void *router;
  void (*cb)(dclmsg_t *msg);
  vilist_t ophash[DCL_NOPHASH];
  timespec swait;
} dcl_t;

#define DCL_QLOCK(dcl) pthread_spin_lock(&dcl->flock)
#define DCL_QUNLOCK(dcl) pthread_spin_unlock(&dcl->flock)






#endif
