/********************************* INCLUDES ***********************************/

#include "Kernel.h"

#include "Drv_GPIO.h"

#include "BSPConfig.h"

#include "postypes.h"

#include "LPC17xx.h"

/***************************** MACRO DEFINITIONS ******************************/

/***************************** TYPE DEFINITIONS *******************************/

/**************************** FUNCTION PROTOTYPES *****************************/
extern void Board_LedInit(void);
extern void Board_LedOn(uint32_t ledNo);
extern void Board_LedOff(uint32_t ledNo);

/******************************** VARIABLES ***********************************/

/**************************** PRIVATE FUNCTIONS ******************************/

static void delayMs(uint32_t delayInMs)
{
	#define MILISECONDS                 (2500000 / 100 - 1)

	uint32_t counterReset = 0;

	counterReset = (delayInMs * MILISECONDS);
	while (counterReset--);
}

/***************************** PUBLIC FUNCTIONS *******************************/
		
int div(int a, int b)
{
	return a / b;
}

int heartBeat()
{
    while (1)
    {
		Board_LedOff(1);
        delayMs(1000);

		OS_Yield();

        Board_LedOn(1);
        delayMs(1000);

		OS_Yield();

		{
			int a = 5, b = 0;
			int c;
			c = div(a, b);
		}
    }
}
int main()
{
    Board_LedInit();
    heartBeat();

    return 0;
}
