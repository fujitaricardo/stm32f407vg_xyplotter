/**
 *  Engenharia Eletrica - Universidade Estadual de Londrina
 *  Mesa xy automatizada microcontrolada com stm32f407vg
 *
 *	autores     Giovani Augusto de Lima Freitas; Guilherme Almeida Pessoa; Guilherme Brandao da Silva; Ricardo Fujita
 *	version     v1.0
 *	ide         CooCox 1.7.7
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
