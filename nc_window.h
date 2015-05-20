#include <unistd.h>
#include <stdint.h>
#include <ncurses.h>
#include "nc_command.h"

#ifndef __NC_WINDOW_H
#define __NC_WINDOW_H


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

/* Flags for nc_window */
#define NCW_BORDER_MASK (0x300)     // Border mask
#define NCW_BORDER_N    (0x000)     // No window border
#define NCW_BORDER_THN  (0x100)     // Thin window border
#define NCW_BORDER_THK  (0x200)     // Thick window border
#define NCW_BORDER_CST  (0x300)     // Custom window border

#define NCD_MAX_SIZE    (4096)
#define NCW_MAX_ARR_LEN (32)

#define BORDER_N(p)         ((p->flags & NCW_BORDER_MASK) == NCW_BORDER_N)
#define BORDER_THN(p)       ((p->flags & NCW_BORDER_MASK) == NCW_BORDER_THN)
#define BORDER_THK(p)       ((p->flags & NCW_BORDER_MASK) == NCW_BORDER_THK)
#define BORDER_CST(p)       ((p->flags & NCW_BORDER_MASK) == NCW_BORDER_CST)

#define FIXED_SIZE_Y(p)     (p->flags & NC_FIXS_Y)
#define FIXED_SIZE_X(p)     (p->flags & NC_FIXS_X)
#define POS_MASK_Y          (0x07)
#define JUST_Y(p)           (p->flags & 0x04)
#define FIXED_POS_Y(p)      ((p->flags & POS_MASK_Y) == NC_FIXS_Y)
#define TOP_JUST(p)         ((p->flags & POS_MASK_Y) == NC_TOP)
#define CENTER_JUST_Y(p)    ((p->flags & POS_MASK_Y) == NC_CENTER_Y)
#define BOTTOM_JUST(p)      ((p->flags & POS_MASK_Y) == NC_BOTTOM)
#define POS_MASK_X          (0x70)
#define JUST_X(p)           (p->flags & 0x40)
#define FIXED_POS_X(p)      ((p->flags & POS_MASK_Y) == NC_FIXS_X)
#define LEFT_JUST(p)        ((p->flags & POS_MASK_X) == NC_LEFT)
#define CENTER_JUST_X(p)    ((p->flags & POS_MASK_X) == NC_CENTER_X)
#define RIGHT_JUST(p)       ((p->flags & POS_MASK_X) == NC_RIGHT)


#define GET_SIZE_Y(p, py)   (FIXED_SIZE_Y(p) ? p->size_y : py*p->size_y/100)
#define GET_SIZE_X(p, px)   (FIXED_SIZE_X(p) ? p->size_x : px*p->size_x/100)
#define GET_POS_Y(p, py) \
(FIXED_POS_Y(p) ? p->pos_y : \
JUST_Y(p) ? \
    (BOTTOM_JUST(p) ? py - (int)GET_SIZE_Y(p, py) + p->pos_y : \
    (CENTER_JUST_Y(p) ? (py - (int)GET_SIZE_Y(p, py))/2 + p->pos_y : \
    p->pos_y)) : \
py*p->pos_y/100)

#define GET_POS_X(p, px) \
(FIXED_POS_X(p) ? p->pos_x : \
JUST_X(p) ? \
    (RIGHT_JUST(p) ? px - (int)GET_SIZE_X(p, px) + p->pos_x : \
    (CENTER_JUST_X(p) ? (px - (int)GET_SIZE_X(p, px))/2 + p->pos_x : \
    p->pos_x)) : \
px*p->pos_x/100)


extern uint32_t NC_WIN_Y, NC_WIN_X;         /* Dimensions of stdscr */
extern uint32_t NC_WIN_RES;                 /* Set in nc_update() if
                                               stdscr changed size */
                                            /* Must be cleared manually */

typedef struct nc_data {
    nc_command *cmd;            /* Pointer to nc_command object */
    int pos_y, pos_x;           /* Position of data */
                                    /* Presedence: fixed, justified, percent */
                                    /* Set by flags */
    uint32_t size_y, size_x;    /* Size of data */
    int apos_y, apos_x;         /* Actual position of data */
                                    /* Calcualted when window is resized */
    uint32_t asize_y, asize_x;  /* Actual size of data */
    void (*draw)(struct nc_data *self, WINDOW *win, char *buf);
                                /* Pointer to draw function with self pointer,
                                   WINDOW pointer, and char buffer */
    uint8_t flags;              /* Flags as described above */
    uint8_t child_flags;        /* Flags reserved for child */
} nc_data;

