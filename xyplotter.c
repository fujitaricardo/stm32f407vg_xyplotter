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
int posit_x=0, posit_y=0;
char px_mm[10], py_mm[10];

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
	TM_HD44780_Puts(0, 0, "Init sensores...");
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
	for(i=10; i--;){
		clockwise('x');
		clockwise('y');
	}
	TM_HD44780_Clear();
	TM_HD44780_Puts(0, 0, "Controle manual");
	return;
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
    		return;
    	}

    	if(x == 1||x == -1||y == 1||y == -1){
    		if(x==1){
    			clockwise('x');
    		}else if(x==-1){
    			counterclockwise('x');
    		}
    		if(y==1){
    			clockwise('y');
    		}else if(y==-1){
    			counterclockwise('y');
    		}
    		/*sprintf(px, "x:%d", _x);
    		sprintf(py, "y:%d", _y);
    		TM_HD44780_Clear();
    		TM_HD44780_Puts(0, 0, px);
    		TM_HD44780_Puts(0, 1, py);*/
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
			posit_x++;
			atualizaDisplayDistancia();
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
			posit_x--;
			atualizaDisplayDistancia();
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
/*'x' ou 'y' para escolher o eixo*/
/*'+" para clockwise e '-' para counterclockwise*/
void runDistancia_unidade(float distancia, char unidade, char eixo, char sentido){
	int passos = convertePassos_Unidade(distancia, unidade);
	if(sentido == '+'){
		runClockwise(passos, eixo);
	}else if(sentido == '-'){
		runCounterclockwise(passos, eixo);
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
	ftoa(px_mm, posit_x*0.02125, 5, 0);
	ftoa(py_mm, posit_y*0.02125, 5, 0);

	TM_HD44780_Clear();
	TM_HD44780_Puts(0, 0, "x:");
	TM_HD44780_Puts(0, 1, "y:");
	TM_HD44780_Puts(12, 0, "mm");
	TM_HD44780_Puts(12, 1, "mm");
	TM_HD44780_Puts(3, 0, px_mm);
	TM_HD44780_Puts(3, 1, py_mm);
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
