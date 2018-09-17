/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>
bs_map_t bsm_tab[TOTAL_STORES];
/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
    int i,j;
    for(i=0;i<TOTAL_STORES;i++)
    {
        bsm_tab[i].bs_status = BSM_UNMAPPED;			/* MAPPED or UNMAPPED		*/
        for(j=0;j<NPROC;j++)
        {
            bsm_tab[i].bs_pid[j]=-1;
        }
        //bsm_tab[i].bs_pid = -1;				/* process id using this slot   */
        //bsm_tab[i].bs_vpno = -1;				/* starting virtual page number */
        //bsm_tab[i].bs_npages = 0;			/* number of pages in the store */
        bsm_tab[i].bs_sem = -1;				/* semaphore mechanism ?	*/
        bsm_tab[i].bs_shared = -1;          /* Whether the bs is shared or private */
    }
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
    int i;
    for(i=0;i<TOTAL_STORES;i++)
    {
        if(bsm_tab[i].bs_status == BSM_UNMAPPED )
        {
            bsm_tab[i].bs_status = BSM_MAPPED;
            bsm_tab[i].bs_shared = BSM_PRIVATE;
            *avail = i;
            break;
        }

    }
    return OK;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
 int j;
    
        bsm_tab[i].bs_status = BSM_UNMAPPED;			/* MAPPED or UNMAPPED		*/
        for(j=0;j<NPROC;j++)
        {
            bsm_tab[i].bs_pid[j]=-1;
        }
        //bsm_tab[i].bs_pid = -1;				/* process id using this slot   */
        //bsm_tab[i].bs_vpno = -1;				/* starting virtual page number */
        //bsm_tab[i].bs_npages = 0;			/* number of pages in the store */
        bsm_tab[i].bs_sem = -1;				/* semaphore mechanism ?	*/
        bsm_tab[i].bs_shared = -1;          /* Whether the bs is shared or private */
       
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, unsigned long vaddr, int* store, int* pageth)
{
    unsigned int vpno;
    int i;
    vpno = vaddr >> 12; 
 for(i=0;i<TOTAL_STORES;i++)
 {
          if(vpno >= bsm_tab[i].bs_vpno[pid] && vpno< bsm_tab[i].bs_vpno[pid] + bsm_tab[i].bs_npages[pid] )
         {
             *store= i;
             *pageth = vpno - bsm_tab[i].bs_vpno[pid]; /* Page offset into the Backing store */
             return OK;
         }
     
 }
 *store = -1;
 *pageth = -1;
 return SYSERR;   
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
  
  if(bsm_tab[source].bs_status == BSM_MAPPED && bsm_tab[source].bs_shared == BSM_PRIVATE)
  {
     bsm_tab[source].bs_max_npages = npages;   
  }	
  bsm_tab[source].bs_pid[pid] = 1;				/* process id using this slot   */
  bsm_tab[source].bs_vpno[pid] = vpno;				/* starting virtual page number */
  bsm_tab[source].bs_npages[pid] = npages;			/* number of pages in the store */
  bsm_tab[source].bs_sem = -1;				/* semaphore mechanism ?	*/ 
  return OK;
  
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
    int i,j,store,pageth;
     unsigned long addr,dir_offset,page_table_offset;
    unsigned int k, range; 

        bsm_lookup(pid, vpno*4096 , &store, &pageth);
        i = store;
        int n=0;
        if(bsm_tab[i].bs_vpno[pid]== vpno)
        {
            if(bsm_tab[i].bs_status== BSM_MAPPED)
            {
                for(j=0;j<NPROC;j++)
                {
                   if(proctab[j].store[i]== VIRT_SHARE)
                   {
                       n++;
                   }
                }
                if(n<=1 || bsm_tab[i].bs_shared == BSM_PRIVATE)
                {
                    bsm_tab[i].bs_max_npages = 0;
                }

            }
            
            range = vpno + bsm_tab[i].bs_npages[pid];
            bsm_tab[i].bs_pid[pid] = -1;				/* process id using this slot   */
            bsm_tab[i].bs_vpno[pid] = -1;				/* starting virtual page number */
            if(bsm_tab[i].bs_max_npages != 0 && bsm_tab[i].bs_npages[pid] == bsm_tab[i].bs_max_npages)
            {
                bsm_tab[i].bs_max_npages =0; 
                bsm_tab[i].bs_npages[pid]=0;  
                for(j=0;j<NPROC;j++)
                {
                if(bsm_tab[i].bs_npages[j]> bsm_tab[i].bs_max_npages)
                {
                    bsm_tab[i].bs_max_npages = bsm_tab[i].bs_npages[j];
                }
                }
            }
            
            for(k=vpno; k<range; k++)
            {

            addr = k * NBPG;
            dir_offset= (addr&0xffc00000) >> 22;
            page_table_offset = addr << 10;
            page_table_offset = page_table_offset >> 22;
           
            pd_t *pd_entry = proctab[pid].pdbr  + (dir_offset * sizeof(pd_t));
           
            if(pd_entry->pd_pres == 0)
            {
                return OK;
            }
            pt_t *pt = (pd_entry->pd_base * NBPG )+ (page_table_offset * sizeof(pt_t));
             pt->pt_pres = 0;
            if(frm_tab[pt->pt_base - FRAME0].fr_status == FRM_MAPPED)
            {
                write_bs((pt->pt_base - FRAME0)*4096,i,pageth);
                frm_tab[pt->pt_base - FRAME0].fr_pid[pid] = -1;
                frm_tab[pt->pt_base - FRAME0].fr_vpno[pid] = -1;
                if(flag == 1)
                {
                cqremove(pt->pt_base - FRAME0);
                free_frm(pt->pt_base - FRAME0);}
                //kprintf("REF CNT %d\n",frm_tab[pd_entry->pd_base-FRAME0].fr_refcnt );
                frm_tab[pd_entry->pd_base-FRAME0].fr_refcnt--;

                //kprintf("REF CNT22 %d\n",frm_tab[pd_entry->pd_base-FRAME0].fr_refcnt );
            }
            if(frm_tab[pd_entry->pd_base-FRAME0].fr_refcnt == 0 && frm_tab[pd_entry->pd_base-FRAME0].fr_status == FRM_MAPPED) 
            {
                 pd_entry->pd_pres =0; 
                free_frm(pd_entry->pd_base-FRAME0);
            }
           
            
            }
            

        }

        int v = 0, b = 0;
    for(i=0;i<1024;i++)
    {
        for(j=1;j<NPROC;j++)
        {
            if(frm_tab[i].fr_pid[j]==1)
            break;
        }
        if(frm_tab[i].fr_type == FR_PAGE && frm_tab[i].fr_pid[j] == pid)   v++;
        if(frm_tab[i].fr_type == FR_TBL && frm_tab[i].fr_pid[j] == pid) b =i;
    }

    if(v==0)
    frm_tab[i].fr_status = FRM_UNMAPPED;
    return OK;
}


