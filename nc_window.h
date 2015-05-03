#include <unistd.h>
#include <stdint.h>
#include <ncurses.h>

#ifndef __WINDOW_H
#define __WINDOW_H

#define NC_CENTER       (0)
#define NC_LEFT         (1)
#define NC_RIGHT        (2)

#define NC_STRING       (3)
#define NC_STRING_T     (4)
#define NC_HORZ         (5)
#define NC_VERT         (6)

#define NC_BORDER_N     (7)
#define NC_BORDER_THIN  (8)
#define NC_BORDER_THICK (9)
#define NC_BORDER_DBL   (10)
#define NC_BORDER_DSH   (11)

#define NC_MAX_DATA_SIZE    (1024)
#define NC_MAX_ARR_LEN      (32)


#define NC_FIXED_POS(p)    (p->fixed & 0x1)
#define NC_FIXED_SIZE(p)   (p->fixed & 0x2)
#define NC_VERT_JUST(p)    (p->fixed & 0x4)
#define NC_HORZ_JUST(p)    (p->fixed & 0x8)


extern uint32_t NC_PARENT_Y, NC_PARENT_X;   /* Dimensions of stdscr */
extern uint32_t NC_WIN_RES;                 /* Set in nc_update() if stdscr changed size */
                                            /* Must be cleared manually */

typedef struct nc_data {
    void *data;                 /* Pointer to data */
    size_t size;                /* Size of data */
    uint32_t pos_y, pos_x;      /* Position of data */
                                    /* Presedence: fixed, justified, percent */
    uint32_t size_y, size_x;    /* Size of data */
    int fixed;                  /* Whether position / size is fixed */
                                    /* Bit 0 is set if position is fixed */
                                    /* Bit 1 is set if size is fixed */
                                    /* Bit 2 is set if vert_justified */
                                    /* Bit 3 is set if horz_justified */
    int vert_type, horz_type;   /* Center, left, or right justified */
                                    /* Will be ignored if negative */
    int type;                   /* Vertical bar, horizontal bar, or string */
} nc_data;

typedef struct nc_window {
    WINDOW *win;                /* Ncurses window */
    int border;                 /* Type of border */
    uint32_t pos_y, pos_x;      /* Position of window */
    uint32_t size_y, size_x;    /* Size of window */
    int fixed;                  /* Whether position / size is fixed */
                                    /* Bit 0 is set if position is fixed */
                                    /* Bit 1 is set if size is fixed */
    nc_data **data;             /* Array of nc_data objects */
    size_t data_size;           /* Length of data array */
} nc_window;


/*=Initializes nc_data object===============================================*/
/*      nc_data *p      Pointer to nc_data object                           */
/*      void *data      Pointer to data to represent                        */
/*      size_t size     Size of data to represent                           */
/*      uint32_t pos_y  Y position of data top left corner in percentage    */
/*      uint32_t pos_x  X position of data top left corner in percentage    */
/*      int fixed_pos   Whether the position is percentage or fixed         */
/*      uint32_t size_y Y size of data in percentage                        */
/*      uint32_t size_x X size of data in percentage                        */
/*      int fixed_size  Whether the size is percentage or fixed             */
/*      int vert_type   Vertical justification of data (center/top/bottom)  */
/*      int horz_type   Horizontal justification of data (center/left/right)*/
/*      int type        Vertical bar, horizontal bar, or string             */
/*      Returns 0 on success                                                */
/*==========================================================================*/
int ncd_init(nc_data *p,
             void *data, size_t size,
             uint32_t pos_y, uint32_t pos_x, int fixed_pos,
             uint32_t size_y, uint32_t size_x, int fixed_size,
             int vert_type, int horz_type,
             int type);

/*=Initializes nc_window object=============================================*/
/*      nc_window *p            Pointer to nc_window object                 */
/*      uint32_t pos_y          Y position of window                        */
/*      uint32_t pos_x          X position of window                        */
/*      int fixed_pos           Whether the position is percentage or fixed */
/*      uint32_t size_y         Y rows of window                            */
/*      uint32_t size_x         X cols of window                            */
/*      int fixed_size          Whether the size is percentage or fixed     */
/*      int border              Border type of window                       */
/*      Returns 0 on success                                                */
/*==========================================================================*/
int ncw_init(nc_window *p,
             uint32_t pos_y, uint32_t pos_x, int fixed_pos,
             uint32_t size_y, uint32_t size_x, int fixed_size,
             int border);


/*=Updates every interval===================================================*/
/*      Returns 0 if nothing was updated                                    */
/*      Returns 1 if something was updated                                  */
/*          Current list of variables that can be updated                   */
/*           NC_WIN_RES     Window resized                                  */
/*==========================================================================*/
int ncw_update(void);

/*=Resizes nc_window object=================================================*/
/*      nc_window *p            Pointer to nc_window object                 */
/*      Returns 0 on success                                                */
/*==========================================================================*/
int ncw_resize(nc_window *p);

/*=Draws window=============================================================*/
/*      nc_window *p            Pointer to nc_window object                 */
/*      Returns 0 on success                                                */
/*==========================================================================*/
int ncw_draw(nc_window *p);

/*=Adds a data object to window=============================================*/
/*      nc_window *p            Pointer to nc_window object                 */
/*      void *data              Pointer to data                             */
/*      size_t size             Size of data                                */
/*      uint32_t pos_y          Y position of data                          */
/*      uint32_t pos_x          X position of data                          */
/*      uint32_t size_y         Y size of data                              */
/*      uint32_t size_x         X size of data                              */
/*      int vert_type           Vertical alignment type                     */
/*      int horz_type           Horizontal alignment type                   */
/*      int type                Type of data (bar / string)                 */
/*      Returns 0 on success                                                */
/*==========================================================================*/
int ncw_add_data(nc_window *p,
                 void *data, size_t size,
                 uint32_t pos_y, uint32_t pos_x, int fixed_pos,
                 uint32_t size_y, uint32_t size_x, int fixed_size,
                 int vert_type, int horz_type,
                 int type);

#endif
