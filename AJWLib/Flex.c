
/************************************************************************/
/*  Acorn Computers Ltd, 1992.                                         */
/*                                                                      */
/* This file forms part of an unsupported source release of RISC_OSLib. */
/*                                                                      */
/* It may be freely used to create executable images for saleable       */
/* products but cannot be sold in source form or as an object library   */
/* without the prior written consent of Acorn Computers Ltd.            */
/*                                                                      */
/* If this file is re-distributed (even if modified) it should retain   */
/* this copyright notice.                                               */
/*                                                                      */
/************************************************************************/

/* Title: c.flex
 * Purpose: provide memory allocation for interactive programs requiring
 *          large chunks of store.
 * History: IDJ: 06-Feb-92: prepared for source release
 *          Andy Armstrong:  21-Oct-94: modified to use dynamic areas and callbacks.
 *          David Jackson :  06-Sep-95: modified to use Virtual Memory
 *          David Jackson :  19-Apr-96: modified to use Acorn Toolbox (Ugh!)
 */

#define TRACE 0

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "kernel.h"
#include "swis.h"
#include "flex.h"

#include "Desk.Msgs.h"
#include "Desk.Error2.h"

#ifndef OS_DynamicArea
#define OS_DynamicArea  0x00066
#endif

#ifndef Virtualise_Configure
#define Virtualise_Configure           0x04b6c0
#endif
#ifndef Virtualise_Start
#define Virtualise_Start               0x04b6c1
#endif
#ifndef Virtualise_End
#define Virtualise_End                 0x04b6c2
#endif
#ifndef Virtualise_Lock
#define Virtualise_Lock                0x04b6c3
#endif
#ifndef Virtualise_Unlock
#define Virtualise_Unlock              0x04b6c4
#endif
#ifndef Virtualise_MiscOp
#define Virtualise_MiscOp              0x04b6c5
#endif



static int flex__initialised = 0;
static int flex__da          = -1; /* or da number */
static Desk_bool flex__VM         = Desk_FALSE;  /* is VM in use? */
static int  _mblk;

/* This implementation goes above the original value of GetEnv,
to memory specifically requested from the Wimp. The heap is kept
totally compacted all the time, with pages being given back to
the Wimp whenever possible. */

typedef struct {
  flex_ptr anchor;      /* *anchor should point back to here. */
#ifdef flex_CALLBACK
  flex_cbfunc cb;
  void *handle;
#endif
  int size;             /* in bytes. Exact size of logical area. */
                        /* then the actual store follows. */
} flex__rec;


static void flex__fail(int i)
{
   Desk_Msgs_ReportFatal(0,"Flex.Flex1:Flex memory error (%d)", i);
#if TRACE
   i = *(int *)-4 ;     /* go bang! */
#else
   i = i; /* avoid compiler warning. */
#endif
}


static void flex__check(void)
{
   if(flex__initialised == 0)
     Desk_Msgs_ReportFatal(0,"Flex.Flex3:Flex not initialised");
}



/* macro to avoid stack usage */
#define roundup(n)  (0xfffffffc & (n + 3))

static char *flex__base;        /* lowest flex__rec - only for budging. */
static char *flex__freep;       /* free flex memory */
static char *flex__lim;         /* limit of flex memory */
/* From base upwards, it's divided into store blocks of
  a flex__rec
  the space
  align up to next word.
*/


static void flex__wimpslot(char **top)
{
   /* read/write the top of available memory. *top == 0 -> just read. */
   int dud = -1;
   int slot = ((int) *top);
   _kernel_swi_regs r;
   _kernel_oserror  *err;
   int memlim, appspace, oldmemlim;

   if (slot != -1) slot -= 0x8000;

   /* read memory limit value */
    r.r[0] = 0;
    r.r[1] = 0;
    r.r[2] = 0;
    err = _kernel_swi(OS_ChangeEnvironment, &r, &r);
    oldmemlim = memlim = r.r[1];

   /* read appspace value */
    r.r[0] = 14;  /* Application space */
    r.r[1] = 0;
    r.r[2] = 0;
    err = _kernel_swi(OS_ChangeEnvironment, &r, &r);
    appspace = r.r[1];
   /* set memory limit before slot size change ... */
    if(appspace > memlim)
    {
       r.r[0] = 0;
       r.r[1] = appspace;
       r.r[2] = 0;
       err = _kernel_swi(OS_ChangeEnvironment, &r, &r);
    }

   /* set wimpslot size (or read it) */
   r.r[0] = slot;         /*Call Wimp_SlotSize - AJW (1/6/98)*/
   r.r[1] = dud;
   if (_kernel_swi(0x400EC,&r,&r)!=NULL) Desk_Msgs_ReportFatal(0,"Flex.Flex3:Fatal Flex memory error");
   slot = r.r[0];

   *top = (char*) slot + 0x8000;

   /* .... and set memory limit back again */
   if (appspace > memlim)
   {
       r.r[0] = 0;
       r.r[1] = oldmemlim;
       r.r[2] = 0;
       err = _kernel_swi(OS_ChangeEnvironment, &r, &r);
    }

}

