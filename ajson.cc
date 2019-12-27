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
int offset=OFFSET_DCOP_CUP; 

int g_libslot = 0;
uint16_t g_port;
char *g_serverip = (char *)"tcp://127.0.0.1:50000";
//char *g_routerip = (char *)"tcp://*:50301";
uint64_t g_servernid = 0, g_mynid = 64 *  1024 + OFFSET_DCOP_CUP;
void*dcl = NULL;
int toSendCloud(char* recbuf);

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
    
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: // just log message that someone is connecting
            printf("connection established\n");
            break;
        case LWS_CALLBACK_RECEIVE: { // the funny part
            // create a buffer to hold our response
            // it has to have some pre and post padding. You don't need to care
            // what comes there, lwss will do everything for you. For more info see
            // http://git.warmcat.com/cgi-bin/cgit/lwss/tree/lib/lwss.h#n597
            //unsigned char *buf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + len +
            //                                             LWS_SEND_BUFFER_POST_PADDING);

            //int i;

            // pointer to `void *in` holds the incomming request
            // we're just going to put it in reverse order and put it in `buf` with
            // correct offset. `len` holds length of the request.
           // for (i=0; i < len; i++) {
           //     buf[LWS_SEND_BUFFER_PRE_PADDING + (len - 1) - i ] = ((char *) in)[i];
           // }

            // log what we recieved and what we're going to send as a response.
            // that disco syntax `%.*s` is used to print just a part of our buffer
            // http://stackoverflow.com/questions/5189071/print-part-of-char-array
            //printf("received data: %s, replying: %.*s\n", (char *) in, (int) len,
            //       buf + LWS_SEND_BUFFER_PRE_PADDING);

            // send response
            // just notice that we have to tell where exactly our response starts. That's
            // why there's `buf[LWS_SEND_BUFFER_PRE_PADDING]` and how long it is.
            // we know that our response has the same length as request because
            // it's the same message in reverse order.
            //lws_write(wsi, &buf[LWS_SEND_BUFFER_PRE_PADDING], len, LWS_WRITE_TEXT);
            
            toSendCloud((char*)in);
            // release memory back into the wild
            //free(buf);
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
        NULL, NULL, 0   /* End of list */
    }
};


int toSendCloud(char* recbuf)
{ 
    json jn= json::parse(std::string(recbuf));
    for (json::iterator it = jn.begin(); it != jn.end(); ++it) {
        std::cout << it.key() << " : " << it.value() << "\n";
    }
    


    char obuf[sizeof(dcpref_t) + DCCUP_MAX + 1024];
    dcpref_t *pref = (dcpref_t *)obuf;
    char *vbuf = &obuf[sizeof(dcpref_t)];
    char *ebuf = vbuf + DCCUP_MAX;
    int tlen, nsheet, sind, rmin, rmax, rind, i;
    char cellnm[64];
    dclcab_t lcab;
    dclnid_t nidcab;
    uint64_t cab64 = 0, ref64 = 0;
    uint32_t cab32 = 0;
    int mapop = 0;
    uint32_t cla16 = 0;

    
   
      tlen = 0;
      char *cp = (char *)pref + sizeof(dcpref_t);
      
        const char *cstr256 = (const char *)recbuf;
        fprintf(stderr, "cstr256 %s\n", cstr256);
        int len256 = (int)strlen(cstr256);
        int klen = len256 + sizeof(dckmycdn_t);
        int vlen = 0;
        if ((tlen + klen + vlen + sizeof(dcslice_t)) > DCCUP_MAX) {
          fprintf(stderr, "sheet %d longer than %d\n", i, DCCUP_MAX);
          return 0;
        }
        if (mapop == DCKOP_NID) {
          fprintf(stderr, "ip %s\n", cstr256);
        }

        dcslice_t *slice = (dcslice_t *)cp;
        cp += sizeof(dcslice_t);
        slice->klen = klen;
        slice->vlen = vlen;
        dckmycdn_t *k = (dckmycdn_t *)cp;
        cp += klen;
        memset((void *)k, 0, sizeof(dckmycdn_t));
        strcpy(k->str256, cstr256);
        k->op16 = mapop;
        k->cab64 = cab64;
        k->ref64 = ref64;
        k->cla16 = cla16;
        k->cab32 = cab32;
        tlen += sizeof(dcslice_t) + klen + vlen;
        fprintf(stderr, "klen vlen tlen %d %d %d\n", klen, vlen, tlen);
         
    dclop_t *op = dclGetop(dcl);
    memset((void *)pref, 0, sizeof(dcpref_t));
    pref->op = DCOP_CUP;
    pref->len = tlen;
    op->p = obuf;
    op->plen = tlen + sizeof(dcpref_t);
    dclSendop(dcl, op);
    if (op->ret) {
    //fprintf(stderr, "update sheet %d failed - check and try\n", g_sheet);
     return 0;
    }
    dclFreeop(dcl, op);
    return 1;
}

int main(void) {
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

    if (context == NULL) {
        fprintf(stderr, "lws init failed\n");
        return -1;
    }

    printf("starting web server...\n");

    char *es = getenv("DC_NIDSLOT");
    if (es) sscanf(es, "%d", &g_libslot);
    g_port = DCPORT_CUP + g_libslot * DCPORT_ADJ;

    if (offset <= 9)
        sprintf(g_routerip,"tcp://*:5030%d", offset);
    else if (offset <= 99)
        sprintf(g_routerip,"tcp://*:503%d", offset);
    else if (offset >=100){
        offset += 300;
        sprintf(g_routerip,"tcp://*:50%d", offset);
        printf("\n\noffset > 99 if ERROR probably need more nids added to core\n\n");
    }
    printf("\n offset=%d g_routerip=%s\n",  offset, g_routerip);

    dcl = dclInit(g_serverip, g_servernid, g_mynid, g_routerip);
    if (!dcl) {
      printf("cannot connect to %s\n", g_serverip);
      _exit(1);
    }
    printf("after dclinit\n");


    // infinite loop, to end this server send SIGTERM. (CTRL+C)
    while (true) {
        lws_service(context, 50);
        // lws_service will process all waiting events with their
        // callback functions and then wait 50 ms.
        // (this is a single threaded webserver and this will keep our server
        // from generating load while there are not requests to process)
    }

    lws_context_destroy(context);

    return 0;
}
