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

const char bobina_motor[] = "11000001";
int bobina_x = 0, bobina_y = 0, bobina_caneta = 0;
int posit_x=0, posit_y=0;

/*configura pinos dos motores*/
void motorInit(){
	GPIO_InitTypeDef config;

	config.GPIO_Pin = MOTOR_A_PIN1|MOTOR_A_PIN2|MOTOR_A_PIN3|MOTOR_A_PIN4|MOTOR_B_PIN1|MOTOR_B_PIN2|MOTOR_B_PIN3|MOTOR_B_PIN4;
	config.GPIO_Mode = GPIO_Mode_OUT;
	config.GPIO_Speed = GPIO_Speed_50MHz;
	config.GPIO_OType = GPIO_OType_PP;
	config.GPIO_PuPd = GPIO_PuPd_NOPULL;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	//liga clock barramento AHB1 GPIOE
	GPIO_Init(MOTOR_PORT, &config);							//inicializa pinos dos motores
}

/*configura pinos dos motores da caneta/drill*/
void canetaInit(){
	GPIO_InitTypeDef config;

	config.GPIO_Pin = CANETA_PIN1|CANETA_PIN2|CANETA_PIN3|CANETA_PIN4;
	config.GPIO_Mode = GPIO_Mode_OUT;
	config.GPIO_Speed = GPIO_Speed_50MHz;
	config.GPIO_OType = GPIO_OType_PP;
	config.GPIO_PuPd = GPIO_PuPd_NOPULL;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_Init(CANETA_PORT, &config);
}

/*configura pinos fim de curso*/
void endInit(){
	GPIO_InitTypeDef config;

	config.GPIO_Pin = END1|END2; //chave fim de curso
	config.GPIO_Mode = GPIO_Mode_IN;
	config.GPIO_Speed = GPIO_Speed_50MHz;
	config.GPIO_OType = GPIO_OType_PP;
	config.GPIO_PuPd = GPIO_PuPd_UP;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_Init(END_PORT, &config);
}

/*configura joystick*/
void joystickInit(){
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
	uint8_t customChar[] = {
        0x03,
        0x06,
        0x0C,
        0x1F,
        0x1F,
        0x0C,
        0x06,
        0x03
    };

	SystemInit();
	TM_HD44780_Init(16, 2);
    	TM_HD44780_CreateChar(0, &customChar[0]);
	TM_HD44780_Puts(0, 0, "Init sensores...");
	motorInit();
	canetaInit();
	endInit();
	joystickInit();
	buttonsInit();
	Delayms(250);
}

/*seleciona qual porta do ADC1 sera lida*/
uint16_t readADC(uint8_t channel){
	ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_480Cycles);
	ADC_SoftwareStartConv(ADC1);				//comeca conversao no pino channel
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);	//espera conversao acabar
	return ADC_GetConversionValue(ADC1);			//retorna valor lido
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
	TM_HD44780_Clear();
	TM_HD44780_Puts(0, 0, "Calibrando...");
	while(!(readEnd('x')&&readEnd('y'))){
		if(!readEnd('x')){
			counterclockwise('x');
		}
		if(!readEnd('y')){
			counterclockwise('y');
		}
	}
	//apos o ajuste de zero, volta um pouco para liberar as chaves
	for(i=15; i--;){
		clockwise('x');
		clockwise('y');
	}
	TM_HD44780_Clear();
	TM_HD44780_Puts(0, 0, "Mesa calibrada");
	Delayms(250);
	posit_x=0;
	posit_y=0;
}

/*ajusta altura da caneta/drill manualmente*/
void calibraCaneta(void){
	int b1, b2, digital=0;

	TM_HD44780_Clear();
	TM_HD44780_Puts(0, 0, "CALIBRE EIXO Z");
	TM_HD44780_Puts(0, 1, "Digital = OK");

	while(!digital){
    	digital = readJSButton();
    	b1 = readButtons('1');
    	b2 = readButtons('2');

    	if(b1){
    		runCaneta(1, '+');
    	}else if(b2){
    		runCaneta(1, '-');
    	}
	}
	TM_HD44780_Clear();
	runCaneta(300, '-');
}

