#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>



int main( )
{
        /* These test cases are only used for test purpose.
         * The provided results do not guarantee your correctness.
         * You need to read the PA2 instruction carefully.
         */
        /* The hook to shutdown QEMU for process-like execution of XINU.
         * This API call exists the QEMU process.
         */
        shutdown();
}
