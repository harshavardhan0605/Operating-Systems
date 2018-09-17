/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>
bs_map_t bsm_tab[TOTAL_STORES];



/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
  STATWORD ps;
  disable(ps);
  if(bsm_tab[source].bs_status == BSM_UNMAPPED)
  {
    //kprintf("In 1\n");
    restore(ps);
    return SYSERR;
  }
  if(bsm_tab[source].bs_max_npages< npages)
  {
   restore(ps);
    return SYSERR;
  }

  if(bsm_tab[source].bs_max_npages > npages || bsm_tab[source].bs_max_npages == npages)
  {
    proctab[currpid].store[source] = VIRT_SHARE;
    bsm_map(currpid,virtpage,source,npages);
    restore(ps);
    return OK;
  }
  bsm_map(currpid,virtpage,source,npages);
  restore(ps);
  return OK;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
  STATWORD ps;
  disable(ps);
  bsm_unmap(currpid,virtpage,1);
  restore(ps);
  return OK;
}
