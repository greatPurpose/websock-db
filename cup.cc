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

char router[30];
char *g_routerip = router;
int offset=OFFSET_DCOP_CUP; 


int g_libslot = 0;
uint16_t g_port;
char *g_serverip = (char *)"tcp://127.0.0.1:50000";
//char *g_routerip = (char *)"tcp://*:50301";
uint64_t g_servernid = 0, g_mynid = 64 *  1024 + OFFSET_DCOP_CUP;
uint32_t g_sheet;

int main(int argc, char *argv[])
{
    xlnt::workbook wb;
    char obuf[sizeof(dcpref_t) + DCCUP_MAX + 1024];
    dcpref_t *pref = (dcpref_t *)obuf;
    char *vbuf = &obuf[sizeof(dcpref_t)];
    char *ebuf = vbuf + DCCUP_MAX;
    int tlen, nsheet, sind, rmin, rmax, rind, i, k;
    char cellnm[64];
    dclcab_t lcab;
    dclnid_t nidcab;
    uint64_t cab64, ref64;
    uint32_t cab32;
    int mapop;
    uint32_t cla16;

    memset((void *)pref, 0, sizeof(dcpref_t));


    char *es = getenv("DC_NIDSLOT");
    if (es) sscanf(es, "%d", &g_libslot);
    g_port = DCPORT_CUP + g_libslot * DCPORT_ADJ;

    wb.load(argv[1]);
    //auto ws = wb.active_sheet();

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
    fprintf(stderr, "after dclinit\n");

fprintf(stderr, "op NID %x\n", (int)g_dclmapop16["NID"]);
    nsheet = (int)wb.sheet_count();
    nsheet = 1;     /// HACK!!!!!!!!!!
    for (i = 0; i < nsheet; i++) {
      tlen = 0;
      char *cp = (char *)pref + sizeof(dcpref_t);
      worksheet ws = wb.sheet_by_index(0);
      rmin = ws.lowest_row();
      rmax = ws.highest_row();
      if (rmin > DCXL_ROWMIN || rmax > DCXL_ROWMAX) {
        fprintf(stderr, "sheet %d - bad row range %d - %d\n", i, rmin, rmax);
        _exit(1);
      }
      fprintf(stderr, "processing sheet %d rows %d  to %d \n",i, DCXL_ROWMIN, ws.highest_row());
      for (k = DCXL_ROWMIN; k <= rmax; k++) {
        sprintf(cellnm, "C%d", k);
        cell c = ws.cell((const char *)cellnm);
        string s = dclTrim(c.to_string());
        if (s.empty()) continue;
        const char *cstr = (const char *)s.c_str();
        fprintf(stderr, "%s %s\n", cellnm, cstr);
        if (g_dclmapop16.count(s)) fprintf(stderr, "op %x\n", (int)g_dclmapop16[s]);
        else {
          fprintf(stderr, "invalid op (%s)\n", cstr);
          continue;
        }
        if ((int)g_dclmapop16[s] > 5) continue;   //temp hack
        mapop = (int)g_dclmapop16[s];
        sprintf(cellnm, "D%d", k);
        c = ws.cell((const char *)cellnm);
        s = dclTrim(c.to_string());
        cstr = (const char *)s.c_str();
        if (mapop == DCKOP_NID || mapop == DCKOP_PLEX) {
          dclNidExpand(&nidcab, (char *)cstr);
          cab64 = nidcab.cab64;
        } else {
          dclCabExpand(&lcab, (char *)cstr);
          cab64 = lcab.cab64;
        }
        fprintf(stderr, "%s %s %lx\n", cellnm, cstr, cab64);

        sprintf(cellnm, "E%d", k);
        c = ws.cell((const char *)cellnm);
        s = dclTrim(c.to_string());
        cstr = (const char *)s.c_str();
        cab32 = 0;
        sscanf(cstr, "%d", &cab32);

        sprintf(cellnm, "M%d", k);
        c = ws.cell((const char *)cellnm);
        s = dclTrim(c.to_string());
        cstr = (const char *)s.c_str();
        cla16 = 0;
        sscanf(cstr, "%u", &cla16);


        ref64 = 0;
        if (mapop == DCKOP_PLEX) {
          sprintf(cellnm, "I%d", k);
          c = ws.cell((const char *)cellnm);
          s = dclTrim(c.to_string());
          cstr = (const char *)s.c_str();
          dclNidExpand(&nidcab, (char *)cstr);
          ref64 = nidcab.cab64;
        }

        sprintf(cellnm, "Q%d", k);
        c = ws.cell((const char *)cellnm);
        string s256 = dclTrim(c.to_string());
        const char *cstr256 = (const char *)s256.c_str();
        fprintf(stderr, "cstr256 %s\n", cstr256);
        int len256 = (int)strlen(cstr256);
        int klen = len256 + sizeof(dckmycdn_t);
        int vlen = 0;
        if ((tlen + klen + vlen + sizeof(dcslice_t)) > DCCUP_MAX) {
          fprintf(stderr, "sheet %d longer than %d\n", i, DCCUP_MAX);
          _exit(1);
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
      }
      #if 1
          dclop_t *op = dclGetop(dcl);
          memset((void *)pref, 0, sizeof(dcpref_t));
          pref->op = DCOP_CUP;
          pref->len = tlen;
          op->p = obuf;
          op->plen = tlen + sizeof(dcpref_t);
          dclSendop(dcl, op);
          if (op->ret) {
            fprintf(stderr, "update sheet %d failed - check and try\n", g_sheet);
            _exit(1);
          }
          dclFreeop(dcl, op);
      #endif
    }

      #if 0
    std::clog << "Processing spread sheet" << std::endl;
    fprintf(stderr, "sheets %d\n", (int)wb.sheet_count());
    worksheet ws = wb.sheet_by_index(0);
    //fprintf(stderr, "%d %d %d %d\n", ws.lowest_column(), ws.highest_column(),ws.lowest_row(), ws.highest_row());
        fprintf(stderr, "%d %d \n",DCXL_ROWMIN, ws.highest_row());
    cell c = ws.cell("C7");
    string s = c.to_string();
    //int b = (int)c;
     fprintf(stderr, "%s %d %d\n", s.c_str(), g_libslot, g_port);

    for (auto row : ws.rows(false))
    {
        for (auto cell : row)
    {
        std::clog << cell.to_string() << std::endl;
    }
    }
    #endif
    std::clog << "Processing complete" << std::endl;
    return 0;
}

int main2(int argc, char *argv[]) {
  xlnt::workbook wb;
  char obuf[sizeof(dcpref_t) + DCCUP_MAX + 1024];
  dcpref_t *pref = (dcpref_t *)obuf;
  char *vbuf = &obuf[sizeof(dcpref_t)];
  char *ebuf = vbuf + DCCUP_MAX;
  int tlen, nsheet, sind, rmin, rmax, rind, i, k;
  char cellnm[64];
  dclcab_t lcab;
  dclnid_t nidcab;
  uint64_t cab64, ref64;
  uint32_t cab32;
  int mapop;
  uint32_t cla16;

  memset((void *)pref, 0, sizeof(dcpref_t));
  char *es = getenv("DC_NIDSLOT");
  if (es) sscanf(es, "%d", &g_libslot);
  g_port = DCPORT_CUP + g_libslot * DCPORT_ADJ;
      wb.load(argv[1]);
  fprintf(stderr, "hi there\n");

}