/*controle manual via joystick e botoes*/
void manualControl(){
	int x, y, digital, b1, b2, end1, end2;

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
    		TM_HD44780_Puts(0, 1, "RESETE A MESA");
    		return;
    	}

    	if(x == 1||x == -1||y == 1||y == -1){
    		if(x==1){
    			runClockwise2(1, 'x');
    		}else if(x==-1){
    			runCounterclockwise2(1, 'x');
    		}
    		if(y==1){
    			runClockwise2(1, 'y');
    		}else if(y==-1){
    			runCounterclockwise2(1, 'y');
    		}
    	}

    	if(digital==1){
    		TM_HD44780_Clear();
    		TM_HD44780_Puts(0, 0, "MARCAR");
    		setFura();
    	}
    	if(b1==1){
    		TM_HD44780_Clear();
    		TM_HD44780_Puts(0, 0, "DESCER DRILL");
    		runCaneta(1, '+');
    	}
    	if(b2==1){
    		TM_HD44780_Clear();
    		TM_HD44780_Puts(0, 0, "SUBIR DRILL");
    		runCaneta(1, '-');
    	}
	}
}

/*movimenta a caneta*/
/* dir '+' = desce; dir '-' =  sobe*/
void runCaneta(int passos, char dir){
	int times = 0;

	if(dir == '+'){
		for(;times<passos;times++){
			bobina_caneta %= 8;
			if(bobina_motor[bobina_caneta] == '1'){
				GPIO_WriteBit(CANETA_PORT, CANETA_PIN1, Bit_SET);
			}else{
				GPIO_WriteBit(CANETA_PORT, CANETA_PIN1, Bit_RESET);
			}
			if(bobina_motor[(bobina_caneta+6)%8] == '1'){
				GPIO_WriteBit(CANETA_PORT, CANETA_PIN2, Bit_SET);
			}else{
				GPIO_WriteBit(CANETA_PORT, CANETA_PIN2, Bit_RESET);
			}
			if(bobina_motor[(bobina_caneta+4)%8] == '1'){
				GPIO_WriteBit(CANETA_PORT, CANETA_PIN3, Bit_SET);
			}else{
				GPIO_WriteBit(CANETA_PORT, CANETA_PIN3, Bit_RESET);
			}
			if(bobina_motor[(bobina_caneta+2)%8] == '1'){
				GPIO_WriteBit(CANETA_PORT, CANETA_PIN4, Bit_SET);
			}else{
				GPIO_WriteBit(CANETA_PORT, CANETA_PIN4, Bit_RESET);
			}
			bobina_caneta++;
			Delayms(CANETA_SPEED);
		}
	}else if(dir == '-'){
		for(;times<passos;times++){
			bobina_caneta %= 8;
			if(bobina_motor[bobina_caneta] == '1'){
				GPIO_WriteBit(CANETA_PORT, CANETA_PIN4, Bit_SET);
			}else{
				GPIO_WriteBit(CANETA_PORT, CANETA_PIN4, Bit_RESET);
			}
			if(bobina_motor[(bobina_caneta+6)%8] == '1'){
				GPIO_WriteBit(CANETA_PORT, CANETA_PIN3, Bit_SET);
			}else{
				GPIO_WriteBit(CANETA_PORT, CANETA_PIN3, Bit_RESET);
			}
			if(bobina_motor[(bobina_caneta+4)%8] == '1'){
				GPIO_WriteBit(CANETA_PORT, CANETA_PIN2, Bit_SET);
			}else{
				GPIO_WriteBit(CANETA_PORT, CANETA_PIN2, Bit_RESET);
			}
			if(bobina_motor[(bobina_caneta+2)%8] == '1'){
				GPIO_WriteBit(CANETA_PORT, CANETA_PIN1, Bit_SET);
			}else{
				GPIO_WriteBit(CANETA_PORT, CANETA_PIN1, Bit_RESET);
			}
			bobina_caneta++;
			Delayms(CANETA_SPEED);
		}
	}
}

/*marca um ponto com a caneta, abaixa e sobe*/
void setFura(void){
	runCaneta(301, '+');

	runClockwise(32, 'x');
	runClockwise(32, 'y');
	runCounterclockwise2(64, 'x');
	runCounterclockwise2(64, 'y');
	runClockwise(32, 'x');
	runClockwise(32, 'y');

	Delayms(2);
	runCaneta(300, '-');
}

