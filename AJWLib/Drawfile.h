#ifndef drawfile_H
#define drawfile_H

#include "Desk.Wimp.h"

/* C header file for DrawFile
 * written by DefMod (Oct 24 1994) on Tue Jan 17 15:20:45 1995
 * Copyright © Acorn Computers Ltd, 1994
 */


/************************************
 * Structure and union declarations *
 ************************************/
typedef struct drawfile_object                  drawfile_object;
typedef struct drawfile_diagram                 drawfile_diagram;
typedef struct os_trfm                          os_trfm;

/********************
 * Type definitions *
 ********************/
struct os_trfm
   {  int entries [3] [2];
   };

struct drawfile_object
   {  int type;
      int size;
      char data[1];
   };

struct drawfile_diagram
   {  char tag[4];
      int major_version;
      int minor_version;
      char source[12];
      Desk_wimp_box bbox;
      drawfile_object objects[1];
   };


/*************************
 * Function declarations *
 *************************/

#ifdef __cplusplus
   extern "C" {
#endif

/* ------------------------------------------------------------------------
 * Function:      drawfile_render()
 *
 * Description:   Calls SWI 0x45540
 *
 * Input:         flags - value of R0 on entry
 *                diagram - value of R1 on entry
 *                size - value of R2 on entry
 *                trfm - value of R3 on entry
 *                clip - value of R4 on entry
 *                flatness - value of R5 on entry
 */

extern Desk_os_error *XDrawFile_Render (int flags,
      drawfile_diagram *diagram,
      int size,
      os_trfm *trfm,
      Desk_wimp_box *clip,
      int flatness);

extern void DrawFile_Render (int flags,
      drawfile_diagram *diagram,
      int size,
      os_trfm *trfm,
      Desk_wimp_box *clip,
      int flatness);


#ifdef __cplusplus
   }
#endif

#endif
