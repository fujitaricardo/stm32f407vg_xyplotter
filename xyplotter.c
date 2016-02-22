//MOTOR_A_PIN1	4	PE7
//MOTOR_A_PIN2	5	PE9
//MOTOR_A_PIN3	6	PE11
//MOTOR_A_PIN4	7	PE13

//MOTOR_B_PIN1	8	PE8
//MOTOR_B_PIN2	9	PE10
//MOTOR_B_PIN3	10	PE12
//MOTOR_B_PIN4	11	PE14

//END1			12 	PB11
//END2			13 	PB12

//JOYSTICK_X	A1 	PC4
//JOYSTICK_Y	A0	PC5
//JSBUTTON 			PC1

//BUTTON1			PC2
//BUTTON2			PC3

//1 passo = 0,17mm

#include "xyplotter.h"

const char bobina_motor[] = "11000001";
int posit_bobina_x = 0, posit_bobina_y = 0;

/*configura pinos dos motores*/
void motorInit(){
	//MOTOR_A_PIN1	PE7
	//MOTOR_A_PIN2	PE9
	//MOTOR_A_PIN3	PE11
	//MOTOR_A_PIN4	PE13

	//MOTOR_B_PIN1	PE8
	//MOTOR_B_PIN2	PE10
	//MOTOR_B_PIN3	PE12
	//MOTOR_B_PIN4	PE14

	GPIO_InitTypeDef config;

	config.GPIO_Pin = MOTOR_A_PIN1|MOTOR_A_PIN2|MOTOR_A_PIN3|MOTOR_A_PIN4|MOTOR_B_PIN1|MOTOR_B_PIN2|MOTOR_B_PIN3|MOTOR_B_PIN4;
	config.GPIO_Mode = GPIO_Mode_OUT;
	config.GPIO_Speed = GPIO_Speed_50MHz;
	config.GPIO_OType = GPIO_OType_PP;
	config.GPIO_PuPd = GPIO_PuPd_NOPULL;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	//liga clock barramento AHB1 GPIOE
	GPIO_Init(MOTOR_PORT, &config);							//inicializa pinos dos motores
}

/*configura pinos fim de curso*/
void endInit(){
	//END1 	PB11
	//END2	PB12

	GPIO_InitTypeDef config;

	config.GPIO_Pin = END1|END2; //chave fim de curso
	config.GPIO_Mode = GPIO_Mode_IN;
	config.GPIO_Speed = GPIO_Speed_50MHz;
	config.GPIO_OType = GPIO_OType_PP;
	config.GPIO_PuPd = GPIO_PuPd_UP;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//liga clock barramento AHB1 GPIOB
	GPIO_Init(END_PORT, &config);
}

/*configura joystick*/
void joystickInit(){
	//JOYSTICK_X	PC4 (ADC)
	//JOYSTICK_Y	PC5	(ADC)
	//JOYSTICK_BUT	PC1	(DIGITAL IN)

	GPIO_InitTypeDef initPinADC;	//configura analogicos xy do joystick
	ADC_CommonInitTypeDef commonADC;
	ADC_InitTypeDef initADC;
	GPIO_InitTypeDef config;		//configura pino botao digital joystick

	initPinADC.GPIO_Pin = JOYSTICK_X|JOYSTICK_Y;
	initPinADC.GPIO_Mode = GPIO_Mode_AN;
	initPinADC.GPIO_Speed = GPIO_Speed_50MHz;
	initPinADC.GPIO_OType = GPIO_OType_PP;
	initPinADC.GPIO_PuPd = GPIO_PuPd_NOPULL;

	commonADC.ADC_Mode = ADC_Mode_Independent;
	commonADC.ADC_Prescaler = ADC_Prescaler_Div2;
	commonADC.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	commonADC.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;

	initADC.ADC_Resolution = ADC_Resolution_12b;
	initADC.ADC_ScanConvMode = DISABLE;
	initADC.ADC_ContinuousConvMode = ENABLE;
	initADC.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	initADC.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	initADC.ADC_DataAlign = ADC_DataAlign_Right;
	initADC.ADC_NbrOfConversion = 1;

	config.GPIO_Pin = JSBUTTON;
	config.GPIO_Mode = GPIO_Mode_IN;
	config.GPIO_Speed = GPIO_Speed_50MHz;
	config.GPIO_OType = GPIO_OType_PP;
	config.GPIO_PuPd = GPIO_PuPd_UP;	//habilita resistor PULL-UP

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	GPIO_Init(JOYSTICK_PORT, &initPinADC);	//analog joystick
	GPIO_Init(JSBUTTON_PORT, &config);		//digital joystick
	ADC_CommonInit(&commonADC);
	ADC_StructInit(&initADC);
	ADC_Init(ADC1, &initADC);
	ADC_EOCOnEachRegularChannelCmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
}