typedef struct nc_window {
    WINDOW *win;                /* Ncurses window */
    int pos_y, pos_x;           /* Position of window */
                                    /* Presedence: fixed, justified, percent */
                                    /* Set by flags */
    uint32_t size_y, size_x;    /* Size of window */
    int apos_y, apos_x;         /* Actual position of data */
                                    /* Calcualted when window is resized */
    uint32_t asize_y, asize_x;  /* Actual size of data */
    nc_data **data;             /* Array of nc_data objects */
    size_t data_size;           /* Length of data array */
    uint16_t flags;             /* Flags as described above */
} nc_window;


/*=Initializes nc_window object=============================================*/
/* nc_window *p             nc_window pointer                               */
/* int pos_y                Y position of window                            */
/* int pos_x                X position of window                            */
/*                              If position is a justification, then these  */
/*                              values are used as offsets                  */
/* uint32_t size_y          Y size of window (fixed / perc)                 */
/* uint32_t size_x          X size of window (fixed / perc)                 */
/* uint16_t flags           Flags as described above                        */
/*                                                                          */
/* Initializes a nc_window object. Given these values, it will also         */
/* calculate the actual size and position values by calling                 */
/* ncw_resize(p). Returns 0 on success and -1 on error.                     */
/*==========================================================================*/
int ncw_init(nc_window *p,
        int pos_y, int pos_x,
        uint32_t size_y, uint32_t size_x,
        uint16_t flags);

/*=Updates every interval===================================================*/
/* Updates the NC_WIN_Y and NC_WIN_X. If they changed, it will set          */
/* NC_WIN_RES to indicate a window resize and return 1. Otherwise it will   */
/* return 0.                                                                */
/*==========================================================================*/
int ncw_update(void);

/*=Resizes nc_window object=================================================*/
/* nc_window *p             nc_window pointer                               */
/*                                                                          */
/* Calculates and stores the actual position and size for the window        */
/* and each nc_data object in its array. Returns 0.                         */
/*==========================================================================*/
int ncw_resize(nc_window *p);

/*=Adds data object to nc_window============================================*/
/* nc_window *p             nc_window pointer                               */
/* nc_data *d               nc_data pointer                                 */
/*                                                                          */
/* Adds an nc_data object to nc_window's data array. Returns 0 on success   */
/* and -1 on failure.                                                       */
/*==========================================================================*/
int ncw_add_data(nc_window *p, nc_data *d);

/*=Draws window=============================================================*/
/* nc_window *p             nc_window pointer                               */
/*                                                                          */
/* Calls ncd_draw for each data object. It then draws its border            */
/* last. Returns 0.                                                         */
/*==========================================================================*/
int ncw_draw(nc_window *p);

/*=Initializes nc_data object===============================================*/
/* nc_data *p               nc_data pointer                                 */
/* const char *cmd          Command to run                                  */
/* uint32_t period          Period to run the command in milliseconds       */
/* int pos_y                Y position of window                            */
/* int pos_x                X position of window                            */
/*                              If position is a justification, then these  */
/*                              values are used as offsets                  */
/* uint32_t size_y          Y size of window (fixed / perc)                 */
/* uint32_t size_x          X size of window (fixed / perc)                 */
/* uint8_t flags            Flags as described above                        */
/*                                                                          */
/* Initializes a nc_data object using a nc_command object. Returns 0 on     */
/* success and -1 on failure.                                               */
/*==========================================================================*/
int ncd_init(nc_data *p,
        const char *cmd, uint32_t period,
        int pos_y, int pos_x,
        uint32_t size_y, uint32_t size_x,
        uint8_t flags);

/*=Draws data===============================================================*/
/* nc_data *p               nc_data pointer                                 */
/* WINDOW *win              Window to draw to                               */
/*                                                                          */
/* Sets up the data to draw by copying the nc_command buffer to a local     */
/* buffer to pass to the draw function. It then calls the draw function.    */
/* Returns 0.                                                               */
/*==========================================================================*/
int ncd_draw(nc_data *p, WINDOW *win);

#endif
