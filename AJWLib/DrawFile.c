/*
	AJWLib - DrawFile
	© Alex Waugh 2002

	$Id: DrawFile.c,v 1.1 2002-12-07 13:38:33 ajw Exp $

*/

#include "Desk/SWI.h"
#include "Desk/Error2.h"

#include "AJWLib/Drawfile.h"

extern Desk_os_error *XDrawFile_Render (int flags,
      drawfile_diagram *diagram,
      int size,
      os_trfm *trfm,
      Desk_wimp_box *clip,
      int flatness)
{
	return Desk_SWI(6,0,0x45540,flags,diagram,size,trfm,clip,flatness);
}

extern void DrawFile_Render (int flags,
      drawfile_diagram *diagram,
      int size,
      os_trfm *trfm,
      Desk_wimp_box *clip,
      int flatness)
{
	Desk_Error2_CheckOS(Desk_SWI(6,0,0x45540,flags,diagram,size,trfm,clip,flatness));
}

