#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>
bs_map_t bsm_tab[TOTAL_STORES];


int get_bs(bsd_t bs_id, unsigned int npages) {
  /* requests a new mapping of npages with ID map_id */
    if(npages<1 || npages >128)
    {
      return SYSERR;
    }
    STATWORD ps;
    disable(ps);
    if(bsm_tab[bs_id].bs_status == BSM_UNMAPPED)
    {
        bsm_tab[bs_id].bs_status = BSM_MAPPED;
        bsm_tab[bs_id].bs_shared = BSM_SHARED;
        bsm_tab[bs_id].bs_max_npages = npages;
        
        restore(ps);
        return npages;
    }
    if(bsm_tab[bs_id].bs_status == BSM_MAPPED && bsm_tab[bs_id].bs_shared == BSM_SHARED)
    {
        restore(ps);
        return bsm_tab[bs_id].bs_max_npages;            
    }

}


