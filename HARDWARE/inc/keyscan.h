#ifndef __key_scan_H
#define __key_scan_H

#include "includes.h"

extern unsigned char key_num_buf;

extern bool KEY_CHKPWR_READ(void);
extern bool KEY_SOS_READ(void);
extern bool KEY_CALL1_READ(void);

extern void key_scan(void);

#endif
