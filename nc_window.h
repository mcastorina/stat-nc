#include <unistd.h>
#include <stdint.h>
#include <ncurses.h>

#ifndef __WINDOW_H
#define __WINDOW_H

/* Flags */

/* Flags for both nc_data and nc_window */
#define NC_FIXP_Y       (0x1)       // Fixed Y position
#define NC_CENTER_Y     (0x4)       // Center justified on Y
#define NC_TOP          (0x5)       // Top justified
#define NC_BOTTOM       (0x6)       // Bottom justified
#define NC_FIXS_Y       (0x8)       // Fixed Y size
#define NC_FIXP_X       (0x10)      // Fixed X position
#define NC_CENTER_X     (0x40)      // Center justified on X
#define NC_LEFT         (0x50)      // Left justified
#define NC_RIGHT        (0x60)      // Right justified
#define NC_FIXS_X       (0x80)      // Fixed X size

        // Position bit definitions:
        //  000    percentage
        //  001    fixed
        //  100    justified center
        //  101    justified left / top
        //  110    justified right / bottom

/* Flags for nc_data */
#define NCD_STRING      (0x100)     // String type
#define NCD_TSTRING     (0x200)     // Transparent string type
#define NCD_BAR         (0x300)     // Bar type
#define NCD_BV_Y        (0x400)     // Bar vector Y
#define NCD_BV_X        (0x800)     // Bar vector X
                                        // Vectors mean the following:
                                             // 0: grow increasingly
                                             // 1: grow decreasingly
#define NCD_BB_Y        (0x1000)    // Bracket around bar in Y
#define NCD_BB_X        (0x2000)    // Bracket around bar in X

/* Flags for nc_window */
#define NCW_BORDER_N    (0x000)     // No window border
#define NCW_BORDER_THN  (0x100)     // Thin window border
#define NCW_BORDER_THK  (0x200)     // Thick window border
#define NCW_BORDER_DSH  (0x300)     // Dashed window border

#define NC_MAX_DATA_SIZE    (1024)
#define NC_MAX_ARR_LEN      (32)


#define FIXED_POS_Y(p)      ((p->flags & 0x7) == NC_FIXP_Y)
#define CENTER_JUST_Y(p)    ((p->flags & 0x7) == NC_CENTER_Y)
#define TOP_JUST(p)         ((p->flags & 0x7) == NC_TOP)
#define BOTTOM_JUST(p)      ((p->flags & 0x7) == NC_BOTTOM)
#define FIXED_SIZE_Y(p)     (p->flags & NC_FIXS_Y)
#define FIXED_POS_X(p)      ((p->flags & 0x70) == NC_FIXP_X)
#define CENTER_JUST_X(p)    ((p->flags & 0x70) == NC_CENTER_X)
#define LEFT_JUST(p)        ((p->flags & 0x70) == NC_LEFT)
#define RIGHT_JUST(p)       ((p->flags & 0x70) == NC_RIGHT)
#define FIXED_SIZE_X(p)     (p->flags & NC_FIXS_X)

#define JUST_Y(p)           (p->flags & 0x4)
#define JUST_X(p)           (p->flags & 0x40)

#define STRING(p)           (p->flags & NCD_STRING)
#define TSTRING(p)          (p->flags & NCD_TSTRING)
#define BAR(p)              (p->flags & NCD_BAR)
#define BV_Y(p)             (p->flags & NCD_BV_Y)
#define BV_X(p)             (p->flags & NCD_BV_X)
#define BB_Y(p)             (p->flags & NCD_BB_Y)
#define BB_X(p)             (p->flags & NCD_BB_X)

#define BORDER_N(p)         ((p->flags & 0x300) == NCW_BORDER_N)
#define BORDER_THN(p)       ((p->flags & 0x300) == NCW_BORDER_THN)
#define BORDER_THK(p)       ((p->flags & 0x300) == NCW_BORDER_THK)
#define BORDER_DSH(p)       ((p->flags & 0x300) == NCW_BORDER_DSH)

// TODO: fix getting size
// three options:
//  add actual_size to struct
//      don't have to recursively calculate size every time you draw an object
//      more memory
//  write a recursive function
//      for less depth wouldn't be bad
//  only allow a depth of two
//      simpler
//      easier
//      who needs more than 2?

#define HAS_BORDER(p)       (p ? !BORDER_N(p) : 0)
#define NC_PARENT_Y(p)      (p->parent == NULL ? NC_WIN_Y : \
                                (FIXED_SIZE_Y(p->parent) ? p->parent->size_y : \
                                 NC_WIN_Y*p->parent->size_y/100))
