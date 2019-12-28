#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <libwebsockets.h>
#include <nlohmann/json.hpp>
#include "dclapi.h"
#include "nidoffsets.h"

using namespace std;
using json = nlohmann::json;

char router[30];
char *g_routerip = router;
int offset = OFFSET_DCOP_CUP;

int g_libslot = 0;
uint16_t g_port;
char *g_serverip = (char *)"tcp://127.0.0.1:50000";
//char *g_routerip = (char *)"tcp://*:50301";
uint64_t g_servernid = 0, g_mynid = 64 * 1024 + OFFSET_DCOP_CUP;
void *dcl = NULL;
int toSendCloud(char *recbuf);

static int callback_http(struct lws *wsi,
                         enum lws_callback_reasons reason, void *user,
                         void *in, size_t len)
{
  return 0;
}

static int callback_dumb_increment(struct lws *wsi,
                                   enum lws_callback_reasons reason,
                                   void *user, void *in, size_t len)
{

  switch (reason)
  {
  case LWS_CALLBACK_ESTABLISHED: // just log message that someone is connecting
    printf("connection established\n");
    break;
  case LWS_CALLBACK_RECEIVE:
  { 
    char *tmp = "{ \"dcpref\": { \"cab64s\": \"2-20-9-0-101-9\", \"op\": 22, \"jser\":1}, \"items\": [{\"dck\": { \"op16\": 9,  \"cla\":101, \"clb\": 1, \"cab64s\": \"2-20-9-0-101-9\", \"cab32s\": \"1-1-0-64-0\" },\"dcv\": {\"geo\": [31.3, 108.4],\"segs\": [{\"idx\": 1,  \"typ\": 1, \"flow\":1, \"restrict\":48, \"smin\":70, \"smax\":90, \"lpaint\": 1, \"rpaint\":2},{\"idx\": 2,  \"typ\": 1, \"flow\":1, \"restrict\":48, \"smin\":60, \"smax\":80, \"lpaint\": 2, \"rpaint\":3}]}},{ \"dck\": { \"op16\": 9,  \"cla\":101, \"clb\": 3, \"cab64s\": \"2-20-9-0-101-9\", \"cab32s\": \"1-1-0-64-0\" },\"dcv\": {\"paint\": [{ \"idx\": 1, \"clr\": 0 , \"width\": 1, \"dashed\": 0, \"pts\":[ [103, 0, 5], [103, 0, 3895] ]},{ \"idx\": 2, \"clr\": 0 , \"width\": 1, \"dashed\": 0, \"pts\": [ [403, 0, 5], [403, 0, 3895] ]},{ \"idx\": 3, \"clr\": 0 , \"width\": 1, \"dashed\": 0, \"pts\":[ [703, 0, 5], [703, 0, 3895] ]}]}}]}";
    toSendCloud(tmp); //((char*)in);    
    break;
  }
  default:
    break;
  }

  return 0;
}

static struct lws_protocols protocols[] = {
    /* first protocol must always be HTTP handler */
    {
        "http-only",   // name
        callback_http, // callback
        0              // per_session_data_size
    },
    {
        "dumb-increment-protocol", // protocol name - very important!
        callback_dumb_increment,   // callback
        0                          // we don't use any per session data
    },
    {
        NULL, NULL, 0 /* End of list */
    }};