/*abaixa em um passo a caneta*/
void setAbaixaCaneta(void){
	runCaneta(301, '+');
	Delayms(2);
}

/*sobe em um passo a caneta*/
void setSobeCaneta(void){
	runCaneta(300, '-');
	Delayms(2);
}

/*desenha um quadrado 3cm x 3 cm*/
void setDesenhaQuadrado(void){
	setAbaixaCaneta();
	moveParaXY(30, 0, 'm');
	moveParaXY(30, 30, 'm');
	moveParaXY(0, 30, 'm');
	moveParaXY(0, 0, 'm');
	setSobeCaneta();
}

/*dado dois arrays com coordenadas x e y, perfura todos os pontos*/
void setPerfura(float x[], float y[], int n){
	int i = 0;
	char str[20];

	sprintf(str, "%d FUROS", n-i);
	TM_HD44780_Clear();
	TM_HD44780_Puts(0, 0, str);
	TM_HD44780_Puts(0, 1, "RESTANTES");

	for(; i<n; i++){
		moveParaXY(x[i], y[i], 'm');
		setFura();
		sprintf(str, "%d FUROS", n-i);
		TM_HD44780_Clear();
		TM_HD44780_Puts(0, 0, str);
		TM_HD44780_Puts(0, 1, "RESTANTES");

	}
	TM_HD44780_Clear();
	TM_HD44780_Puts(0, 3, "PERFURACAO");
	TM_HD44780_Puts(0, 5, "COMPLETA");
}

/*menu interativo com o usuário com escolha de modo automático ou manual*/
/*APRIMORAR E TESTAR*/
void menu(void){
    int b1, b2, digital = 0;
    int menu = 0;

	while(!digital){
        digital = readJSButton();
        b1 = readButtons('1');
        b2 = readButtons('2');

        if(b1||b2){
        	menu++;
        	menu %= 2;
        }

        if(menu == 0){
            TM_HD44780_Clear();
            TM_HD44780_Puts(0, 0, "MODO AUTONOMO");
            TM_HD44780_PutCustom(15, 0, 0);
            TM_HD44780_Puts(0, 1, "MODO MANUAL");
        }else if(menu == 1){
            TM_HD44780_Clear();
            TM_HD44780_Puts(0, 0, "MODO AUTONOMO");
            TM_HD44780_Puts(0, 1, "MODO MANUAL");
            TM_HD44780_PutCustom(15, 1, 0);
        }
    TM_HD44780_Clear();
	}

	if(menu == 0){
		modoPerfuracao();
		//setDesenhaQuadrado();
	}else if(menu == 1){
		manualControl();
	}
}

/*movimenta a mesa no sentido positivo*/
void runClockwise(int passos, char eixo){
	int times = 0;

	if(eixo == 'x'){
		for(;times<passos;times++){
			bobina_x %= 8;
			if(bobina_motor[bobina_x] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_RESET);
			}
			if(bobina_motor[(bobina_x+6)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_RESET);
			}
			if(bobina_motor[(bobina_x+4)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_RESET);
			}
			if(bobina_motor[(bobina_x+2)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_RESET);
			}
			bobina_x++;
			posit_x++;
			Delayms(MOTOR_SPEED);
		}
	}else if(eixo == 'y'){
		for(;times<passos;times++){
			bobina_y %= 8;
			if(bobina_motor[bobina_y] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_RESET);
			}
			if(bobina_motor[(bobina_y+6)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_RESET);
			}
			if(bobina_motor[(bobina_y+4)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_RESET);
			}
			if(bobina_motor[(bobina_y+2)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_RESET);
			}
			bobina_y++;
			posit_y++;
			Delayms(MOTOR_SPEED);
		}

	}
}

/*movimenta a mesa no sentido positivo*/
/*atualiza posicao no display*/
void runClockwise2(int passos, char eixo){
	int times = 0;

	if(eixo == 'x'){
		for(;times<passos;times++){
			bobina_x %= 8;
			if(bobina_motor[bobina_x] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_RESET);
			}
			if(bobina_motor[(bobina_x+6)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_RESET);
			}
			if(bobina_motor[(bobina_x+4)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_RESET);
			}
			if(bobina_motor[(bobina_x+2)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_RESET);
			}
			bobina_x++;
			posit_x++;
			atualizaDisplayDistancia();
			Delayms(MOTOR_SPEED);
		}
	}else if(eixo == 'y'){
		for(;times<passos;times++){
			bobina_y %= 8;
			if(bobina_motor[bobina_y] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_RESET);
			}
			if(bobina_motor[(bobina_y+6)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_RESET);
			}
			if(bobina_motor[(bobina_y+4)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_RESET);
			}
			if(bobina_motor[(bobina_y+2)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_RESET);
			}
			bobina_y++;
			posit_y++;
			atualizaDisplayDistancia();
			Delayms(MOTOR_SPEED);
		}
	}	
}

