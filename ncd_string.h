#include <stdint.h>
#include "nc_window.h"

#ifndef __NCD_STRING_H
#define __NCD_STRING_H


#define NCD_STRING_T    (0x01)  // Transparent type
                                // This option doesn't draw spaces

/*=Initializes ncd_string object============================================*/
/* nc_data *d               nc_data pointer                                 */
/* uint8_t                  Flags as described above                        */
/*                                                                          */
/* Initializes ncd_string object and installs the draw                      */
/* pointer in its nc_data parent.                                           */
/*==========================================================================*/
int ncd_string_init(nc_data *d, uint8_t flags);

#endif
