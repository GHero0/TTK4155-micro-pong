#include "tests/test_inter2.h"
#include <avr/io.h>
#include <stdio.h>

void test_inter2(void)
{
    if (INTF2 == 1)
    {
        printf("Hey!\n");
    }
}