/*configura pushbuttons*/
void buttonsInit(){
	//BUTTON_1	PC2
	//BUTTON_2	PC3

	GPIO_InitTypeDef config;

	config.GPIO_Pin = BUTTON1|BUTTON2;
	config.GPIO_Mode = GPIO_Mode_IN;
	config.GPIO_Speed = GPIO_Speed_50MHz;
	config.GPIO_OType = GPIO_OType_PP;
	config.GPIO_PuPd = GPIO_PuPd_DOWN;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_Init(BUTTON_PORT, &config);
}

/*inicializa todos os sensores da mesa xy*/
void xyPlotterInit(){
	TM_HD44780_Puts(0, 0, "Init sensores");
	motorInit();
	endInit();
	joystickInit();
	buttonsInit();
}

/*seleciona qual porta do ADC1 sera lida*/
uint16_t readADC(uint8_t channel){
	ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_480Cycles);
	ADC_SoftwareStartConv(ADC1);								//comeca conversao no pino channel
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);		//espera conversao acabar
	return ADC_GetConversionValue(ADC1);						//retorna valor lido
}

/*le o AD de um dos eixos analogicos do joystick*/
uint16_t readJoystick(char eixo){
	if(eixo == 'x'){
		return readADC(ADC_Channel_14);	//joystick X esta ligado no canal 14 do ADC1
	}else if(eixo == 'y'){
		return readADC(ADC_Channel_15); //joystick Y esta ligado no canal 15 do ADC1
	}else{
		return 9999;					//eixo invalido
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
		return 1;	//sentido positivo
	}else if(valor<1024){
		return -1;	//sentido negativo
	}else{
		return 0;	//nao pressionado
	}
}

/*le o botao digital do joystick*/
int readJSButton(){
	//quando pressionado le zero, mas retorna 1
	//ligado com resistor de pull-up
	if(!GPIO_ReadInputDataBit(JSBUTTON_PORT, JSBUTTON)){
		return 1;
	}else{
		return 0;
	}
}

//le fim de curso*/
int readEnd(char eixo){
	//ligado com resistor de pull-up
	if(eixo == 'x'){
		if(GPIO_ReadInputDataBit(END_PORT, END1) == Bit_SET){
			return 0;
		}else{
			return 1;
		}
	}else if(eixo == 'y'){
		if(GPIO_ReadInputDataBit(END_PORT, END2) == Bit_SET){
			return 0;
		}else{
			return 1;
		}
	}else{
		return 9999;	//eixo invalido
	}
}

/*le pushbuttons*/
int readButtons(char button){
	//ligado com resistor de pull-down
	if(button == '1'){
		if(GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON1) == Bit_SET){
			return 1;
		}else{
			return 0;
		}
	}else if(button == '2'){
		if(GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON2) == Bit_SET){
			return 1;
		}else{
			return 0;
		}
	}else{
		return 9999;	//botao invalido
	}
}

/*ajusta a posicao zero da mesa xy*/
void calibraZero(){
	//move a mesa ate atingir as duas chaves de fim de curso
	int i;
	while(!(readEnd('x')&&readEnd('y'))){
		counterclockwise('x');
		counterclockwise('y');
	}
	//apos o ajuste de zero, volta um pouco para liberar as chaves
	for(i=10; i--;){
		clockwise('x');
		clockwise('y');
	}
	return;
}

