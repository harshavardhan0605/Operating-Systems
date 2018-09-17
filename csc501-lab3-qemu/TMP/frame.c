/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>
extern int arr[size];
extern int cqfront;
extern int cqrear;
extern int reference;

int global_pt[4];
fr_map_t frm_tab[NFRAMES];

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm()
{
  int i,j;
	for (i = 0; i < NFRAMES; i++) {
  
		frm_tab[i].fr_status = FRM_UNMAPPED;
		for(j=0;j<NPROC;j++)
        {
            frm_tab[i].fr_pid[j]=-1;
        }
    frm_tab[i].fr_refcnt = 0;
		frm_tab[i].fr_type = -1;
    frm_tab[i].fr_dirty = 0;
		frm_tab[i].lfu_cnt =0;

	}
  return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
	STATWORD ps;
	disable(ps);
	unsigned long addr,dir_offset,page_table_offset;
  int i;
	for (i = 0; i < NFRAMES; i++) {
		if (frm_tab[i].fr_status == FRM_UNMAPPED) {
      *avail = i ;
      restore(ps);
			return OK;
		}
  }
	if(grpolicy()== SC)
	{
		int got_frame;

     
		got_frame = cqreplace();
		//kprintf("\nFRAME %d is replaced\n",got_frame +1024);
		*avail = got_frame;
		int j,pid,vpno,store,pageth;
		for(j=1;j<NPROC;j++)
		{
			if(frm_tab[got_frame].fr_pid[j]==1)
			{break;}
		}
		pid = j;
		vpno = frm_tab[got_frame].fr_vpno[pid];
            addr = vpno * NBPG;
            dir_offset= (addr&0xffc00000) >> 22;
            page_table_offset = addr << 10;
            page_table_offset = page_table_offset >> 22;
           
            pd_t *pd_entry = proctab[pid].pdbr  + (dir_offset * sizeof(pd_t));
            if(pd_entry->pd_pres == 0)
            {
                return OK;
            }

            pt_t *pt = (pd_entry->pd_base * NBPG )+ (page_table_offset * sizeof(pt_t));
						if(bsm_lookup(j,addr,&store,&pageth)== SYSERR)
  {
    //kprintf("You are trying to access an illegal address which is not mapped\n");
    kill(currpid);
    restore(ps);
    return OK;
  }	
						 write_bs((pt->pt_base - FRAME0)*4096,store,pageth);
		
            pt->pt_pres = 0; 
		free_frm(got_frame); 
    } 
		if(grpolicy()==LFU)
		{
			int max_cnt =0, max_vpno =0,pid1=-1,vpno;
			int i,j,store,pageth
			;
			for(i=0;i<NFRAMES;i++)
			{
				if(max_cnt< frm_tab[i].lfu_cnt)
				max_cnt = frm_tab[i].lfu_cnt;
			}
			for(i=0;i<NFRAMES;i++)
				{
					if(frm_tab[i].lfu_cnt == max_cnt)
					{

						for(j=1;j<NPROC;j++)
						{
							if(frm_tab[i].fr_pid[j]==1)
							{break;}
							}
							if(max_vpno < frm_tab[i].fr_vpno[j] ){
							max_vpno = frm_tab[i].fr_vpno[j];
							pid1 = j;
							}
						}
					}
			for(i=0;i<NFRAMES;i++)
			{
				if(frm_tab[i].fr_vpno[pid1] == max_vpno)
				{
					*avail = i;
					vpno = frm_tab[i].fr_vpno[pid1];
            addr = vpno * NBPG;
            dir_offset= (addr&0xffc00000) >> 22;
            page_table_offset = addr << 10;
            page_table_offset = page_table_offset >> 22;
						pd_t *pd_entry = proctab[pid1].pdbr  + (dir_offset * sizeof(pd_t));\
            if(pd_entry->pd_pres == 0)
            {
                return OK;
            }
            pt_t *pt = (pd_entry->pd_base * NBPG )+ (page_table_offset * sizeof(pt_t));
							if(bsm_lookup(j,addr,&store,&pageth)== SYSERR)
  {
    //kprintf("You are trying to access an illegal address which is not mapped\n");
    kill(currpid);
    restore(ps);
    return OK;
  }	
						 write_bs((pt->pt_base - FRAME0)*4096,store,pageth);
		
          
            pt->pt_pres = 0; 
						//kprintf("\nFrame %d is replaced\n",i);
						free_frm(i);
					return OK;
				}
			}

		}
		
	
	restore(ps);
  return OK;

}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
	STATWORD ps;
	disable(ps);
	int j,k=0;
	if(frm_tab[i].fr_type == FR_PAGE)
	{
	{
		if(frm_tab[i].fr_pid[j] == 1)
		{
			k++;
		}
	}
	if(k==0){
	reset_frame(i);}
	
  restore(ps);
  return OK;
	}
	if(frm_tab[i].fr_type == FR_TBL)
	{
			//kprintf("RPAGE Table %d \n", i);
		int j,pid;
		for(j=1;j<NPROC;j++)
		{
			if(frm_tab[i].fr_pid[j]==1)
			{break;}
		}
		pid = j;

		int vpno;
		unsigned long addr,page_table_offset;
		vpno = frm_tab[i].fr_vpno[pid];
		page_table_offset = addr << 10;
  	page_table_offset = page_table_offset >> 22;
		pt_t *pt = ((FRAME0 + i) * NBPG )+ (page_table_offset * sizeof(pt_t));
		pt->pt_pres = 0; /* page is present?		*/
		pt->pt_user = 0;/* is use level protection?	*/
		pt->pt_pwt = 0; /* write through for this page? */
		pt->pt_pcd = 0; /* cache disable for this page? */
		pt->pt_acc = 1; /* page was accessed?		*/
    pt->pt_dirty = CLEAN; /* page was written?		*/
		pt->pt_mbz = 0;  /* must be zero			*/
		pt->pt_global = 0; /* should be zero in 586	*/
		pt->pt_avail = 0;  /* for programmer's use		*/
		pt->pt_base = 0;  /* location of page?		*/
		reset_frame(i);
		restore(ps);
		return OK;
	}
	if(frm_tab[i].fr_type == FR_DIR)
	{

			//kprintf("RPAGE DIrect %d \n", i);
		int pid;
		int j;
		for(j=1;j<NPROC;j++)
		{
			if(frm_tab[i].fr_pid[j]==1)
			{break;}
		}
		pid = j;
		for (j = 4; j < 1024; j++) {
		pd_t *pd_entry = proctab[pid].pdbr + (j * sizeof(pd_t));
		
		pd_entry->pd_pres = 0;
		pd_entry->pd_write = 1;
		pd_entry->pd_user = 0;
		pd_entry->pd_pwt = 0;
    pd_entry->pd_pcd = 0;
		pd_entry->pd_acc = 0;
		pd_entry->pd_mbz = 0;
		pd_entry->pd_fmb = 0;
		pd_entry->pd_global = 0;
		pd_entry->pd_avail = 0;
		pd_entry->pd_base = 0;
	}
	reset_frame(i);
	restore(ps);
	return OK;
}
}