/*movimenta a mesa no sentido negativo*/
void runCounterclockwise(int passos, char eixo){
	int times = 0;

	if(eixo == 'x'){
		for(;times<passos;times++){
			bobina_x %= 8;
			if(bobina_motor[bobina_x] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_RESET);
			}
			if(bobina_motor[(bobina_x+6)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_RESET);
			}
			if(bobina_motor[(bobina_x+4)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_RESET);
			}
			if(bobina_motor[(bobina_x+2)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_RESET);
			}
			bobina_x++;
			posit_x--;
			Delayms(MOTOR_SPEED);
		}
	}else if(eixo == 'y'){
		for(;times<passos;times++){
			bobina_y %= 8;
			if(bobina_motor[bobina_y] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_RESET);
			}
			if(bobina_motor[(bobina_y+6)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_RESET);
			}
			if(bobina_motor[(bobina_y+4)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_RESET);
			}
			if(bobina_motor[(bobina_y+2)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_RESET);
			}
			bobina_y++;
			posit_y--;
			Delayms(MOTOR_SPEED);
		}

	}
}

/*movimenta a mesa no sentido negativo*/
/*atualiza posicao no display*/
void runCounterclockwise2(int passos, char eixo){
	int times = 0;

	if(eixo == 'x'){
		for(;times<passos;times++){
			bobina_x %= 8;
			if(bobina_motor[bobina_x] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN4, Bit_RESET);
			}
			if(bobina_motor[(bobina_x+6)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN3, Bit_RESET);
			}
			if(bobina_motor[(bobina_x+4)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN2, Bit_RESET);
			}
			if(bobina_motor[(bobina_x+2)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_A_PIN1, Bit_RESET);
			}
			bobina_x++;
			posit_x--;
			atualizaDisplayDistancia();
			Delayms(MOTOR_SPEED);
		}
	}else if(eixo == 'y'){
		for(;times<passos;times++){
			bobina_y %= 8;
			if(bobina_motor[bobina_y] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN4, Bit_RESET);
			}
			if(bobina_motor[(bobina_y+6)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN3, Bit_RESET);
			}
			if(bobina_motor[(bobina_y+4)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN2, Bit_RESET);
			}
			if(bobina_motor[(bobina_y+2)%8] == '1'){
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_SET);
			}else{
				GPIO_WriteBit(MOTOR_PORT, MOTOR_B_PIN1, Bit_RESET);
			}
			bobina_y++;
			posit_y--;
			atualizaDisplayDistancia();
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

/*converte distância em mm ou pol para passos a serem andados*/
int convertePassos_Unidade(float distancia, char unidade){
	if(unidade == 'm'){
		distancia *= 1000;
		int passos = distancia/21.25;
		return passos;
	}else if(unidade == 'p'){
		return convertePassos_Unidade(25.40*distancia, 'm');
	}else{
		return 0;
	}
}

/*converte quantidade de passos para distância em mm ou pol*/
float converteUnidade_Passos(int passos, char unidade){
	if(unidade == 'm'){
		return passos*0.02125;
	}else if(unidade == 'p'){
		return passos*0.0008366;
	}else{
		return 0;
	}
}

/*move a mesa em uma distancia relativa em mm ou pol*/
/*atualiza no display a posição simultaneamente*/
/*'x' ou 'y' para escolher o eixo*/
/*'+" para clockwise e '-' para counterclockwise*/
void runDistancia_unidade(float distancia, char unidade, char eixo, char sentido){
	int passos = convertePassos_Unidade(distancia, unidade);
	if(sentido == '+'){
		runClockwise2(passos, eixo);
	}else if(sentido == '-'){
		runCounterclockwise2(passos, eixo);
	}
}

