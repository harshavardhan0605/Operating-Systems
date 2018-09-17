#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {
  STATWORD ps;
  disable(ps);

  /* release the backing store with ID bs_id */
    ////kprintf("release _bs  To be implemented!\n");
    if(bsm_tab[bs_id].bs_shared== BSM_SHARED)
    {
      int k=0,j,i;
      for(i=0;i<NPROC;i++)
      {
        if(bsm_tab[bs_id].bs_pid[i] !=-1)
        {
            k++;
        }
      }
      if(k==0)
      {
        free_bsm(bs_id);
      }
      else{
        restore(ps);
        return SYSERR;
      }

    }
    if(bsm_tab[bs_id].bs_shared== BSM_PRIVATE)
    {
      free_bsm(bs_id);
    }


  restore(ps);
   return OK;

}