int create_PageTable(int pid) {

	int i;
	int frame = 0 ; 
  get_frm(&frame);
		//kprintf("PAGE TABLE %d \n", frame);
    ////kprintf("Page Table NUll %d\n ", frame);
	if (frame == -1) {
		return SYSERR;
	}
	frm_tab[frame].fr_status = FRM_MAPPED;
	frm_tab[frame].fr_pid[pid] = 1;
	frm_tab[frame].fr_vpno[pid] = -1;
  frm_tab[frame].fr_refcnt = 0;
	frm_tab[frame].fr_type = FR_TBL;
	frm_tab[frame].fr_dirty = CLEAN;
	for (i = 0; i < 1024; i++) {
		pt_t *pt = ((FRAME0 + frame) * NBPG) + (i * sizeof(pt_t));	
		pt->pt_pres = 0; /* page is present?		*/
		pt->pt_write = 1; /* page is writable?		*/
		pt->pt_user = 0;/* is use level protection?	*/
		pt->pt_pwt = 0; /* write through for this page? */
		pt->pt_pcd = 0; /* cache disable for this page? */
		pt->pt_acc = 1; /* page was accessed?		*/
    pt->pt_dirty = CLEAN; /* page was written?		*/
		pt->pt_mbz = 0;  /* must be zero			*/
		pt->pt_global = 0; /* should be zero in 586	*/
		pt->pt_avail = 0;  /* for programmer's use		*/
		pt->pt_base = 0;  /* location of page?		*/
		
	}
	return frame;
}

int create_PageDirectory(int pid) {
	int i;
  int frame=0;
	get_frm(&frame);
	if (frame == -1) {
		return -1;
	}
	//kprintf("PAGE DIRECTORY %d \n", frame);
	frm_tab[frame].fr_dirty = CLEAN;
	frm_tab[frame].fr_pid[pid] = 1;
	frm_tab[frame].fr_refcnt = 0;
	frm_tab[frame].fr_status = FRM_MAPPED;
	frm_tab[frame].fr_type = FR_DIR;
	frm_tab[frame].fr_vpno[pid] = -1;

	proctab[pid].pdbr = (FRAME0 + frame)* 4096;
	
	for (i = 0; i < 1024; i++) {
		pd_t *pd_entry = proctab[pid].pdbr + (i * sizeof(pd_t));
		pd_entry->pd_pres = 0;
		pd_entry->pd_write = 1;
		pd_entry->pd_user = 0;
		pd_entry->pd_pwt = 0;
    pd_entry->pd_pcd = 0;
		pd_entry->pd_acc = 0;
		pd_entry->pd_mbz = 0;
		pd_entry->pd_fmb = 0;
		pd_entry->pd_global = 0;
		pd_entry->pd_avail = 0;
		pd_entry->pd_base = 0;
		

		if (i < 4) {
			pd_entry->pd_pres = 1;
			pd_entry->pd_write = 1;
			pd_entry->pd_base = global_pt[i];
		}
	}
		return frame;
}