static Desk_bool flex__more(int n)
{
   /* Tries to get at least n more bytes, raising flex__lim and
   returning Desk_TRUE if it can. */
   char *prev = flex__lim;

   if (flex__da != -1)
   {  _kernel_swi_regs regs;
      _kernel_oserror *err;

      regs.r[0] = flex__da;
      regs.r[1] = n;

      err = _kernel_swi(OS_ChangeDynamicArea, &regs, &regs);

      if (err || regs.r[1] < n)
      {  regs.r[1] = -regs.r[1];
         _kernel_swi(OS_ChangeDynamicArea, &regs, &regs);
         return Desk_FALSE;
      }

      err = _kernel_swi(OS_ReadDynamicArea, &regs, &regs);
      flex__lim = flex__base + regs.r[1];
      return Desk_TRUE;
   }
   else
   {  flex__lim += n;
      flex__wimpslot(&flex__lim);


      if (flex__lim < prev + n)
      {
         flex__lim = prev;             /* restore starting state:
                                          extra memory is useless */
         flex__wimpslot(&flex__lim);
         return Desk_FALSE ;
      }
      else
         return Desk_TRUE ;
   }
}


static void flex__give(void)
{
   if (flex__da != -1)
   {  _kernel_swi_regs regs;
      _kernel_oserror *err;

      regs.r[0] = flex__da;
      regs.r[1] = flex__freep - flex__lim; /* -ve */

      err = _kernel_swi(OS_ChangeDynamicArea, &regs, &regs);
      if (!err)
      {  err = _kernel_swi(OS_ReadDynamicArea, &regs, &regs);
         flex__lim = flex__base + regs.r[1];
      }
   }
   else
   {
      /* Gives away memory, lowering flex__lim, if possible. */
#if TRACE
      int prev = (int) flex__lim;
#endif

      flex__lim = flex__freep;
      flex__wimpslot(&flex__lim);

   }
}


static Desk_bool flex__ensure(int n)
{
   n -= flex__lim - flex__freep;

   if (n <= 0 || flex__more(n))
      return Desk_TRUE;
   else return Desk_FALSE;
}

void AJWLib_Flex_Alloc(flex_ptr anchor, int n)
{
   flex__rec *p;

   flex__check();

   if (n < 0 || ! flex__ensure(sizeof(flex__rec) + roundup(n)))
   {
      *anchor = 0;
      Desk_Error2_HandleText("Error.NoMem:Out of memory");
      exit(EXIT_FAILURE);
   }

   p = (flex__rec*) flex__freep;
   flex__freep += sizeof(flex__rec) + roundup(n);

   p->anchor = anchor;
   p->size = n;
#ifdef flex_CALLBACK
   p->cb = NULL;
   p->handle = NULL;
#endif
   *anchor = p + 1; /* sizeof(flex__rec), that is */
}

static void flex__reanchor(flex__rec *p, int by)
{
   /* Move all the anchors from p upwards. This is in anticipation
   of that block of the heap being shifted. */

   while (1)
   {
      if ((int) p >= (int) flex__freep) break;

      if (*(p->anchor) != p + 1) flex__fail(6);
      *(p->anchor) = ((char*) (p + 1)) + by;
      p = (flex__rec*) (((char*) (p + 1)) + roundup(p->size));
   }
}

/* Notify all the blocks after and including p that the are about to move / have
 moved */

#ifdef flex_CALLBACK
#define flex__inlinenotify(b4, p)                                    \
   do                                                                \
   {  flex__rec *p2 = (p);                                           \
      for (;;)                                                       \
      {  if ((int) p2 >= (int) flex__freep) break;                   \
         if (*(p2->anchor) != p2 + 1) flex__fail(6);                 \
         if (p2->cb) p2->cb(b4, p2->handle);                         \
         p2 = (flex__rec*) (((char*) (p2 + 1)) + roundup(p2->size)); \
      }                                                              \
   } while (0)

static void flex__notify(Desk_bool b4, flex__rec *p)
{  flex__inlinenotify(b4, p);
}

#else
#define flex__inlinenotify(b4, p) (void) 0
#define flex__notify(b4, p) (void) 0
#endif

