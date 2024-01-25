#include <REGX52.H>
unsigned char xdata Ones _at_ 0xFD00;
unsigned char xdata Tens _at_ 0xFE00;

void Send(unsigned char Value);

bit wlaczono;
char odebrano;
char z;
char a;
char wcisniety1;
char wyswietl;
char wyswietl2;
char charH = 0x76;
char charU = 0x3E;
char charB = 0x7C;
char charE = 0x79;
char charR = 0x50;
char charT = 0x78;
char charM = 0x15;
char charA = 0x77;
char charK = 0x76;
char charO = 0x3F;
char charW = 0x1C;
char charS = 0x6D;
char charI = 0x30;
char Jeden = 0x06;
char Dwa = 0x5B;
char Trzy = 0x4F;
char Cztery = 0x66;
char Piec = 0x6D;
char Szesc = 0x7D;
char Siedem = 0x07;
char Osiem = 0x7F;
char Dziewiec = 0x6F;
char Zero = 0x3F;
char charC = 0x39;
char charD = 0x5E;
char charF = 0x71;
char LicznikSerial;
char Imie[] = {0x76, 0x3E, 0x7C, 0x79, 0x50, 0x78, 0x15, 0x77, 0x76, 0x3F, 0x1C, 0x6D, 0x30};
char Pasmo[3] = {64, 64, 64};
char Pasmo2[6] = {64, 64, 64, 64, 64, 64};
char DlugoscD = 10 - 1;
char flagaSerial;
char glosnosc;
char glosnoscTimer;
bit wlaczonaGlosnosc;
bit rozladowano;
void timerInit(void) {
	//timer
	//tryb timera
	TMOD = TMOD | 0x01;
	TCON = TCON | 0x10;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
	
}
void Send(unsigned char Value)//cala sekwencja nadawania
{    
	
	P3_4 = 1;
	ES = 0;
	TI=0;//TRANSMIT INTERRUPT, przyszykuj nadajnik
	SBUF = Value;//serial buffer
	
	while (TI == 0){;}
	//TI=0;//dla porzadku
	ES = 1;
	P3_4 = 0;
	
	
}//RI receiver interrupt
//zwroc czesc dziesietna
char Konwertuj1(char system, char liczba) {
	return liczba/system;
}
//zwroc czesc jednosci
char Konwertuj2(char system, char liczba) {
	return liczba%system;
}
char Konwertuj(char znak) {
	if (znak == 1) {
		return Jeden;
	}
	else if (znak == 2) {
		return Dwa;
	}
	else if (znak == 3) {
		return Trzy;
	}
	else if (znak == 4) {
		return Cztery;
	}
	else if (znak == 5) {
		return Piec;
	}
	else if (znak == 6) {
		return Szesc;
	}
	else if (znak == 7) {
		return Siedem;
	}
	else if (znak == 8) {
		return Osiem;
	}
	else if (znak == 9) {
		return Dziewiec;
	}
	else if (znak == 0) {
		return Zero;
	}
	return 64;
}
//dla ascii
char KonwertujAscii(char znak) {
	if (znak == '1') {
		return Jeden;
	}
	else if (znak == '2') {
		return Dwa;
	}
	else if (znak == '3') {
		return Trzy;
	}
	else if (znak == '4') {
		return Cztery;
	}
	else if (znak == '5') {
		return Piec;
	}
	else if (znak == '6') {
		return Szesc;
	}
	else if (znak == '7') {
		return Siedem;
	}
	else if (znak == '8') {
		return Osiem;
	}
	else if (znak == '9') {
		return Dziewiec;
	}
	else if (znak == '0') {
		return Zero;
	}
	return 64;
}
void ISR_T0() interrupt 1{
	if (rozladowano == 1)
	return;
	a++;
	if (a == 5)
		a = 0;
	glosnoscTimer++;
	if (glosnoscTimer % 6 == 0) {
		glosnoscTimer = 0;
		wlaczonaGlosnosc = 0;
	}
	TH0 = 0x00;
	TL0 = 0x00;
}
void ISR_TIMER1() interrupt 3{
	//a++;
}	
void ISR_INT0() interrupt 0{
	if (rozladowano == 1)
	return;
	while(P3_2 == 0) {;}
	if (wlaczono == 1) {
		if (glosnosc < 15) {
			glosnosc++;
			Send('G');
		}
		if (glosnosc < 10) {
			wyswietl = Konwertuj(glosnosc);
			wyswietl2 = 0;
		}
		else {
			wyswietl2 = Konwertuj(Konwertuj1(10, glosnosc));
			wyswietl = Konwertuj(Konwertuj2(10, glosnosc));
		}
		
		wlaczonaGlosnosc = 1;
	}
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
	IP=0x10; //podwyzszenie priorytetu portu szeregowego!!!!!!
	ES=1;
	EA  = 1;
	TR2=1;
}
void ISR_Serial(void) interrupt 4
{
	
	if(TI==1) {TI=0;}
	if(RI==1)
	{
		odebrano = SBUF;
		P1 = odebrano;
		if (odebrano == 'S' && wlaczono == 0)// wlaczenie 
		{
			wlaczono = 1;

		}
		else if (odebrano == 'S' && wlaczono == 1)// wylaczenie 
		{
			wlaczono = 0;
			Pasmo[0] = 64; //znak -
			Pasmo[1] = 64;
			Pasmo[2] = 64;
		}
		else if (odebrano == 'R')// rozladowano baterie 
		{
			rozladowano = 1;
			wlaczono = 0;
			Pasmo[0] = 0; //znak -
			Pasmo[1] = 0;
			Pasmo[2] = 0;
		}
		else if (odebrano == '*' && wlaczono == 1)// reset pasma i glosnosci
		{
			Pasmo[0] = 64; //znak -
			Pasmo[1] = 64;
			Pasmo[2] = 64;
			glosnosc = 0;
		}
		else if (wlaczono == 1 && (odebrano == '1' || odebrano == '2' || odebrano == '3' || odebrano == '4' || odebrano == '5' || odebrano == '6' || odebrano == '7' || odebrano == '8' || odebrano == '9' || odebrano == '0'))// w rzeczywistosci sa 2 adresy SBUF 1 na wejscie i 1 na wyjscie
		{
			if (Pasmo[0] == 64) {
				Pasmo[0] = KonwertujAscii(odebrano);
			}
			else {
				if (Pasmo[1] == 64) {
					Pasmo[1] = KonwertujAscii(odebrano);
				}
				else {
					if (Pasmo[2] == 64) {
						Pasmo[2] = KonwertujAscii(odebrano);
					}
				}
			}

			P1 = odebrano;
		}
		Pasmo2[1] = Pasmo[2];
		Pasmo2[2] = Pasmo[1];
		Pasmo2[3] = Pasmo[0];
		RI=0;
	}
	//TI = 1;
}


void main() {
	rozladowano = 0;
	wlaczonaGlosnosc = 0;
	glosnoscTimer = 0;
	glosnosc = 0;
	Pasmo2[3] = 64;
	Pasmo2[4] = 64;
	wlaczono = 0;
	LicznikSerial = 0;
	P3_4 = 0; //nasluchiwanie
	wcisniety1 = 0;
	a = 0;
	EA = 1;
	SerialInit();
	
	//wlacz przerwanie INT00
	EX0 = 1;
	timerInit();
	
	while (1) {
		if (rozladowano == 1 || wlaczono == 0) {
			Ones = 0;
			Tens = 0;
		}
		else if (wlaczonaGlosnosc == 0 && rozladowano != 1) {
			Ones = Pasmo2[a];
			Tens = Pasmo2[a + 1];
		}
		else {
			Ones = wyswietl;
			Tens = wyswietl2;
		}

	}
}