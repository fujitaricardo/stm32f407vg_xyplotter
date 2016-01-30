/**
 *  Defines for your entire project at one place
 * 
 *	@author 	Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.com
 *	@version 	v1.0
 *	@ide		Keil uVision 5
 *	@license	GNU GPL v3
 *	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#ifndef TM_DEFINES_H
#define TM_DEFINES_H

/* Put your global defines for all libraries here used in your project */

//delay para acionamento dos motores
#define MOTOR_SPEED 50000	//calibrar para que este tempo seja de 5ms!

//constante de conversão de passos
#define PASSOS_mm	6		//6 passos = 1 mm
#define PASSOS_pol	150		//150 passos = 1 pol

//pinagem motores
#define MOTOR_PORT		GPIOE
#define MOTOR_A_PIN1	GPIO_Pin_7
#define	MOTOR_A_PIN2	GPIO_Pin_8	
#define	MOTOR_A_PIN3	GPIO_Pin_9
#define	MOTOR_A_PIN4	GPIO_Pin_10
#define MOTOR_B_PIN1	GPIO_Pin_11
#define	MOTOR_B_PIN2	GPIO_Pin_12	
#define	MOTOR_B_PIN3	GPIO_Pin_13
#define	MOTOR_B_PIN4	GPIO_Pin_14

//pinagem fim de curso
#define END_PORT		GPIOB
#define END1			GPIO_Pin_0
#define END2			GPIO_PIN_1

//pinagem joystick
#define JOYSTICK_PORT	GPIOC
#define	JSBUTTON_PORT	GPIOB
#define JOYSTICK_X 		GPIO_Pin_4 	//analogico
#define JOYSTICK_Y		GPIO_Pin_5 	//analogico
#define JSBUTTON 		GPIO_Pin_2 	//digital

//pinagem pushbuttons
#define	BUTTON_PORT		GPIOA
#define	BUTTON1 		GPIO_Pin_6
#define BUTTON2 		GPIO_Pin_7


//pinagem display LCD
//RS - Register select pin
#define HD44780_RS_PORT     GPIOA
#define HD44780_RS_PIN      GPIO_PIN_2
//E - Enable pin
#define HD44780_E_PORT      GPIOA
#define HD44780_E_PIN       GPIO_PIN_3
//D4 - Data 4 pin
#define HD44780_D4_PORT     GPIOA
#define HD44780_D4_PIN      GPIO_PIN_4
//D5 - Data 5 pin
#define HD44780_D5_PORT     GPIOA
#define HD44780_D5_PIN      GPIO_PIN_5
//D6 - Data 6 pin
#define HD44780_D6_PORT     GPIOA
#define HD44780_D6_PIN      GPIO_PIN_6
//D7 - Data 7 pin
#define HD44780_D7_PORT     GPIOA
#define HD44780_D7_PIN      GPIO_PIN_7

#endif