void AJWLib_Flex_Free(flex_ptr anchor)
{
   flex__rec *p = ((flex__rec*) *anchor) - 1;
   int roundsize = roundup(p->size);
   flex__rec *next = (flex__rec*) (((char*) (p + 1)) + roundsize);

   flex__check();

   if (p->anchor != anchor)
   {
      flex__fail(0);
   }

   flex__notify(Desk_TRUE, next);
   flex__reanchor(next, - (sizeof(flex__rec) + roundsize));
   memmove(p, next, flex__freep - (char*) next);
   flex__freep -= sizeof(flex__rec) + roundsize;
   flex__notify(Desk_FALSE, p);

   flex__give();

   *anchor = 0;
}


int AJWLib_Flex_Size(flex_ptr anchor)
 {
   flex__rec *p = ((flex__rec*) *anchor) - 1;
   flex__check();

   if (p->anchor != anchor)
   {
      flex__fail(4);
   }

   return(p->size);
}


void AJWLib_Flex_Extend(flex_ptr anchor, int newsize)
{
   flex__rec *p = ((flex__rec*) *anchor) - 1;
   flex__check();
   AJWLib_Flex_MidExtend(anchor, p->size, newsize - p->size);
}


void AJWLib_Flex_MidExtend(flex_ptr anchor, int at, int by)
{
   flex__rec *p;
   flex__rec *next;

   flex__check();

   p = ((flex__rec*) *anchor) - 1;

   if (p->anchor != anchor)
      flex__fail(1);

   if (at > p->size)
      flex__fail(2);

   if (by < 0 && (-by) > at)
      flex__fail(3);

   if (by == 0)
   {
      /* do nothing */
   }
   else if (by > 0)
   {
      /* extend */

      int growth = roundup(p->size + by) - roundup(p->size);
      /* Amount by which the block will actually grow. */

      if (! flex__ensure(growth)) {
          Desk_Error2_HandleText("Error.NoMem:Out of memory");
          exit(EXIT_FAILURE);
      }

      next = (flex__rec*) (((char*) (p + 1)) + roundup(p->size));
      /* The move has to happen in two parts because the moving
      of objects above is word-aligned, while the extension within
      the object may not be. */

      flex__notify(Desk_TRUE, next);
      flex__reanchor(next, growth);

      memmove(((char*) next) + roundup(growth), next, flex__freep - (char*) next
);

      flex__freep += growth;
      flex__notify(Desk_FALSE, (flex__rec *) (((char *) next) + roundup(growth)));

      memmove(((char*) (p + 1)) + at + by, ((char*) (p + 1)) + at, p->size - at)
;

      p->size += by;

   }
   else
   {
      /* The block shrinks. */
      int shrinkage;

      next = (flex__rec*) (((char*) (p + 1)) + roundup(p->size));

      by = -by; /* a positive value now */
      shrinkage = roundup(p->size) - roundup(p->size - by);
        /* a positive value */

      memmove(((char*) (p + 1)) + at - by, ((char*) (p + 1)) + at, p->size - at)
;

      p->size -= by;

      flex__notify(Desk_TRUE, next);
      flex__reanchor(next, - shrinkage);

      memmove(((char*) next) - shrinkage, next, flex__freep - (char*) next);

      flex__freep -= shrinkage;
      flex__notify(Desk_FALSE, (flex__rec *) (((char *) next) - shrinkage));

      flex__give();

   }

}

#ifdef flex_CALLBACK
void flex_register(flex_ptr anchor, flex_cbfunc cb, void *handle)
{  flex__rec *p = ((flex__rec*) *anchor) - 1;

   flex__check();

   if (p->anchor != anchor)
   {
      flex__fail(4);
   }

   p->cb = cb;
   p->handle = handle;
}
#endif

/* stack checking off */
#pragma -s1

/* The underlying system asks us to move all flex store up (if n +ve) or
down by n bytes. If you succeed, put the store allocated in *a and return
the size. size >= roundup(n) on successful exit, and will be a multiple of
four. If you fail, return what we can.
If n is -ve, no result is required: success is assumed.
*/

