#include <lab0.h>
#include <stdio.h>

static unsigned long    *esp;
static unsigned long    *ebp;
void printos(){

	asm("movl %esp,esp");
	asm("movl %ebp,ebp");


	kprintf("Before[0x%08x]: 0x%08x\n", ebp+2, *(ebp+2));
	kprintf("After[0x%08x]: 0x%08x\n", ebp, *ebp);
	int offset = 0;
	while((offset < 4) && (esp + offset < ebp))
	{
		kprintf("\telement[0x%08x]: 0x%08x\n",esp+offset, *(esp+offset));
		offset++;
	}
}
