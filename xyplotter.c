//MOTOR_A_PIN1	4	PE7	
//MOTOR_A_PIN2	5	PE8	
//MOTOR_A_PIN3	6	PE9
//MOTOR_A_PIN4	7	PE10

//MOTOR_B_PIN1	8	PE11	
//MOTOR_B_PIN2	9	PE12	
//MOTOR_B_PIN3	10	PE13
//MOTOR_B_PIN4	11	PE14

//END1			12 	PB0
//END2			13 	PB1

//JOYSTICK_X	A1 	PC4
//JOYSTICK_Y	A0	PC5
//JSBUTTON 			PB2

//BUTTON1			PA6
//BUTTON2			PA7	

#include "xyplotter.h"

/*configura pinos dos motores*/
void motorInit(){
	//MOTOR_A_PIN1	PE7	(DIGITAL OUT)
	//MOTOR_A_PIN2	PE8	
	//MOTOR_A_PIN3	PE9
	//MOTOR_A_PIN4	PE10

	//MOTOR_B_PIN1	PE11	
	//MOTOR_B_PIN2	PE12	
	//MOTOR_B_PIN3	PE13
	//MOTOR_B_PIN4	PE14

	GPIO_InitTypeDef configPE;
	configPE.GPIO_Pin = MOTOR_A_PIN1|MOTOR_A_PIN2|MOTOR_A_PIN3|MOTOR_A_PIN4|MOTOR_B_PIN1|MOTOR_B_PIN2|MOTOR_B_PIN3|MOTOR_B_PIN4;
	configPE.GPIO_Mode = GPIO_Mode_OUT;
	configPE.GPIO_Speed = GPIO_Speed_50MHz;
	configPE.GPIO_OType = GPIO_OType_PP;
	configPE.GPIO_PuPd = GPIO_PuPd_NOPULL;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	//liga clock barramento AHB1 GPIOE
	GPIO_Init(MOTOR_PORT, &configPE);							//inicializa pinos dos motores
}

/*configura pinos fim de curso*/
void endInit(){
	//END1	 	PB0 (DIGITAL IN)
	//END2		PB1 (DIGITAL IN)

	GPIO_InitTypeDef configPB;
	configPB.GPIO_Pin = END1|END2;
	configPB.GPIO_Mode = GPIO_Mode_IN;
	configPB.GPIO_Speed = GPIO_Speed_50MHz;
	configPB.GPIO_OType = GPIO_OType_PP;
	configPB.GPIO_PuPd = GPIO_PuPd_NOPULL;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//liga clock barramento AHB1 GPIOB
	GPIO_Init(END_PORT, &configPB);							//inicializa pinos fim de curso
}

/*configura joystick*/
void joystickInit(){
	//JOYSTICK_X	PC4 (ADC)
	//JOYSTICK_Y	PC5	(ADC)
	//JOYSTICK_BUT	PB2	(DIGITAL IN)

	GPIO_InitTypeDef initPinADC;							//configura analógicos xy do joystick
	initPinADC.GPIO_Pin = JOYSTICK_X|JOYSTICK_Y;
	initPinADC.GPIO_Mode = GPIO_Mode_AN;
	initPinADC.GPIO_Speed = GPIO_Speed_50MHz;
	initPinADC.GPIO_OType = GPIO_OType_PP;
	initPinADC.GPIO_PuPd = GPIO_PuPd_NOPULL;

	ADC_CommonInitTypeDef commonADC;
	commonADC.ADC_Mode = ADC_Mode_Independent;
	commonADC.ADC_Prescaler = ADC_Prescaler_Div2;
	commonADC.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	commonADC.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;

	ADC_InitTypeDef initADC;
	initADC.ADC_Resolution = ADC_Resolution_12b;
	initADC.ADC_ScanConvMode = DISABLE;
	initADC.ADC_ContinuousConvMode = ENABLE;
	initADC.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	initADC.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	initADC.ADC_DataAlign = ADC_DataAlign_Right;
	initADC.ADC_NbrOfConversion = 1;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	GPIO_Init(JOYSTICK_PORT, &initPinADC);
	ADC_CommonInit(&commonADC);
	ADC_StructInit(&initADC);
	ADC_Init(ADC1, &initADC);
	ADC_EOCOnEachRegularChannelCmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

	GPIO_InitTypeDef configPB;								//configura pino botão digital joystick
	configPB.GPIO_Pin = JSBUTTON;
	configPB.GPIO_Mode = GPIO_Mode_IN;
	configPB.GPIO_Speed = GPIO_Speed_50MHz;
	configPB.GPIO_OType = GPIO_OType_PP;
	configPB.GPIO_PuPd = GPIO_PuPd_UP;						//habilita resistor PULL-UP

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_Init(JSBUTTON_PORT, &configPB);
}

