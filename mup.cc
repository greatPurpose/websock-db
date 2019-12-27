#include <iostream>
#include <xlnt/xlnt.hpp>
#include <stdio.h>
#include <fcntl.h>
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
int offset=OFFSET_DCOP_MUP;

int g_libslot = 0;
uint16_t g_port;
char *g_serverip = (char *)"tcp://127.0.0.1:50000";
//char *g_routerip = (char *)"tcp://*:50301";
uint64_t g_servernid = 0, g_mynid = 64 *  1024 + OFFSET_DCOP_MUP;
uint32_t g_sheet;


char obuf[64 * 1024 * 1024];
char ibuf[16*1024*1024];

int main(int argc, char *argv[])
{
    fprintf(stderr,"main\n");

    dcpref_t *pref = (dcpref_t *)obuf;
    char *vbuf = &obuf[sizeof(dcpref_t)];
    char *ebuf = vbuf + DCCUP_MAX;
    int tlen, nsheet, sind, rmin, rmax, rind, i, k;
    char cellnm[64];
    dclcab_t lcab, scab;
    dclnid_t nidcab;
    uint64_t cab64, ref64, sect64;
    uint32_t cab32;
    int mapop;
    uint32_t cla16;
    char path[256];

      fprintf(stderr,"before memset\n");

    memset((void *)pref, 0, sizeof(dcpref_t));

    if (argc < 4) {
      fprintf(stderr, "usage: mup  <xls> <imagedir> <sect cab>\n");
      _exit(1);
    }
    dclCabExpand(&scab, argv[3]);
    sect64 = scab.cab64;

  fprintf(stderr,"before nid slot\n");
    char *es = getenv("DC_NIDSLOT");
    if (es) sscanf(es, "%d", &g_libslot);
    g_port = DCPORT_CUP + g_libslot * DCPORT_ADJ;
    fprintf(stderr,"before wb load\n");

  xlnt::workbook wb;
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

	fprintf(stderr,"before dclInit\n");
	fprintf(stderr,"g_serverip=%s g_servernid=%ld g_mynid=%ld g_routerip=%s\n\n",g_serverip,g_servernid,g_mynid, g_routerip);

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

        sprintf(cellnm, "D%d", k);
        cell c = ws.cell((const char *)cellnm);
        string s = dclTrim(c.to_string());
        const char *cstr = (const char *)s.c_str();

        cstr = (const char *)s.c_str();

          dclCabExpand(&lcab, (char *)cstr);
          cab64 = lcab.cab64;
        fprintf(stderr, "%s %s %lx\n", cellnm, cstr, cab64);

        sprintf(cellnm, "E%d", k);
        c = ws.cell((const char *)cellnm);
        s = dclTrim(c.to_string());
        cstr = (const char *)s.c_str();
        cab32 = 0;

        dclCab32Expand(&lcab, (char *)cstr);
        cab32 = lcab.cab32;
        fprintf(stderr, "%s %s %x\n", cellnm, cstr, cab32);

#if 0
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
#endif

        sprintf(cellnm, "Q%d", k);
        c = ws.cell((const char *)cellnm);
        std::cerr << "hi " << c.to_string() << std::endl;
        string s256 = dclTrim(c.to_string());
        const char *cstr256 = (const char *)s256.c_str();
        sprintf(path,"%s/%s", argv[2], cstr256);
        fprintf(stderr, "cstr256 <%s>\n", cstr256);
          fprintf(stderr, "path = %s\n", path);
        int len256 = (int)strlen(cstr256);
        int klen = len256 + sizeof(dckmycdn_t);

        int fd = open(path, O_RDONLY);
        fprintf(stderr, "%s fd == %d\n", path, fd);
        //char ibuf[16*1024*1024];
        // char *ibuf = (char *)malloc(32 * 1024 * 1024);
        size_t sz = 0;
        if (fd > 0) {
          sz = read(fd, &ibuf, 32 * 1024 * 1024 );
          fprintf(stderr, "read sz == %lu\n", sz);
          if (sz <= 30) fprintf(stderr,"\n\n%s\n\n", ibuf);
          close(fd);
        }
        if (sz == 0) {
            fprintf(stderr, "%s not found\n", path);
            _exit(1);
        }
        int vlen = sz;
        if ((tlen + klen + vlen + sizeof(dcslice_t)) > (sizeof(obuf)-1024)) {
          fprintf(stderr, "sheet %d longer than %ld\n", i, sizeof(obuf)-1024);
          _exit(1);
        }
fprintf(stderr, "vlen == %d\n", vlen);
        dcslice_t *slice = (dcslice_t *)cp;
        cp += sizeof(dcslice_t);
        slice->klen = klen;
        slice->vlen = vlen;
        dckmycdn_t *k = (dckmycdn_t *)cp;
        cp += klen;

    fprintf(stderr, "b4  memset vlen == %d\n", vlen);
        memset((void *)k, 0, sizeof(dckmycdn_t));
        strcpy(k->str256, cstr256);
        k->op16 = mapop;
        k->cab64 = cab64;
        k->ref64 = ref64;
        k->cla16 = cla16;
        k->cab32 = cab32;
  fprintf(stderr, "b4  memcpy vlen == %d\n", vlen);
        memcpy((void *)cp, (void *)ibuf, vlen);
fprintf(stderr, "AFTER  memcpy vlen == %d\n", vlen);
        cp += vlen;
        tlen += sizeof(dcslice_t) + klen + vlen;
        fprintf(stderr, "klen vlen tlen %d %d %d\n", klen, vlen, tlen);
      }

      #if 1
          dclop_t *op = dclGetop(dcl);
          memset((void *)pref, 0, sizeof(dcpref_t));
          pref->op = DCOP_MUPD;
          pref->len = tlen;
          pref->stamp = sect64;
          op->p = obuf;
          op->plen = tlen + sizeof(dcpref_t);
	  fprintf(stderr,"dclsendop ::: \n");
          dclSendop(dcl, op);
          if (op->ret) {
            fprintf(stderr, "update sheet %d failed - check and try\n", g_sheet);
            _exit(1);
          }
	  fprintf(stderr,"dclfreeop ::: \n");
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