int initializeGlobalPageTable() {
	int i, j, k;
	for (i = 0; i < 4; i++) {
		k = create_PageTable(NULLPROC);
		if (k == -1) {
			return SYSERR;
		}
		global_pt[i] = FRAME0 + k;

		for (j = 0; j < 1024; j++) {
			pt_t *pt = global_pt[i] * NBPG + j * sizeof(pt_t);

			pt->pt_pres = 1;
			pt->pt_write = 1;
			pt->pt_base = j + i * 1024;
		}
			frm_tab[k].fr_refcnt++;
	}
	return OK;
}
int write2CR3(int pid) {
	unsigned int pdbr = (proctab[pid].pdbr) / NBPG;

/*	if ((frm_tab[pdbr - FRAME0].fr_status != FRM_MAPPED) || (frm_tab[pdbr - FRAME0].fr_type != FR_DIR) || (frm_tab[pdbr - FRAME0].fr_pid != pid)) {
		return SYSERR;
	} */
	write_cr3(proctab[pid].pdbr);
	return OK;
}

void reset_frame(int i)
{
	int j;
frm_tab[i].fr_status = FRM_UNMAPPED;
for(j=0;j<NPROC;j++)
{
  frm_tab[i].fr_pid[j] = -1;
	frm_tab[i].fr_vpno[j] = -1;
}
  
  frm_tab[i].fr_refcnt=0;
  frm_tab[i].fr_type = -1;
  frm_tab[i].fr_dirty = 0;
	frm_tab[i].lfu_cnt =0;
}

int sc_check(int frame)
{
	int j,pid;
	unsigned long vpno, addr,dir_offset,page_table_offset; 
	for(j=0;j<NPROC;j++)
	{
		if(frm_tab[frame].fr_pid[j]==1)
		{
			pid = j;
			break;
		}
	}
	vpno = frm_tab[frame].fr_vpno[pid];
	addr = vpno * NBPG;
	dir_offset= addr >> 22;
	page_table_offset = addr << 10;
	page_table_offset = page_table_offset >> 22;
	pd_t *pd_entry = proctab[currpid].pdbr  + (dir_offset * sizeof(pd_t));
	pt_t *pt = (pd_entry->pd_base * NBPG )+ (page_table_offset * sizeof(pt_t));
	if(pt->pt_acc == 1)
	{
		pt->pt_acc = 0;
		return 0;
	}
	else if( pt->pt_acc == 0)
	{
		return 1;
	}
}

int cqinsert(int frame)
{
    if ((cqfront == 0 && cqrear == size-1) ||
            (cqrear == cqfront-1))
    {
        return;
    }
 
    else if (cqfront == -1) 
    {
				////kprintf("\n Insert First Element ");
        cqfront =0;
				cqrear =0;
				reference = 0;
        arr[cqrear] = frame;
    }
 
    else if (cqrear == size-1 && cqfront != 0)
    {
        cqrear = 0;
        arr[cqrear] = frame;
    }
 
    else
    {
			int z;
			for(z=0;z<size;z++)
			{
				if(arr[z]==frame)
				{
					return OK;
				}
			}
        cqrear++;
        arr[cqrear] = frame;

				////kprintf("At position %d value inserted is %d",cqrear,arr[cqrear]);
    }
  return OK;
}
int cqremove(int frame)
{
	
  int i, to_remove = -1;
  for(i=0;i<size;i++)
  {
    if(arr[i]== frame)
    {
      to_remove = i;
      break;
    }
  }
    if (cqfront == -1)
    {
        return OK;
    }
 
    if (cqfront == cqrear)
    {

        cqfront = -1;
        cqrear = -1;
        reference = cqfront;
				arr[to_remove]= -1;
				return OK;
    }
    else if (cqfront == size-1)
        cqfront = 0;
        for(i=to_remove;i<cqrear;i++)
        {
          arr[i]= arr[i+1];
        }
        cqrear--;
 	  return OK;
}
int cqreplace()
{
  int n;
  int k=0;
	while (k<1)
  {
    n = sc_check(arr[reference]);
    if(n==0)
    {
      reference++;
			if(reference == cqrear)
			{
				reference = cqfront;
			}
    }
    else
    {
      k = arr[reference];
      reference++;
    }
  }
  return k;
} 

