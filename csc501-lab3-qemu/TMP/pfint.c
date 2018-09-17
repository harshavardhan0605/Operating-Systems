/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include<proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */


SYSCALL pfint()
{
  STATWORD ps;
  disable(ps);
  int store,pageth;
  unsigned long addr, pdbr,vpno,dir_offset,page_table_offset,offset;
  addr = read_cr2();
  vpno = (addr&0xfffff000) >>12;
  dir_offset= (addr&0xffc00000) >> 22;
  page_table_offset = addr << 10;
  page_table_offset = page_table_offset >> 22;
  offset = addr << 20;
  offset = offset >> 20;
  
  pd_t *pd_entry = proctab[currpid].pdbr  + (dir_offset * sizeof(pd_t));
  if(bsm_lookup(currpid,addr,&store,&pageth)== SYSERR)
  {
    //kprintf("You are trying to access an illegal address which is not mapped\n");
    kill(currpid);
    restore(ps);
    return OK;
  }
  int new_frame = -1; 
  if(bsm_tab[store].bs_status == BSM_SHARED)
  {
    int j;
     unsigned long addr2,vpno2,dir_offset2,page_table_offset2,offset2;
    for(j=0;j<NPROC;j++)
    {
      if(bsm_tab[store].bs_pid[j]== 1)
      {
        vpno2 = bsm_tab[store].bs_vpno[j] + pageth;
        addr2 = vpno2 * 4096;
        dir_offset2= (addr2 & 0xffc00000) >> 22;
        page_table_offset2 = addr2 << 10;
        page_table_offset2 = page_table_offset2 >> 22;
        offset2 = addr2 << 20;
        offset2 = offset2 >> 20;
        pd_t *pd_entry2 = proctab[j].pdbr  + (dir_offset2 * sizeof(pd_t));
        if(pd_entry2->pd_pres == 1)
        {
          pt_t *pt2 = ((pd_entry2->pd_base ) * NBPG )+ (page_table_offset2 * sizeof(pt_t));
          if(pt2->pt_pres == 1)
          {
            new_frame = pt2->pt_base - FRAME0;
            break;
          }
        }
      }
    }
  }
   int table;
  if(	pd_entry->pd_pres == 0)
  {
    int pt;
    table = create_PageTable(currpid);
    pd_entry->pd_pres = 1;
    pd_entry->pd_base = (FRAME0 + table) ;
  }
  frm_tab[pd_entry->pd_base-FRAME0].fr_refcnt++;
  if(new_frame == -1)
  {
  get_frm(&new_frame);
  cqinsert(new_frame);
    read_bs(((FRAME0+new_frame)*NBPG),store,pageth);
  }
  frm_tab[new_frame].fr_status = FRM_MAPPED;
  frm_tab[new_frame].fr_pid[currpid] = 1;
  frm_tab[new_frame].fr_vpno[currpid] = vpno;
  frm_tab[new_frame].fr_type = FR_PAGE;
  frm_tab[new_frame].fr_dirty = CLEAN;
  frm_tab[new_frame].lfu_cnt = 1;
  pt_t *pt = ((pd_entry->pd_base ) * NBPG )+ (page_table_offset * sizeof(pt_t));
		pt->pt_pres = 1; /* page is present?		*/
		pt->pt_base = (FRAME0 + new_frame) ; 
    pt->pt_acc =1; /* location of page?		*/
restore(ps);
  return OK;
}


