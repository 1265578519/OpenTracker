/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef _VA_NARG_H
#define _VA_NARG_H

/* Idea from Stefan Reuther, fe5vsq.17c.1@stefan.msgid.phost.de */
#define PP_NARG(...) (sizeof((_Bool[]){__VA_ARGS__})/sizeof(_Bool))

#endif