extern int AJWLib_Flex_Budge(int n, void **a)
{  if (flex__da != -1)
      return AJWLib_Flex_DontBudge(n, a);

   flex__inlinenotify(Desk_TRUE, (flex__rec *) flex__base);

   if (n >= 0) /* all moving up */
   {
      int roundupn = roundup(n);
      int more = roundupn - (flex__lim - flex__freep);

      /* try to satisfy the request */
      if (more > 0)   /* ie we have to increase slot */
      {
         char *prev = flex__lim;
         flex__lim += more;

         /* in-line implementation (of flex__wimpslot)  */
         /*  to reduce stack requirements               */
         {
            int slot = ((int) flex__lim);
            _kernel_swi_regs r;
            _kernel_oserror  *err;
            int memlim, appspace, oldmemlim;

            if (slot != -1)
               slot -= 0x8000;

            /* read memory limit value */
            r.r[0] = 0;
            r.r[1] = 0;
            r.r[2] = 0;
            err = _kernel_swi(OS_ChangeEnvironment, &r, &r);
            oldmemlim = memlim = r.r[1];

            /* read appspace value */
            r.r[0] = 14;  /* Application space */
            r.r[1] = 0;
            r.r[2] = 0;
            err = _kernel_swi(OS_ChangeEnvironment, &r, &r);
            appspace = r.r[1];

            /* set memory limit before slot size change ... */
            if(appspace > memlim)
            {
               r.r[0] = 0;
               r.r[1] = appspace;
               r.r[2] = 0;
               err = _kernel_swi(OS_ChangeEnvironment, &r, &r);
            }

            /* set wimpslot size */
            r.r[0] = slot;
            r.r[1] = -1;
            err =_kernel_swi(Wimp_SlotSize, &r, &r);
            slot = r.r[0];

            flex__lim = (char*) slot + 0x8000;

            /* .... and set memory limit back again */
            if (appspace > memlim)
            {
               r.r[0] = 0;
               r.r[1] = oldmemlim;
               r.r[2] = 0;
               err = _kernel_swi(OS_ChangeEnvironment, &r, &r);
            }
         }

         /* if we couldn't satisfy it, still give back what we can, */
         /* _kernel_alloc may be able to use it!!!!!                */
         if (flex__lim < prev + more)
             roundupn = flex__lim - flex__freep; /*all we got*/
      }

      {
           flex__rec *p = (flex__rec*)flex__base;
           while (1) {
                if ((int) p >= (int) flex__freep) break;
                *(p->anchor) = ((char*) (p + 1)) + roundupn;
                p = (flex__rec*) (((char*) (p + 1)) + roundup(p->size));
           }
      }

      memmove(flex__base + roundupn, flex__base, flex__freep - flex__base);
      *a = flex__base;
      flex__base += roundupn;
      flex__freep += roundupn;

      flex__inlinenotify(Desk_FALSE, (flex__rec *) flex__base);
      return(roundupn);

   }
   else
   {
      /* all moving down */
      int roundupn = roundup(-n); /* a +ve value */

      {
         flex__rec *p = (flex__rec*)flex__base;
         while (1)
         {
             if ((int) p >= (int) flex__freep) break;
             *(p->anchor) = ((char*) (p + 1)) + roundupn;
             p = (flex__rec*) (((char*) (p + 1)) + roundup(p->size));
         }
      }
      memmove(flex__base - roundupn, flex__base, flex__freep - flex__base);
      flex__base -= roundupn;
      flex__freep -= roundupn;
      flex__inlinenotify(Desk_FALSE, (flex__rec *) flex__base);
   }


   return(0);
}

extern int AJWLib_Flex_DontBudge(int n, void **a)
{  n = n;
   a = a;
   return 0;
}

/* stack checks on again */
#pragma -s0

static void flex__exit(void)
{  _kernel_swi_regs regs;

   if (flex__da != -1)
   {  regs.r[0] = 1;
      regs.r[1] = flex__da;
      _kernel_swi(OS_DynamicArea, &regs, &regs);
   }
}

