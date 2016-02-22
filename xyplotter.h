#include <stdio.h>
#include <stm32f4xx_gpio.h>
#include "stm32f4xx_rcc.h"
#include <stm32f4xx_adc.h>
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_hd44780.h"
#include "defines.h"

void motorInit(void);
void endInit(void);
void joystickInit(void);
void buttonsInit(void);
void xyPlotterInit(void);
uint16_t readADC(uint8_t channel);
uint16_t readJoystick(char eixo);
int posicaoJoystick(char eixo);
int readJSButton(void);
void manualControl(void);
int readEnd(char eixo);
int readButtons(char button);
void counterclockwise(char eixo);
void clockwise(char eixo);
void runClockwise(int passos, char eixo);
void runCounterclockwise(int passos, char eixo);
int getDistancia(void);
char getUnidade(void);
int converteMilimetro(void);
int convertePolegada(void);
void setPassos(char direction);
void runMotor(int passos, char direction);
void calibraZero(void);
