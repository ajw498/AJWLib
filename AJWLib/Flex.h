/****************************************************************************
 * This source file was written by Acorn Computers Limited. It is part of   *
 * the RISCOS library for writing applications in C for RISC OS. It may be  *
 * used freely in the creation of programs for Archimedes. It should be     *
 * used with Acorn's C Compiler Release 3 or later.                         *
 *                                                                          *
 ***************************************************************************/

/*
 * Title  : flex.h
 * Purpose: provide memory allocation for interactive programs requiring
 *          large chunks of store. Such programs must respond to memory
 *          full errors.
 *
 *          Modified by Andy Armstrong to allow the use of Dynamic Areas
 *          when used on a Risc PC. Also allows a callback function to be
 *          registered. This notifies the application when a flex block moves.
 *
 *          Further modified by David Jackson to provide an interface with
 *          the Virtualise module.
 *
 *          Modified again to work with Acorn Toolbox (Ugh!)
 */

#ifndef __flex_h
#define __flex_h

#ifndef __dl_core_h
#include "Desk.Core.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
typedef void **flex_ptr;

#include "kernel.h"

#define flex_CALLBACK

/* ----------------------------- flex_alloc -------------------------------
 * Description:   Allocates n bytes of store, obtained from the Wimp.
 *
 * Parameters:    flex_ptr anchor -- to be used to access allocated store
 *                int n -- number of bytes to be allocated
 * Returns:       0 == failure, 1 == success
 * Other Info:    You should pass the & of a pointer variable as the first
 *                parameter. The allocated store MUST then be accessed
 *                indirectly, through this, ie. (*anchor)[0] ..(*anchor)[n]
 *                This is important since  the allocated store may later be
 *                moved (it's a shifting heap!!). If there's not enough
 *                store returns zero leaving anchor unchanged.
 *
 */

void AJWLib_Flex_Alloc(flex_ptr anchor, int n);


/* ------------------------------ flex_free -------------------------------
 * Description:   Frees the previously allocated store.
 *
 * Parameters:    flex_ptr anchor -- pointer to allocated store
 * Returns:       void.
 * Other Info:    *anchor will be set to 0.
 *
 */

void AJWLib_Flex_Free(flex_ptr anchor);


/* ------------------------------- flex_size ------------------------------
 * Description:   Informs caller of the number of bytes allocated
 *
 * Parameters:    flex_ptr -- pointer to allocated store
 * Returns:       number of allocated bytes.
 * Other Info:    None.
 *
 */

int AJWLib_Flex_Size(flex_ptr);


/* --------------------------- flex_extend --------------------------------
 * Description:   Extend ot truncate the store area to have a new size.
 *
 * Parameters:    flex_ptr -- pointer to allocated store
 *                int newsize -- new size of store
 * Returns:       0 == failure, 1 == success.
 * Other Info:    None.
 *
 */

void AJWLib_Flex_Extend(flex_ptr, int newsize);


/* --------------------------- flex_midextend -----------------------------
 * Description:   Extend or truncate store, at any point.
 *
 * Parameters:    flex_ptr -- pointer to allocated store
 *                int at -- location within the allocated store
 *                int by -- extent
 * Returns:       0 == failure, 1 == success
 * Other Info:    If by is +ve, then store is extended, and locations above
 *                "at" are copied up by "by".
 *                If by is -ve, then store is reduced, and any bytes beyond
 *                "at" are copied down to "at"+"by".
 *
 */

void AJWLib_Flex_MidExtend(flex_ptr, int at, int by);

#ifdef flex_CALLBACK

/* AA: 30.9.94 */

/* --------------------------- flex_cbfunc --------------------------------
 * A function of this type may be registered for each flex block so that
 * the owner of the block can be notified when the block moves
 */

typedef void (*flex_cbfunc)(Desk_bool b4, void *handle);

/* --------------------------- flex_register -------------------------------
 * Register the function to be called when a block of moves
 */

void AJWLib_Flex_Register(flex_ptr anchor, flex_cbfunc cb, void *handle);

#endif


/* ---------------------------- flex_budge --------------------------------
 * Description:    Function to move flex store, when the C library needs
 *                 to extend the heap.
 *
 * Parameters:     int n -- number of bytes needed by C library
 *                 void **a -- address of acquired store.
 * Returns:        amount of store acquired.
 * Other Info:     Don't call this function directly, but register it
 *                 with the C library via:
 *                    _kernel_register_slotextend(flex_budge);
 *                 This will cause flex store to be moved up if the C
 *                 library needs to extend the heap.  Note that in this
 *                 state, you can only rely on pointers into flex blocks
 *                 across function calls which do not extend the stack and
 *                 do not call malloc.
 *                 The default state is flex_dont_budge, so, if required,
 *                 this function should be registered AFTER calling
 *                 flex_init().
 *
 */

extern int AJWLib_Flex_Budge(int n, void **a);


/* -------------------------- flex_dont_budge -----------------------------
 * Description:   Function to refuse to move flex store, when the C library
 *                needs to extend the heap.
 *
 * Parameters:    int n -- number of bytes needed by C library
 *                void **a -- address of acquired store.
 * Returns:       amount of store acquired (always 0).
 * Other Info:    Don't call this function directly, but register it
 *                with the C library via:
 *                   _kernel_register_slotextend(flex_dont_budge);
 *                If the C library needs to extend the heap, flex will
 *                refuse to move. This means that you can rely on pointers
 *                into flex blocks across function calls.
 *                This is the DEFAULT state after flex_init().
 *
 */

