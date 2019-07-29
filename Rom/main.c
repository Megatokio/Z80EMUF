

// > cd …/Rom
// > zasm -c sdcc -L lib -I include EMUF.asm 


#include <stdio.h>

static int n;
extern unsigned long int system_time;
static float f = 0x842;

void foo(){ f=0x421; }

void main()
{
	unsigned long int i;
	char c;
	
	f = 0.0;
	foo();
	f = 1.0;
	foo();
	f = 2.0;
	foo();
	f = 3.0;
	foo();
	f = 4.0;
	foo();
	f = 5.0;
	foo();
	f = -1.0;
	foo();
	f = 1.0e30;
	foo();
	f = 1.0e-30;
	foo();
	f = 1.5e-30;
	
	printf("\n\rZ80EMUF booting...\n\r");
	printf("press any key to continue");

	c = getchar();
	printf("\n\r");

	if(c>' ' && c<='~')
	{
		printf("your key of choice was '%c'\n\r",c);
	}
	
	for(;;)
	{
//		printf("\rsystem time: %lu",system_time);
		printf("\rsystem time: %.2f seconds",system_time/100.0);
		for(i=0;i<65000UL;i++){n+=1;}		
	}
}
