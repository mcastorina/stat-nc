#include <stdint.h>
#include "nc_window.h"

#ifndef __NCD_STRING_H
#define __NCD_STRING_H


#define NCD_STRING_T    (0x01)  // Transparent type
                                // This option doesn't draw spaces

typedef struct ncd_string {
    nc_data *parent;            /* Parent pointer */
    uint8_t flags;              /* Flags */
} ncd_string;

/*=Initializes ncd_string object============================================*/
/* ncd_string *p            ncd_string pointer                              */
/* nc_data *d               nc_data pointer                                 */
/* uint8_t                  Flags as described above                        */
/*                                                                          */
/* Initializes ncd_string object and installs the draw                      */
/* pointer and the child pointer in its nc_data parent.                     */
/*==========================================================================*/
int ncd_string_init(ncd_string *p, nc_data *d, uint8_t flags);

#endif
