/**
 *	Engenharia Eletrica - Universidade Estadual de Londrina
 *	Mesa xy automatizada microcontrolada com stm32f407vg
 *
 *	autor 		Ricardo Fujita; Guilherme Brandao da Silva; Giovani Augusto de Lima Freitas; Guilherme Almeida Pessoa
 *	version		v1.0
 *	ide			CooCox 1.7.7
 *
 */

#include <stdio.h>
#include <stm32f4xx_gpio.h>
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_flash.h"
#include <stm32f4xx_adc.h>
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_hd44780.h"
#include "defines.h"
#include "semihosting.h"
#include "stm32_ub_string.h"

//MOTOR_A_PIN1  PE7
//MOTOR_A_PIN2  PE9
//MOTOR_A_PIN3  PE11
//MOTOR_A_PIN4  PE13

//MOTOR_B_PIN1  PE8
//MOTOR_B_PIN2  PE10
//MOTOR_B_PIN3  PE12
//MOTOR_B_PIN4  PE14

//CANETA_PIN1 PA1
//CANETA_PIN2 PA3
//CANETA_PIN3 PA5
//CANETA_PIN4 PA7

//END1  PB11
//END2  PB12

//JOYSTICK_X  PC4
//JOYSTICK_Y  PC5
//JSBUTTON    PC1

//BUTTON1 PC2
//BUTTON2 PC3

//HD44780_RS_PIN  PE0
//HD44780_E_PIN   PE1
//HD44780_D4_PIN  PE2
//HD44780_D5_PIN  PE3
//HD44780_D6_PIN  PE4
//HD44780_D7_PIN  PE5

//1 passo = (0,17/8)mm
//6 passos = 1 mm
//150 passos = 1 pol
//1 pol = 2,54 mm

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
void runClockwise2(int passos, char eixo);
void runCounterclockwise(int passos, char eixo);
void runCounterclockwise2(int passos, char eixo);
void runDistancia_unidade(float distancia, char unidade, char eixo, char sentido);
void runCaneta(int passos, char dir);

/*funcoes de funcionamento da mesa xy*/
void calibraZero(void);
void calibraCaneta(void);
void moveParaXY(float coord_x, float coord_y, char unidade);
void setFura(void);
void setAbaixaCaneta(void);
void setSobeCaneta(void);
void setDesenhaQuadrado(void);
void manualControl(void);
void menu(void);
void modoPerfuracao(void);
void atualizaDisplayDistancia(void);

/*funcoes de conversoes entre passos e unidades de medida*/
int convertePassos_Unidade(float distancia, char unidade);
float converteUnidade_Passos(int passos, char unidade);

/*funcoes para conversao de float para string e semihosting*/
char* ftoa(char* dest, double num, int afterPoint, int sign);
char* inc(char* s, char* e);
void reverse(char* p, char* q);
void readStringSH(char *str);
