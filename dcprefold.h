#ifndef DCPREF_H
#define DCPREF_H 1

// various limits and conventions
#define DCCUP_MAX (60*1024)

// mask for response messages
#define DCOP_RESP 0x8000

//  pref op values
#define DCOP_CUP  0x11

// dck op17 values for mycdn
#define DCKOP_LBL   0x01
#define DCKOP_NID   0x02
#define DCKOP_REG   0x03
#define DCKOP_SECT  0x04
#define DCKOP_PLEX  0x05
#define DCKOP_DEV   0x06
#define DCKOP_FCID  0x07
#define DCKOP_PLID  0x08
#define DCKOP_EMETA 0x09
#define DCKOP_STAT  0x0a


typedef struct dcpref {
  uint64_t ser;
  uint64_t stamp;
  uint64_t stamp2;
  uint64_t off;
  uint32_t len;
  uint32_t epoch;
  uint16_t op;
  uint16_t cl1;
  uint16_t cl2;
  char ret;
  char flag;
} dcpref_t;

#define DCCAB_AZ2CAB(az) ( az << 60)
#define DCCAB_REG2CAB(reg) ( reg << 48)
#define DCCAB_SECT2CAB(sect) ( sect << 32)
#define DCCAB_ZN2CAB(zn) ( zn << 24)
#define DCCAB_AREA2CAB(area) ( zn << 16)
#define DCCAB_FLR2CAB(flr) ( flr << 8)
#define DCCAB_DEV2CAB(dev) ( dev)

#define DCCAB_PLEX2CAB(plex) ( plex)
#define DCCAB_NID2CAB(nid) ( nid)


#define DCCAB_CABTOAZ(cab) ( (cab >> 60) & 0xf)
#define DCCAB_CABTOREG(cab) ( (cab >> 48) & 0xfff)
#define DCCAB_CABTOSECT(cab) ( (cab >> 32) & 0xffff)
#define DCCAB_CABTOZN(cab) ( (cab >> 24) & 0xff)
#define DCCAB_CABTOAREA(cab) ( (cab >> 16) & 0xff)
#define DCCAB_CABTOFLR(cab) ( (cab >> 8) & 0xff)
#define DCCAB_CABTODEV(cab) ( (cab) & 0xff)

#define DCCAB_CABTOPLEX(cab) ( (cab) & 0xffff)
#define DCCAB_CABTONID(cab) ( (cab) & 0x7ffff)

typedef struct dckmycdn {
  uint16_t op16;
  uint64_t cab64;
  uint32_t id32;
  uint64_t stamp64;
  uint16_t cla1;
  uint16_t cla2;
  uint32_t ctim32;
  uint32_t mtim32;
  char str256[1];     // variable
} dckmycdn_t;

typedef struct dcvmycdn {
  char str64k[1];   // variable
} dcvmycdn_t;

typedef struct dcslice {
  uint32_t klen;
  uint32_t vlen;
} dcslice_t;

#endif