/*move a mesa para uma coordenada absoluta (x,y) em mm ou pol*/
void moveParaXY(float coord_x, float coord_y, char unidade){
	int abs_x = convertePassos_Unidade(coord_x, unidade);
	int abs_y = convertePassos_Unidade(coord_y, unidade);
	int passos_x, passos_y;
	if(abs_x > posit_x){
		passos_x = abs_x - posit_x;
		runDistancia_unidade(converteUnidade_Passos(passos_x, unidade), unidade, 'x', '+');
	}else{
		passos_x = posit_x - abs_x;
		runDistancia_unidade(converteUnidade_Passos(passos_x, unidade), unidade, 'x', '-');
	}
	if(abs_y > posit_y){
		passos_y = abs_y - posit_y;
		runDistancia_unidade(converteUnidade_Passos(passos_y, unidade), unidade, 'y', '+');
	}else{
		passos_y = posit_y - abs_y;
		runDistancia_unidade(converteUnidade_Passos(passos_y, unidade), unidade, 'y', '-');
	}
}

/*atualiza no display lcd a posição absoluta em mm*/
void atualizaDisplayDistancia(void){
	char px_mm[10], py_mm[10];

	ftoa(px_mm, posit_x*0.02125, 2, 0);
	ftoa(py_mm, posit_y*0.02125, 2, 0);

	TM_HD44780_Clear();
	TM_HD44780_Puts(0, 0, "x:");
	TM_HD44780_Puts(0, 1, "y:");
	TM_HD44780_Puts(9, 0, "mm");
	TM_HD44780_Puts(9, 1, "mm");
	TM_HD44780_Puts(3, 0, px_mm);
	TM_HD44780_Puts(3, 1, py_mm);
}

/*le uma string enviada pelo semihosting, a string deve ser terminada em '@' */
void readStringSH(char *str){
	int i = 0;
	do{
		str[i] = SH_GetChar();
		i++;
	}while(str[i-1] != '@');

	str[i-1] = '\0';
}

/*recebe um .drill interpretado em txt via semihosting e perfura os pontos*/
void modoPerfuracao(void){
	float x[100], y[100];
	int n = 0, i = 0;
	char str[10];
	readStringSH(str);

	while(str[0] != '\0'){
		x[i] = UB_String_DezStringToFloat(str);
		readStringSH(str);
		y[i] = UB_String_DezStringToFloat(str);
		readStringSH(str);
		i++;
	}

	for(;n<i;n++){
		ftoa(str, x[n], 4, 0);
		SH_SendString(str);
		SH_SendString("\n\r");
		ftoa(str, y[n], 4, 0);
		SH_SendString(str);
		SH_SendString("\n\r");
	}

	setPerfura(x, y, n);

	return;
}

/*funcao para converter float em string*/
void reverse(char* p, char* q){
	char c;
	for(; p < q; ++p, --q){
		c = *p;
		*p = *q;
		*q = c;
	}
}

/*funcao para converter float em string*/
char* inc(char* s, char* e){
	int co = 1;
	char* t = e;

	//increase from end to start
	for(; t >= s; --t){
		if(*t == '.') continue;
		*t += co;
		if(*t > '9'){
			*t = '0';
			co = 1;
		}
		else{
			co = 0;
			break;
		}
	}
	//check if there's still carry out
	if(co){
		for(t = ++e; t > s; --t) *t = *(t - 1);
		*s = '1';
	}
	return e;
}

/*funcao para converter float em string*/
char* ftoa(char* dest, double num, int afterPoint, int sign){
	char* p = dest;
	int integer = (int)num;
	double decimal = num - integer;

	//parse sign
	if(sign && num > 0) *p++ = '+';
	if(num < 0){
		*p++ = '-';
		integer = -integer;
		decimal = -decimal;
	}
	//parse integer
	if(integer){
		char* q = p;
		for(; integer; integer /= 10){
			*q++ = '0' + integer % 10;
		}
		reverse(p, q - 1);
		p = q;
	}
	else *p++ ='0';
	//parse decimal
	if(afterPoint > 0){
		*p++ = '.';
		for(; afterPoint; --afterPoint){
			decimal *= 10;
			*p++ = '0' + (int)decimal;
			decimal -= (int)decimal;
		}
		if((int)(decimal + 0.5)) p = inc(dest, p - 1) + 1;
	}

	*p = '\0';
	return dest;
}
