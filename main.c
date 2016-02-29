/**
 *	Engenharia Eletrica - Universidade Estadual de Londrina
 *	Mesa xy automatizada microcontrolada com stm32f407vg
 *
 *	autor 		Ricardo Fujita; Guilherme Brandao da Silva; Giovani Augusto de Lima Freitas; Guilherme Almeida Pessoa
 *	version		v1.0
 *	ide			CooCox 1.7.7
 *
 */

#include "xyplotter.h"

int main(void){
    xyPlotterInit();
    calibraZero();
    calibraCaneta();
    menu();
    
    return 0;
}