void AJWLib_Flex_InitX(char *program_name, int *error_fd,Desk_bool da,int maxsize,Desk_bool virtualise)
{  flex__lim = (char *) -1;
   _mblk=(int)error_fd;

   if (da) /* try to create a DA */
   {  _kernel_oserror *err;
      _kernel_swi_regs regs;

      regs.r[0] =    0;  /* create DA         */
      regs.r[1] =   -1;  /* area number       */
      regs.r[2] =    0;  /* initial size      */
      regs.r[3] =   -1;  /* logical address   */
      regs.r[4] = 0x80;  /* access privileges */
      regs.r[5] =   maxsize;  /* max size          */
      if (virtualise)                                       /*DJ    */
       {                                                    /*DJ    */
         regs.r[4] = regs.r[4]  | (1U <<31);                /*DJ    */
         regs.r[5]=maxsize;                                 /*DJ    */
                                                            /*DJ    */
       }                                                    /*DJ    */
      regs.r[6] =    0;  /* no routine yet    */
      regs.r[7] =    0;  /* ws pointer        */
      regs.r[8] = (int) program_name;  /* area name */

      err = _kernel_swi(OS_DynamicArea, &regs, &regs);

      if (!err)
      {  flex__da   = regs.r[1];
         flex__freep = flex__lim = flex__base = (char *) regs.r[3];
         atexit(flex__exit);
         if (virtualise)
           {
            _kernel_swi(OS_ReadMemMapInfo,&regs,&regs);
            regs.r[0]=regs.r[0]*regs.r[1];
            regs.r[1]=regs.r[0];
            regs.r[2]=(int)(regs.r[0]*.05);
            _kernel_swi(Virtualise_Configure,&regs,&regs);

            flex__VM=Desk_TRUE;
           }
         goto initok;
      }
   }

   flex__wimpslot(&flex__lim);  /* shrink */
   flex__freep = flex__lim;
   flex__base = flex__freep;
   _kernel_register_slotextend(AJWLib_Flex_DontBudge);

initok:
   flex__initialised = 1;

   /* Check that we're in the Wimp environment. */
/*   {
      void *a;
      if (! flex_alloc(&a, 1))
         Msgs_ReportFatal("flex2:Not enough memory for Flex to initialise");

      flex_free(&a);
   }*/

}

/* default flex_init() disables use of DA */

void AJWLib_Flex_Init(void)
{
	AJWLib_Flex_InitX("Unknown",NULL,Desk_FALSE,-1,Desk_FALSE);
}

void AJWLib_Flex_InitDA(char *name,char *maxsize)
{
	char newname[256],newmaxsize[10];
	int size;
	Desk_Msgs_Lookup(name,newname,255);
	Desk_Msgs_Lookup(maxsize,newmaxsize,9);
	size=(int)strtol(newmaxsize,NULL,10);
	size*=1024*1024;
	AJWLib_Flex_InitX(newname,NULL,Desk_TRUE,size,Desk_FALSE);
}

Desk_bool AJWLib_Flex_IsDynamic(void)
{  return (Desk_bool)(flex__da != -1);
}


Desk_bool AJWLib_Flex_IsVM(void)
{
 return flex__VM;
}

Desk_os_error * AJWLib_Flex_VMConfigure(int def,int cache,int left)
{
_kernel_swi_regs regs;

 regs.r[0]=def;
 regs.r[1]=cache;
 regs.r[2]=left;
 return (Desk_os_error *)_kernel_swi(Virtualise_Configure,&regs,&regs);
}



Desk_os_error * AJWLib_Flex_ReadVMConfigure(int * def ,int *cache,int *left)

{
_kernel_oserror *err;
_kernel_swi_regs regs;

 regs.r[0]=-1;
 regs.r[1]=-1;
 regs.r[2]=-1;
 err=_kernel_swi(Virtualise_Configure,&regs,&regs);

 if (!err)
 {

 *def    =regs.r[0];
 *cache  =regs.r[1];
 *left   =regs.r[2];
 regs.r[0]=2;
 regs.r[1]=flex__da;
 _kernel_swi(OS_DynamicArea,&regs,&regs);

*def=regs.r[5];
 }

return((Desk_os_error *)err);
}


Desk_os_error * AJWLib_Flex_VirtualStart(char * swapfile)
{
_kernel_swi_regs regs;
_kernel_oserror *err;

regs.r[0]=flex__da;
regs.r[1]=-1;
regs.r[2]=(int) swapfile;
err=_kernel_swi(Virtualise_Start,&regs,&regs);

if (!err) flex__VM=Desk_TRUE;
return((Desk_os_error *)err);

}


Desk_os_error * AJWLib_Flex_VirtualStop(void)
{
_kernel_swi_regs regs;
_kernel_oserror *err;

regs.r[0]=flex__da;
err=_kernel_swi(Virtualise_End,&regs,&regs);

if (!err) flex__VM=Desk_FALSE;
return((Desk_os_error *)err);

}


Desk_os_error * AJWLib_Flex_Lock(int start,int end)
{
_kernel_swi_regs regs;
_kernel_oserror *err=NULL;

if (flex__VM)

{
   regs.r[0]=start;
   regs.r[1]=end;
   err=_kernel_swi(Virtualise_Lock,&regs,&regs);

}

return((Desk_os_error *)err);
}


Desk_os_error * AJWLib_Flex_Unlock(int start,int end)
{
_kernel_swi_regs regs;
_kernel_oserror *err=NULL;

if (flex__VM)

{
   regs.r[0]=start;
   regs.r[1]=end;
   err=_kernel_swi(Virtualise_Unlock,&regs,&regs);

}

return((Desk_os_error *)err);
}


/* end */

