/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Window.Create.c
    Author:  Copyright © 1992, 1993, 1994 Jason Williams
    Version: 1.01 (30 May 1994)
    Purpose: High-level window management functions: Create a window
*/


#include <stdlib.h>
#include <string.h>

#include "DeskLib:LinkList.h"
#include "DeskLib:WimpSWIs.h"
#include "DeskLib:Template.h"
#include "DeskLib:Event.h"
#include "DeskLib:Window.h"
#include "DeskLib:Screen.h"
#include "DeskLib:Error.h"

#include "<DeskLib_Sources$Dir>.Libraries.Window.h.WindowDefs"


#define ERRBASE 1
#define ERR1 ERRBASE+0
#define ERRMESS1 "Insufficient memory to open window"


extern linklist_header window_listanchor;


extern window_handle Window_CreateUserSprite(char *windowname, int maxtitlesize, sprite_area windowcontrolblock, int *iconcontrolblocks)
{
  windowrec     *record;
  window_block  *windowptr;
  window_handle window;

  windowptr = Template_Clone(windowname, maxtitlesize);
  if (windowptr == NULL)
    return(0);

  /* Added by AJW - 3/9/98*/
  {
  	icon_block *icons;
  	int i;
    windowptr->spritearea=windowcontrolblock;
    if (iconcontrolblocks!=NULL) {
      icons=(icon_block *)((char *)windowptr+sizeof(window_block));
      for (i=0;iconcontrolblocks[i]!=-1;i+=2) {
        if (icons[iconcontrolblocks[i]].flags.data.indirected==FALSE || icons[iconcontrolblocks[i]].flags.data.sprite==FALSE || icons[iconcontrolblocks[i]].flags.data.text==TRUE) {
          Error_ReportInternal(1,"Icons in list for Window_CreateUserSprite are not all indirected sprite only icons");
          return(0);
        }
        icons[iconcontrolblocks[i]].data.indirectsprite.spritearea=(void *)iconcontrolblocks[i+1];
      }
    }
  }
  /* End of added bit*/

  Wimp_CreateWindow(windowptr, &window);

  record = (windowrec *) malloc(sizeof(windowrec));
  if (record == NULL)  Error_ReportFatalInternal(ERR1, ERRMESS1);

  LinkList_AddToHead(&window_listanchor, &(record->header));

  strncpy(record->templatename, windowname, wimp_MAXNAME);
  record->templatename[wimp_MAXNAME] = 0;       /* Record of name for help   */

  record->window = window;                      /* Remember window handle    */
  record->memory = windowptr;                   /* remember to dealloc later */

  return(window);
}