extern int AJWLib_Flex_DontBudge(int n, void **a);



/* ---------------------------- flex_init ---------------------------------
 * Description:   Initialise store allocation module.
 *
 * Parameters:    char *program_name - name of program
 *                int  *error_fd     - messages file fd.
 * Returns:       void.
 * Other Info:    Must be called before any other functions in this module.
 *                program_name must point at a character string
 *                whose lifetime is the entire program (eg a string
 *                literal, or static buffer).
 *                error_fd is a pointer to a file descriptor as returned
 *                by MessageTrans_OpenFile - it will be used to report flex
 *                errors.  If it is 0, then English default messages are used.
 *
 */

void AJWLib_Flex_Init(void);



/* ---------------------------- flex_initx ---------------------------------
 * Description:   Initialise store allocation module.
 *
 * Parameters:    char *program_name - name of program
 *                int  *error_fd     - messages file fd.
 *                BOOL da      -- TRUE=use dynamic areas if available.
 *                int  maxsize -- Maximum size of dynamic area.
 *                BOOL virtual -- TRUE means use virtual memory.
 *                                If called with virtual = FALSE, virtual
 *                                memory can be switched on with
 *                                flex_virtualstart
 * Returns:       void.
 * Other Info:    Must be called before any other functions in this module.
 *                May be replaced by flex_init
 */


void AJWLib_Flex_InitX(char *program_name, int *error_fd,Desk_bool da,int maxsize,Desk_bool virtualm);

/*
 * Flex_Init with dynamic areas, using message tags supplied
 *
 */

void AJWLib_Flex_InitDA(char *name,char *maxsize);

/* ---------------------------- flex_isdynamic ---------------------------------
 * Description:   Enquire whether dynamic area is in use.
 *
 * Parameters:    void.
 * Returns:       TRUE if a dynamic area is in use.
 * Other Info:    None
 */

Desk_bool AJWLib_Flex_IsDynamic(void);

/* ---------------------------- flex_VM ---------------------------------
 * Description:   Enquire whether virtual memory area is in use.
 *
 * Parameters:    void.
 * Returns:       TRUE if virtual memory is active.
 * Other Info:    None
 */


Desk_bool AJWLib_Flex_IsVM(void);


/* For full details of the following functions. Please read the documentation
   provided with the virtualise module.
*/





/* ---------------------------- flex_VMConfigure ---------------------------------
 * Description:   Configure Virtulise module.
 *
 * Parameters:    int def   --  size of dynamic area created if passed -1 as size.
 *                              Applications don't normally alter this.
                  int cache --  use this much physical RAM before using VM
                  int left  --  but leave at least this much free for other applications.
 * Returns:       NULL if successful, otherwise an error pointer
 * Other Info:    None
 */

Desk_os_error * AJWLib_Flex_VMConfigure(int def,int cache,int left);



/* ---------------------------- flex_readVMConfigure ---------------------------------
 * Description:   Read Virtulise module configuration.
 *
 * Parameters:    int *def   --  on exit contains size of dynamic area created if passed -1 as size.
 *                               Applications don't normally alter this.
                  int *cache --  on exit contains size of physical RAM to use before using VM
                  int *left  --  on exit contains size to leave at free for other applications.
 * Returns:       NULL if successful, otherwise an error pointer
 * Other Info:    None
 */

Desk_os_error * AJWLib_Flex_ReadVMConfigure(int *def ,int *cache,int *left);


/* ---------------------------- flex_virtualstart ---------------------------------
 * Description:   start virtual memory on flex's dynamic area.
 *
 * Parameters:    char *swapfile   -- pointer to filename to use as swapfile. If NULL,
 *                                    Virtualise module will choose its own name (in the
 *                                    scrap directory. Applications will not normally
 *                                    alter this.
 * Returns:       NULL if successful, otherwise an error pointer
 * Other Info:    None
 */

Desk_os_error * AJWLib_Flex_VirtualStart(char * swapfile);

/* ---------------------------- flex_virtualstop ---------------------------------
 * Description:   stop virtual memory on flex's dynamic area.
 *
 * Parameters:    void
 * Returns:       NULL if successful, otherwise an error pointer
 * Other Info:    None
 */

Desk_os_error * AJWLib_Flex_VirtualStop(void);


/* ---------------------------- flex_lock ---------------------------------
 * Description:   lock pages in phyical ram.
 *
 * Parameters:    int start -- start address to lock
 *                int end   -- end   address to lock
 * Returns:       NULL if successful, otherwise an error pointer
 * Other Info:    None
 */

Desk_os_error * AJWLib_Flex_Lock(int start,int end);


/* ---------------------------- flex_unlock ---------------------------------
 * Description:   unlock pages in phyical ram.
 *
 * Parameters:    int start -- start address to unlock
 *                int end   -- end   address to unlock
 * Returns:       NULL if successful, otherwise an error pointer
 * Other Info:    None
 */

Desk_os_error * AJWLib_Flex_Unlock(int start,int end);



#ifdef __cplusplus
}
#endif
#endif

/* end flex.h */

#ifdef MemCheck_MEMCHECK
#include "AJWLib.MemCheckFlex.h"
#endif


