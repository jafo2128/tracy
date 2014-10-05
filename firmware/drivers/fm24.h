#ifndef __FM24V_H__
#define __FM24V_H__

#include <inttypes.h>

#define FM24V10_BA      0x50    // base address
#define FM24V10_RSVD    0xF8    // reserved slave address
#define FM24V10_SLEEP   0x86    // sleep command

#define FM24V10_MAX_RETRY   4   // how many times to retry read/write opperations

// FM24V10 has 131072 addressable bytes, so uint16_t is not enough

#ifdef CONFIG_HAVE_FM24V10
#define FM_LA   0x1FFFF         // last addressable address
#endif

#ifdef CONFIG_HAVE_FM24CL64B
#define FM_LA   0x
#endif

// sim900 can only send about 1000 bytes at a time
// so the unsent data has to be segmented up
// MAX_SEG must be at least 3
//#define MAX_SEG_SIZE  1000
#define MAX_SEG_SIZE  300
#define MAX_SEG       9

uint8_t fm24_seek(const uint32_t addr);
uint32_t fm24_read(uint8_t * buf, const uint32_t nbyte);
uint32_t fm24_read_from(uint8_t * buf, const uint32_t addr,
                        const uint32_t nbyte);
uint32_t fm24_write(const uint8_t * buf, const uint32_t addr,
                    const uint32_t nbyte);
uint8_t fm24_sleep(void);

// helpers

struct mem_mgmt_t {
    uint32_t e;             // first empty address
    uint32_t ntx;           // first not yet transmitted address
    uint32_t seg[MAX_SEG];  // addresses for the 1kb segments
    uint8_t seg_num;        // current number of segments
    uint8_t seg_c;          // 
};

struct mem_mgmt_t m;

uint32_t fm24_data_len(const uint32_t first, const uint32_t last);

#endif