#define NC_PARENT_X(p)      (p->parent == NULL ? NC_WIN_X : \
                                (FIXED_SIZE_X(p->parent) ? p->parent->size_x : \
                                 NC_WIN_X*p->parent->size_x/100))

#define GET_SIZE_Y(p)       (FIXED_SIZE_Y(p) ? p->size_y : NC_PARENT_Y(p)*p->size_y/100)
#define GET_SIZE_X(p)       (FIXED_SIZE_X(p) ? p->size_x : NC_PARENT_X(p)*p->size_x/100)
#define GET_POS_Y(p)        (FIXED_POS_Y(p) ? p->pos_y : \
                                JUST_Y(p) ? \
                                    (BOTTOM_JUST(p) ? \
                                         NC_PARENT_Y(p) - GET_SIZE_Y(p) - HAS_BORDER(p->parent) : \
                                    CENTER_JUST_Y(p) ? \
                                         (NC_PARENT_Y(p) - GET_SIZE_Y(p))/2 : \
                                    HAS_BORDER(p->parent)) : \
                                NC_PARENT_Y(p)*p->pos_y/100)
#define GET_POS_X(p)        (FIXED_POS_X(p) ? p->pos_x : \
                                JUST_X(p) ? \
                                    (RIGHT_JUST(p) ? \
                                         NC_PARENT_X(p) - GET_SIZE_X(p) - HAS_BORDER(p->parent) : \
                                    CENTER_JUST_X(p) ? \
                                         (NC_PARENT_X(p) - GET_SIZE_X(p))/2 : \
                                    HAS_BORDER(p->parent)) : \
                                NC_PARENT_X(p)*p->pos_x/100)


extern uint32_t NC_WIN_Y, NC_WIN_X;         /* Dimensions of stdscr */
extern uint32_t NC_WIN_RES;                 /* Set in nc_update() if stdscr changed size */
                                            /* Must be cleared manually */

typedef struct nc_data {
    void *data;                 /* Pointer to data */
    size_t size;                /* Size of data */
    uint32_t pos_y, pos_x;      /* Position of data */
                                    /* Presedence: fixed, justified, percent */
                                    /* Set by flags */
    uint32_t size_y, size_x;    /* Size of data */
    uint32_t flags;             /* Flags as described above */
    struct nc_window *parent;   /* Parent nc_window */
} nc_data;

typedef struct nc_window {
    WINDOW *win;                /* Ncurses window */
    uint32_t pos_y, pos_x;      /* Position of window */
                                    /* Presedence: fixed, justified, percent */
                                    /* Set by flags */
    uint32_t size_y, size_x;    /* Size of window */
    uint32_t flags;             /* Flags as described above */
    nc_data **data;             /* Array of nc_data objects */
    size_t data_size;           /* Length of data array */
    struct nc_window *parent;   /* Parent nc_window */
} nc_window;


/*=Initializes nc_data object===============================================*/
/*      nc_data *p      Pointer to nc_data object                           */
/*      void *data      Pointer to data to represent                        */
/*      size_t size     Size of data to represent                           */
/*      uint32_t pos_y  Y position of data top left corner in percentage    */
/*      uint32_t pos_x  X position of data top left corner in percentage    */
/*      uint32_t size_y Y size of data in percentage                        */
/*      uint32_t size_x X size of data in percentage                        */
/*      uint32_t flags  Flags as described above                            */
/*      Returns 0 on success                                                */
/*==========================================================================*/
int ncd_init(nc_data *p, nc_window *parent,
             void *data, size_t size,
             uint32_t pos_y, uint32_t pos_x,
             uint32_t size_y, uint32_t size_x,
             uint32_t flags);

/*=Initializes nc_window object=============================================*/
/*      nc_window *p            Pointer to nc_window object                 */
/*      uint32_t pos_y          Y position of window                        */
/*      uint32_t pos_x          X position of window                        */
/*      uint32_t size_y         Y rows of window                            */
/*      uint32_t size_x         X cols of window                            */
/*      uint32_t flags          Flags as described above                    */
/*      Returns 0 on success                                                */
/*==========================================================================*/
int ncw_init(nc_window *p,
             uint32_t pos_y, uint32_t pos_x,
             uint32_t size_y, uint32_t size_x,
             uint32_t flags);


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
/*      uint32_t flags          Flags as described above                    */
/*      Returns 0 on success                                                */
/*==========================================================================*/
int ncw_add_data(nc_window *p,
                 void *data, size_t size,
                 uint32_t pos_y, uint32_t pos_x,
                 uint32_t size_y, uint32_t size_x,
                 uint32_t flags);

#endif
