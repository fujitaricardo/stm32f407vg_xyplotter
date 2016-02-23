#include "tm_stm32f4_hd44780.h"
#include "xyplotter.h"
#include "semihosting.h"

int main(void){
	SystemInit();
    TM_HD44780_Init(16, 2);
    xyPlotterInit();
    calibraZero();

    moveParaXY(100, 120, 'm');
    manualControl();
    return 0;
}
