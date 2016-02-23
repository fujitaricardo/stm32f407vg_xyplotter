#include <stdio.h>
#include <stm32f4xx_gpio.h>
#include "stm32f4xx_rcc.h"
#include <stm32f4xx_adc.h>
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_hd44780.h"
#include "defines.h"


/*funcoes de inicializacao dos sensores e motores*/
void motorInit(void);
void endInit(void);
void joystickInit(void);
void buttonsInit(void);
void xyPlotterInit(void);

/*funcoes de leitura dos sensores*/
uint16_t readADC(uint8_t channel);
uint16_t readJoystick(char eixo);
int posicaoJoystick(char eixo);
int readJSButton(void);
int readEnd(char eixo);
int readButtons(char button);

/*funcoes de funcionamento dos motores*/
void counterclockwise(char eixo);
void clockwise(char eixo);
void runClockwise(int passos, char eixo);
void runCounterclockwise(int passos, char eixo);
void runDistancia_unidade(float distancia, char unidade, char eixo, char sentido);
void moveParaXY(float coord_x, float coord_y, char unidade);

/*funcoes de funcionamento da mesa xy*/
void calibraZero(void);
void manualControl(void);
void atualizaDisplayDistancia(void);

/*funcoes de conversoes entre passos e unidades de medida*/
int convertePassos_Unidade(float distancia, char unidade);
float converteUnidade_Passos(int passos, char unidade);

/*funcoes para conversao de float para string*/
char* ftoa(char* dest, double num, int afterPoint, int sign);
char* inc(char* s, char* e);
void reverse(char* p, char* q);