#ifdef CRAP
nt main_debug(int argc, char *argv[])
{
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
  char path[256];
  fprintf(stderr,"before memset\n");

memset((void *)pref, 0, sizeof(dcpref_t));

if (argc < 3) {
  fprintf(stderr, "usage: mup  <xls> <imagedir>\n");
  _exit(1);
}

fprintf(stderr,"before nid slot\n");
char *es = getenv("DC_NIDSLOT");
if (es) sscanf(es, "%d", &g_libslot);
g_port = DCPORT_CUP + g_libslot * DCPORT_ADJ;
fprintf(stderr,"before wb load\n");

xlnt::workbook wb;
wb.load(argv[1]);
      fprintf(stderr, "hi there\n");

      fprintf(stderr,"before dclInit\n");
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

                  sprintf(cellnm, "D%d", k);
                  cell c = ws.cell((const char *)cellnm);
                  string s = dclTrim(c.to_string());
                  const char *cstr = (const char *)s.c_str();

                  cstr = (const char *)s.c_str();

                    dclCabExpand(&lcab, (char *)cstr);
                    cab64 = lcab.cab64;
                  fprintf(stderr, "%s %s %lx\n", cellnm, cstr, cab64);

                  sprintf(cellnm, "E%d", k);
                  c = ws.cell((const char *)cellnm);
                  s = dclTrim(c.to_string());
                  cstr = (const char *)s.c_str();
                  cab32 = 0;

                  dclCab32Expand(&lcab, (char *)cstr);
                  cab32 = lcab.cab32;
                  fprintf(stderr, "%s %s %x\n", cellnm, cstr, cab32);

                  #if 0
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
                  #endif

                  sprintf(cellnm, "Q%d", k);
                  c = ws.cell((const char *)cellnm);
                  string s256 = dclTrim(c.to_string());
                  const char *cstr256 = (const char *)s256.c_str();
                  sprintf(path,"%s/%s", argv[1], cstr256);
                  fprintf(stderr, "cstr256 %s\n", cstr256);
                  int len256 = (int)strlen(cstr256);
                  int klen = len256 + sizeof(dckmycdn_t);

                  int fd = open(path, O_RDONLY);
                  size_t sz = 0;

                  //char ibuf[1024*1024*4];


                  if (fd > 0) sz = read(fd, ibuf, sizeof(ibuf));
                  if (sz == 0) {
                    fprintf(stderr, "%s not found\n", path);
                    _exit(1);
                  }

                  int vlen = sz;
                  if ((tlen + klen + vlen + sizeof(dcslice_t)) > (sizeof(obuf)-1024)) {
                    fprintf(stderr, "sheet %d longer than %ld\n", i, sizeof(obuf)-1024);
                    _exit(1);
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

                  memcpy((void *)cp, (void *)ibuf, vlen);
                  cp += vlen;
                  tlen += sizeof(dcslice_t) + klen + vlen;
                  fprintf(stderr, "klen vlen tlen %d %d %d\n", klen, vlen, tlen);

                }
//        #ifdef SAM
                #if 0
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
//    #endif /* SAM */
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
              }
#endif //CRAP
