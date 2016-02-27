/**
 *	Engenharia Eletrica - Universidade Estadual de Londrina
 *	Mesa xy automatizada microcontrolada com stm32f407vg
 *
 *	autor 		Ricardo Fujita; Guilherme Brandao da Silva; Giovani Augusto de Lima Freitas; Guilherme Almeida Pessoa
 *	version		v1.0
 *	ide			CooCox 1.7.7
 *
 */

#include "tm_stm32f4_hd44780.h"
#include "xyplotter.h"

int main(void){
	SystemInit();
    TM_HD44780_Init(16, 2);
    xyPlotterInit();
    calibraZero();
    //calibraCaneta();
    //menu();

    while(1){
        manualControl();
    	//setDesenhaQuadrado();
    	/*moveParaXY(15, 0, 'm');
    	setFura();
    	moveParaXY(15, 15, 'm');
    	setFura();
    	moveParaXY(0, 15, 'm');
    	setFura();
    	moveParaXY(0, 0, 'm');
    	setFura();*/
    	Delayms(250);
    }
    return 0;
}
