#include <stm32f4xx_gpio.h>
#include "stm32f4xx_rcc.h"
#include <stm32f4xx_adc.h>
#include "defines.h"

void motorInit(void);
void endInit(void);
void joystickInit(void);
void buttonsInit(void);
uint16_t readADC(uint8_t channel);
int readDigitalPin(uint8_t gpio, uint8_t pin);
uint16_t readJoystick(char eixo);
int posicaoJoystick(char eixo);
int readJSButton(void);
void manualControl(void);
int readEnd(char eixo);
int readButtons(char button);
void delay(double time);
void counterclockwise(char eixo);
void clockwise(char eixo);
int getDistancia(void);
char getUnidade(void);
int converteMilimetro(void);
int convertePolegada(void);
void setPassos(char direction);
void runMotor(int passos, char direction);