/*configura pushbuttons*/
void buttonsInit(){
	//BUTTON_1	PA6
	//BUTTON_2	PA7

	GPIO_InitTypeDef configPA;
	configPA.GPIO_Pin = BUTTON1|BUTTON2;
	configPA.GPIO_Mode = GPIO_Mode_IN;
	configPA.GPIO_Speed = GPIO_Speed_50MHz;
	configPA.GPIO_OType = GPIO_OType_PP;
	configPA.GPIO_PuPd = GPIO_PuPd_DOWN;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_Init(BUTTON_PORT, &configPA);
}

/*seleciona qual porta do ADC1 será lida*/
uint16_t readADC(uint8_t channel){
  ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_480Cycles);
  ADC_SoftwareStartConv(ADC1, ENABLE);						//Começa conversao no pino channel
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == Bit_RESET);	//Espera conversao acabar
  return ADC_GetConversionValue(ADC1);						//retorna valor lido
}

/*lê uma porta digital*/
int readDigitalPin(uint8_t gpio, uint8_t pin){
	if(GPIO_ReadInputDataBit(gpio, pin) == Bit_SET){
		return 1;
	}else{
		return 0;
	}
}

/*lê o AD de um dos eixos do joystick*/
uint16_t readJoystick(char eixo){
	if(eixo == 'x'){
		return readADC(ADC_Channel_14);	//Joystick X está ligado no canal 14 do ADC1
	}else if(eixo == 'y'){
		return readADC(ADC_Channel_15); //Joystick Y está ligado no canal 15 do ADC1
	}else{
		return 9999999;
	}
}

/*discretiza a posicao do joystick*/
int posicaoJoystick(char eixo){
	uint16_t valor;

	if(eixo == 'x'){
		valor = readJoystick('x');
	}else if(eixo == 'y'){
		valor = readJoystick('y');
	}

	if(valor>3072){
		return 1;
	}else if(valor<1024){
		return -1;
	}else{
		return 0;
	}
}

/*le o botão do joystick*/
int readJSButton(){
	return (!readDigitalPin(JSBUTTON_PORT, JSBUTTON));
	//quando pressionado lê zero, mas retorna 1
}

/*controle manual via joystick*/
void manualControl(){
	int x, y;

	while(1){
		x = posicaoJoystick('x');
		y = posicaoJoystick('y');

		if(x==1){
			clockwise('x');
		}else if(x==-1){
			counterclockwise('x');
		}else if(y==1){
			clockwise('y');
		}else if(y==-1){
			counterclockwise('y');
		}

		if(readJSButton()){
			return;
		}
	}	//loop eterno até que seja apertado o botão do JS
}

//lê fim de curso*/
int readEnd(char eixo){
	if(eixo == 'x'){
		return readDigitalPin(END_PORT, END1);
	}else if(eixo == 'y'){
		return readDigitalPin(END_PORT, END2);
	}
}

/*le pushbuttons*/
int readButtons(char button){
	if(button == '1'){
		return readDigitalPin(BUTTON_PORT, BUTTON1);
	}else if(button == '2'){
		return readDigitalPin(BUTTON_PORT, BUTTON2);
	}
}

void delay(double time){
	for(;time--;);
}

