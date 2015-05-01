#include <unistd.h>
#include <stdint.h>
#include <ncurses.h>

#ifndef __WINDOW_H
#define __WINDOW_H

#define NC_CENTER       (0)
#define NC_LEFT         (1)
#define NC_RIGHT        (2)

#define NC_STRING       (3)
#define NC_HORZ         (4)
#define NC_VERT         (5)

#define NC_BORDER_N     (6)
#define NC_BORDER_THIN  (7)
#define NC_BORDER_THICK (8)

#define NC_MAX_DATA_SIZE    (1024)
#define NC_MAX_ARR_LEN      (32)

extern uint32_t NCW_PARENT_Y, NCW_PARENT_X;

typedef struct nc_data {
    void *data;                 /* Pointer to data */
    size_t size;                /* Size of data */
    uint32_t pos_y, pos_x;      /* Y and X position of top left corner in percentage */
    uint32_t size_y, size_x;    /* Y and X size of object in percentage */
    int vert_type, horz_type;   /* Center, left, or right justified */
    int type;                   /* Vertical bar, horizontal bar, or string */
} nc_data;

typedef struct nc_window {
    WINDOW *win;                /* Ncurses window */
    int border;                 /* Type of border */
    uint32_t pos_y, pos_x;      /* Position of window */
    uint32_t size_y, size_x;    /* Size of window */
    nc_data **data;             /* Array of nc_data objects */
    size_t data_size;           /* Length of data array */
} nc_window;


/*=Initializes nc_data object===============================================*/
/*      nc_data *p      Pointer to nc_data object                           */
/*      void *data      Pointer to data to represent                        */
/*      size_t size     Size of data to represent                           */
/*      uint32_t pos_y  Y position of data top left corner in percentage    */
/*      uint32_t pos_x  X position of data top left corner in percentage    */
/*      uint32_t size_y Y size of data in percentage                        */
/*      uint32_t size_x X size of data in percentage                        */
/*      int vert_type   Vertical justification of data (center/left/right)  */
/*      int vert_type   Horizontal justification of data (center/left/right)*/
/*      int type        Vertical bar, horizontal bar, or string             */
/*      Returns 0 on success                                                */
/*==========================================================================*/
int ncd_init(nc_data *p,
             void *data, size_t size,
             uint32_t pos_y, uint32_t pos_x,
             uint32_t size_y, uint32_t size_x,
             int vert_type, int horz_type,
             int type);

/*=Initializes nc_window object=============================================*/
/*      nc_window *p            Pointer to nc_window object                 */
/*      uint32_t size_y         Y size of window in rows                    */
/*      uint32_t size_x         X size of window in columns                 */
/*      int border              Border type of window                       */
/*      nc_data **data          Array of nc_data objects                    */
/*      size_t data_size        Size of data array                          */
/*      Returns 0 on success                                                */
/*==========================================================================*/
int ncw_init(nc_window *p,
             uint32_t pos_y, uint32_t pos_x,
             uint32_t size_y, uint32_t size_x,
             int border,
             nc_data **data, size_t data_size);


/*=Resizes nc_window object=================================================*/
/*      nc_window *p            Pointer to nc_window object                 */
/*      uint32_t new_y          New row count of nc_window                  */
/*      uint32_t new_x          New column count of nc_window               */
/*      Returns 0 on success                                                */
/*==========================================================================*/
int ncw_resize(nc_window *p,
            uint32_t new_y, uint32_t new_x);

/*=Draws window=============================================================*/
/*      nc_window *p            Pointer to nc_window object                 */
/*      Returns 0 on success                                                */
/*==========================================================================*/
int ncw_draw(nc_window *p);

#endif
