#include <REGX52.H>

//silnik

#define IN1 P2_1// nie wolno srednikow
#define IN2 P2_2
#define EN1 P2_3
#define EN2 P2_4// nie wolno srednikow
#define IN3 P2_5
#define IN4 P2_6//dalej mo|na przepiac przykladowo na P1 i tylko define zmieniamy
#define Max 0xFF0

//suwak sie zmienia w przerwaniu 
volatile unsigned char data Suwak;// inicjowanie tu kosztuje
volatile unsigned char data Suwak2;// inicjowanie tu kosztuje
volatile unsigned char data Nastawa;
volatile unsigned char data Nastawa2;
int data LiczbaObrotowSilnika;
int data LiczbaObrotowSilnika2;
int data LiczbaObrotowSilnikaDoZmiany;
bit zmianaStanu;
char predkosc1 = 40; //191
char predkosc2 = 30;	//170
char predkosc3 = 51;	//51
char predkosc4 = 20;	//102
bit wlaczono;
bit krecenie;	//jesli 1 to nie reaguj na sygnaly
bit rozladowano;// rozladowanie baterii
char odebrano;
void Send(unsigned char Value)//cala sekwencja nadawania
{    
	ES = 0;
	P3_4 = 1;
	TI=0;//TRANSMIT INTERRUPT, przyszykuj nadajnik
	SBUF = Value;//serial buffer
	
	while (TI == 0){;}
	P3_4 = 0;//przechodzisz w tryb nasluchiwania
	ES = 1;
}//RI receiver interrupt
void ISR_Serial(void) interrupt 4
{
	
	if(TI==1) {TI=0;}
	if(RI==1)
	{
		
		odebrano = SBUF;
		if (odebrano == '#' && wlaczono == 1 && krecenie == 0)// wylaczenie maszyny
		{
			krecenie = 1;
			wlaczono = 0;
		  Nastawa2 = predkosc3; // 0xFF;//0x40; max do 0x01
		}
		else if (odebrano == '#' && wlaczono == 0 && krecenie == 0)// wlaczenie maszyny
		{
			krecenie = 1;
			wlaczono = 1;
			Nastawa = predkosc4; // 0xFF;//0x40; max do 0x01
		}
		else if (odebrano == 'R')// wlaczenie maszyny
		{
			rozladowano = 1;
		}
		
		RI=0;
	}

	//TI = 1;
}
void SerialInit(void)//inicjacja portu szeregowego
{
	P3_4 = 0;
	
	TCLK=1;
	RCLK=1;
	SCON=0x50;
	T2CON = 0x30;
	TH2=RCAP2H=0xFF;
	TL2=RCAP2L=0xDC;
	
	IP=0x10; //podwyzszenie priorytetu portu szeregowego!!!!!! //PROBLEM
	ES=1;
	EA  = 1;
	TR2=1;
}
void Setup(void)
{
		krecenie = 0;
		wlaczono = 0;
		zmianaStanu = 0;
		LiczbaObrotowSilnikaDoZmiany = 1500;	//na sekunde 1500
		LiczbaObrotowSilnika = 0;
		LiczbaObrotowSilnika2 = 0;
		//	1/100 predkosc mas
		//Nastawa = 0 wylaczony silnik reguluje predkosc obrotu
		//Nastawa = predkosc1; // 0xFF;//0x40; max do 0x01
		//Nastawa2 = predkosc2; // 0xFF;//0x40; max do 0x01
    EN2 = 1;
		EN1 = 1;
    if(Nastawa == 0x00) {EN2=0;}
		else if(Nastawa == Max) {EN2=1;}
		else {EN2=1;}
    P3_4=0;
    Suwak = 0x00;
    Suwak2 = 0x00;
		//obrot w prawo 2 silnik
    IN4 = 0;
		IN3 = 1;
		//obrot w lewo 1 silnik
		IN1 = 0;
		IN2 = 1;
		//IN3 = 0 obrot w lewo
		//TMOD = 0x20 oba w 8bit z autoreload
    //timer tryb 2 - 8bit z autoreload
		TCON = TCON & 0x0F;
		TCON = TCON | 0x00;
    TMOD= TMOD & 0x0F;
		TMOD= TMOD | 0x22;
		//TMOD = 0x11; tryb 1 dla timer 0 i timer 1
		//predkosc obrotu
    TH1=TL1=0xF0;		//=FF mniejsze odchylenie od sredniej predkosci
    ET1=1; //przerwanie timer 1
    EA=1;	//wlacz przerwania
    TR1=1; //zawsze po EA
		//wlacz przerwania timer 0
		ET0 = 1;
		TR0 = 1;
		//przerwania INT
		//przerwania INT1 - Silnik Gorny
		//przerwania INT0 - Silnik Dolny
		EX0 = 1;
		IT0 = 1;
		EX1 = 1;
		IT1 = 1;
}
//silnik gorny
void ISR_INT1(void) interrupt 2 {

	LiczbaObrotowSilnika++;
	if (wlaczono == 1 && krecenie == 1) {
		Send('S');	//Do LCD
		Nastawa = 0;
		Nastawa2 = 0;
		krecenie = 0;
	}
	zmianaStanu = 1;
	//zmien tryb timera
	TMOD= 0x22;
}
//silnik dolny
void ISR_INT0(void) interrupt 0 {
	if (wlaczono == 0 && krecenie == 1) {
		Send('S');	//Do LCD
		krecenie = 0;
		Nastawa = 0;
		Nastawa2 = 0;
	}
	LiczbaObrotowSilnika2++;
	zmianaStanu = 1;
	//zmien tryb timera
	TMOD= 0x22;
}
//silnik 2 Dolny
void ISR_TIMER0(void) interrupt 1
{
	if (rozladowano == 1)
	return;
	Suwak2++;
	if (Nastawa2 == 0) {EN2 = 0;}
	else {
		if (Nastawa2 == Max) {EN2 = 1;}
		else {
			if (Suwak2 < Nastawa2) {EN2 = 1;}
			else {
				if (Suwak2 < Max) {EN2 = 0;}
				else {
					Suwak2 = 0; EN2 = 1;
				}
			}
		}
	}
	if (TMOD == 0x11) {
		TH0 = 0xFF;
		TL0 = 0xF0;
	}
}
//silnik 1 Gorny
void ISR_PWM(void) interrupt 3
{
	if (rozladowano == 1)
	return;
	Suwak++;
	if (Nastawa == 0) {EN1 = 0;}
	else {
		if (Nastawa == Max) {EN1 = 1;}
		else {
			if (Suwak < Nastawa) {EN1 = 1;}
			else {
				if (Suwak < Max) {EN1 = 0;}
				else {
					Suwak = 0; EN1 = 1;
				}
			}
		}
	}
	if (TMOD == 0x11) {
		TH1 = 0xFF;
		TL1 = 0xF0;
	}
}

void main(void)
{
	
	Setup();
	SerialInit();
	for(;;){
		;
	}
			//w debuger bit nie analog


}