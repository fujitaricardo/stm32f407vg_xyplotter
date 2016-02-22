#include "tm_stm32f4_hd44780.h"
#include "xyplotter.h"

int main(void){
    SystemInit();
    TM_HD44780_Init(16, 2);
    xyPlotterInit();
    calibraZero();
    manualControl();
    return 0;
}