void counterclockwise(char eixo){
	if(eixo == 'x'){	//eixo x = mesa de baixo
		// 1
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_RESET);
		delay(MOTOR_SPEED);
		// 2
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_RESET);
		delay(MOTOR_SPEED);
		// 3
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_RESET);
		delay(MOTOR_SPEED);
		// 4
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_RESET);
		delay(MOTOR_SPEED);
		// 5
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_RESET);
		delay(MOTOR_SPEED);
		// 6
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_SET);
		delay(MOTOR_SPEED);
		// 7
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_SET);
		delay(MOTOR_SPEED);
		// 8
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_SET);
		delay(MOTOR_SPEED);
	}else if(eixo == 'y'){	//eixo y = mesa de cima
		// 1
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_RESET);
		delay(MOTOR_SPEED);
		// 2
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_RESET);
		delay(MOTOR_SPEED);
		// 3
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_RESET);
		delay(MOTOR_SPEED);
		// 4
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_RESET);
		delay(MOTOR_SPEED);
		// 5
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_RESET);
		delay(MOTOR_SPEED);
		// 6
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_SET);
		delay(MOTOR_SPEED);
		// 7
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_SET);
		delay(MOTOR_SPEED);
		// 8
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_SET);
		delay(MOTOR_SPEED);		
	}
}

void clockwise(char eixo){
	if(eixo == 'x'){
		// 1
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_RESET);
		delay(MOTOR_SPEED);
		// 2
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_RESET);
		delay(MOTOR_SPEED);
		// 3
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_RESET);
		delay(MOTOR_SPEED);
		// 4
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_RESET);
		delay(MOTOR_SPEED);
		// 5
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_RESET);
		delay(MOTOR_SPEED);
		// 6
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_SET);
		delay(MOTOR_SPEED);
		// 7
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_SET);
		delay(MOTOR_SPEED);
		// 8
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_SET);
		delay(MOTOR_SPEED);
	}else if(eixo == 'y'){
		// 1
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_RESET);
		delay(MOTOR_SPEED);
		// 2
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_RESET);
		delay(MOTOR_SPEED);
		// 3
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_RESET);
		delay(MOTOR_SPEED);
		// 4
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_RESET);
		delay(MOTOR_SPEED);
		// 5
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_RESET);
		delay(MOTOR_SPEED);
		// 6
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_SET);
		delay(MOTOR_SPEED);
		// 7
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_SET);
		delay(MOTOR_SPEED);
		// 8
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_SET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_RESET);
		GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_SET);
		delay(MOTOR_SPEED);		
	}	
}

/*busca (filtro) e retorna a distancia crua*/
int getDistancia(){
	int distancia = 0;
	return distancia;
}

/*busca (filtro) e retorna tipo da unidade (polegada ou milimetro)*/
char getUnidade(){
	char unidade = ' ';
	return unidade;
}

/*converte distancia em milimetro para passos*/
int converteMilimetro(){
	int distancia = PASSOS_mm*getDistancia();
	return distancia;
}

/*converte distancia em polegada para passos*/
int convertePolegada(){
	int distancia = PASSOS_pol*getDistancia();
	return distancia;
}

/*recebe a unidade (getUnidade) e converte para numero de passos utilizando runMotor()*/
void setPassos(char direction){         //NAO SEI COMO DEFINIR ISSO TEM QUE SER ALGO DO GENERO (ATUAL - NOVO)
	if(getUnidade() == 'm'){			//caso seja em milimetros
		runMotor(converteMilimetro(), direction);
	}else if(getUnidade() == 'p'){
		runMotor(convertePolegada(), direction);
	}
}

/*recebe o numero de passos e chama countercw ou cw de acordo com o sentido(NAO PENSEI EM COMO DIFERENCIAR AINDA!!!!!!!!!!)*/
void runMotor(int passos, char direction){
	int i = passos;	
	if(direction == 'n'){	
		for(;i--;){
			clockwise('y');
		}
	}else if(direction == 's'){
		for(;i--;){
			counterclockwise('y');
		}
	}else if(direction == 'l'){
		for(;i--;){
			clockwise('x');
		}
	}else if(direction == 'o'){
			counterclockwise('x');
	}
}