int toSendCloud(char *recbuf)
{
  char buf[sizeof(dcpref_t) + DCCUP_MAX + 1024];
  dckmycdn_t *k;
  dcpref_t *pref;
  dcslice_t *slice;
  char *dcv;

  json j = json::parse(recbuf);

  memset((void *)buf, 0, sizeof(dcpref_t) + sizeof(dcslice_t) + sizeof(dckmycdn_t));
  char *cp = buf;
  pref = (dcpref_t *)cp;
  cp += sizeof(dcpref_t);
  pref->op = (uint16_t)j["dcpref"]["op"].get<short>();
  std::string s1 = j["dcpref"]["cab64s"].get<std::string>();
  const char *cstr1 = (const char *)s1.c_str();
  dclcab_t lcab1;
  dclCabExpand(&lcab1, (char *)cstr1);
  pref->stamp = lcab1.cab64;
  fprintf(stderr, "pref op stamp %d %ld\n", (int)(pref->op), pref->stamp);
  int vtot = 0;
  for (auto &elem : j["items"])
  {
    slice = (dcslice_t *)cp;
    cp += sizeof(dcslice_t);
    k = (dckmycdn_t *)cp;
    int kslen = 0;
    k->op16 = (uint16_t)elem["dck"]["op16"].get<short>();
    k->cla16 = (uint16_t)elem["dck"]["cla"].get<short>();
    k->clb16 = (uint16_t)elem["dck"]["clb"].get<short>();
    s1 = elem["dck"]["cab64s"].get<std::string>();
    cstr1 = (const char *)s1.c_str();
    dclCabExpand(&lcab1, (char *)cstr1);
    k->cab64 = lcab1.cab64;
    s1 = elem["dck"]["cab32s"].get<std::string>();
    cstr1 = (const char *)s1.c_str();
    dclCab32Expand(&lcab1, (char *)cstr1);
    k->cab32 = lcab1.cab32;
    slice->klen = sizeof(dckmycdn_t);
    cp += sizeof(dckmycdn_t);
    std::string s2 = elem["dcv"].dump();
    const char *cstr2 = s2.c_str();
    int vlen = strlen(cstr2) + 1;
    slice->vlen = vlen;
    strcpy(cp, cstr2);
    cp += vlen;
    vtot += slice->klen + slice->vlen + sizeof(dcslice_t);
    fprintf(stderr, "vlen %d v: %s\n", vlen, cstr2);
    fprintf(stderr, "slice %d %d\n", slice->klen, slice->vlen);
  }
  pref->len = vtot;

  dclop_t *op = dclGetop(dcl);
  op->p = buf;
  op->plen = vtot;
  dclSendop(dcl, op);
  if (op->ret)
  {
    return 0;
  }
  dclFreeop(dcl, op);
  return 1;
}

int main(void)
{
  // server url will be http://localhost:9000
  int port = 9000;
  struct lws_context *context;
  struct lws_context_creation_info context_info;
  /*
    struct lws_context_creation_info context_info =
    {
        .port = port, .iface = NULL, .protocols = protocols, .extensions = NULL,
        .ssl_cert_filepath = NULL, .ssl_private_key_filepath = NULL, .ssl_ca_filepath = NULL,
        .gid = -1, .uid = -1, .options = 0, NULL, .ka_time = 0, .ka_probes = 0, .ka_interval = 0
    };
    */
  memset((void *)&context_info, 0, sizeof(context_info));
  context_info.port = port;
  context_info.protocols = protocols;
  context_info.gid = -1;
  context_info.uid = -1;

  // create lws context representing this server
  context = lws_create_context(&context_info);

  if (context == NULL)
  {
    fprintf(stderr, "lws init failed\n");
    return -1;
  }

  printf("starting web server...\n");

  char *es = getenv("DC_NIDSLOT");
  if (es)
    sscanf(es, "%d", &g_libslot);
  g_port = DCPORT_CUP + g_libslot * DCPORT_ADJ;

  if (offset <= 9)
    sprintf(g_routerip, "tcp://*:5030%d", offset);
  else if (offset <= 99)
    sprintf(g_routerip, "tcp://*:503%d", offset);
  else if (offset >= 100)
  {
    offset += 300;
    sprintf(g_routerip, "tcp://*:50%d", offset);
    printf("\n\noffset > 99 if ERROR probably need more nids added to core\n\n");
  }
  printf("\n offset=%d g_routerip=%s\n", offset, g_routerip);

  dcl = dclInit(g_serverip, g_servernid, g_mynid, g_routerip);
  if (!dcl)
  {
    printf("cannot connect to %s\n", g_serverip);
    _exit(1);
  }
  printf("after dclinit\n");

  // infinite loop, to end this server send SIGTERM. (CTRL+C)
  while (true)
  {
    lws_service(context, 50);    
  }

  lws_context_destroy(context);

  return 0;
}
