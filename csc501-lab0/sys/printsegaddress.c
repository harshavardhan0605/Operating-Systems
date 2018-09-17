#include <lab0.h>
#include <stdio.h>

extern int etext;
extern int edata;
extern int end;

void printsegaddress(){

int *ptrtext;
int *ptrdata;
int *ptrbss;

ptrtext = &etext;
ptrdata = &edata;
ptrbss = &end;

kprintf("Current: etext[0x%08x]=0x%08x, edata[0x%08x]=0x%08x, ebss[0x%08x]=0x%08x\n", ptrtext, *(ptrtext), ptrdata, *(ptrdata), ptrbss,  *(ptrbss));
kprintf("Preceding: etext[0x%08x]=0x%08x, edata[0x%08x]=0x%08x, ebss[0x%08x]=0x%08x\n", ptrtext - 1, *(ptrtext - 1), ptrdata -1, *(ptrdata -1), ptrbss -1,  *(ptrbss-1));
kprintf("After: etext[0x%08x]=0x%08x, edata[0x%08x]=0x%08x, ebss[0x%08x]=0x%08x\n", ptrtext+1, *(ptrtext+1), (ptrdata+1), *(ptrdata+1), ptrbss+1, *(ptrbss+1));
}