/*controle manual via joystick e botoes*/
void manualControl(){
	int x, y, digital, b1, b2, end1, end2;
	uint16_t _x=0, _y=0;
	char px[10], py[10];

	while(1){
		x = posicaoJoystick('x');
		y = posicaoJoystick('y');
    	digital = readJSButton();
    	b1 = readButtons('1');
    	b2 = readButtons('2');
    	end1 = readEnd('x');
    	end2 = readEnd('y');

    	if(end1||end2){
    		TM_HD44780_Clear();
    		TM_HD44780_Puts(0, 0, "FIM DE CURSO");
    		return;
    	}

    	if(x == 1||x == -1||y == 1||y == -1){
    		if(x==1){
    			clockwise('x');
    			_x++;
    		}else if(x==-1){
    			counterclockwise('x');
   				_x--;
    		}
    		if(y==1){
    			clockwise('y');
    			_y++;
    		}else if(y==-1){
    			counterclockwise('y');
    			_y--;
    		}
    		sprintf(px, "x:%d", _x);
    		sprintf(py, "y:%d", _y);
    		TM_HD44780_Clear();
    		TM_HD44780_Puts(0, 0, px);
    		TM_HD44780_Puts(0, 1, py);
    	}
    	if(digital==1){
    		TM_HD44780_Clear();
    		TM_HD44780_Puts(0, 0, "DRILL ON/OFF");
    	}
    	if(b1==1){
    		TM_HD44780_Clear();
    		TM_HD44780_Puts(0, 0, "DESCER DRILL");
    	}
    	if(b2==1){
    		TM_HD44780_Clear();
    		TM_HD44780_Puts(0, 0, "SUBIR DRILL");
    	}
	}
}

/*movimenta a mesa no sentido positivo*/
void runClockwise(int passos, char eixo){
	int times = 0;

	if(eixo == 'x'){
		for(;times<passos;times++){
			posit_bobina_x %= 8;
			if(bobina_motor[posit_bobina_x] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_RESET);
			}
			if(bobina_motor[(posit_bobina_x+6)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_RESET);
			}
			if(bobina_motor[(posit_bobina_x+4)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_RESET);
			}
			if(bobina_motor[(posit_bobina_x+2)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_RESET);
			}
			posit_bobina_x++;
			Delayms(MOTOR_SPEED);
		}
	}else if(eixo == 'y'){
		for(;times<passos;times++){
			posit_bobina_y %= 8;
			if(bobina_motor[posit_bobina_y] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_RESET);
			}
			if(bobina_motor[(posit_bobina_y+6)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_RESET);
			}
			if(bobina_motor[(posit_bobina_y+4)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_RESET);
			}
			if(bobina_motor[(posit_bobina_y+2)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_RESET);
			}
			posit_bobina_y++;
			Delayms(MOTOR_SPEED);
		}

	}
}

/*movimenta a mesa no sentido negativo*/
void runCounterclockwise(int passos, char eixo){
	int times = 0;

	if(eixo == 'x'){
		for(;times<passos;times++){
			posit_bobina_x %= 8;
			if(bobina_motor[posit_bobina_x] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_RESET);
			}
			if(bobina_motor[(posit_bobina_x+6)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_RESET);
			}
			if(bobina_motor[(posit_bobina_x+4)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_RESET);
			}
			if(bobina_motor[(posit_bobina_x+2)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_RESET);
			}
			posit_bobina_x++;
			Delayms(MOTOR_SPEED);
		}
	}else if(eixo == 'y'){
		for(;times<passos;times++){
			posit_bobina_y %= 8;
			if(bobina_motor[posit_bobina_y] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_RESET);
			}
			if(bobina_motor[(posit_bobina_y+6)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_RESET);
			}
			if(bobina_motor[(posit_bobina_y+4)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_RESET);
			}
			if(bobina_motor[(posit_bobina_y+2)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_RESET);
			}
			posit_bobina_y++;
			Delayms(MOTOR_SPEED);
		}

	}
}

/*passo completo no sentido positivo*/
void clockwise(char eixo){
	runClockwise(8, eixo);
}

/*passo completo no sentido negativo*/
void counterclockwise(char eixo){
	runCounterclockwise(8, eixo);
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
