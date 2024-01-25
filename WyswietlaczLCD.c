#include <REGX52.H>
#include <LCD.H>

#define _U1_ID_ 0x01
#define _U3_ID_ 0x02
#define _U5_ID_ 0x04
#define _U8_ID_ 0x08

#define GET_ID() (_U8_ID_)

#define _SERIAL_BUFF_SIZE_ 2

#define DELAY_1000 200
char data AnimacjaPusta[17]   = "                 ";
char data AnimacjaPasmo[17]   = "Pasmo        MHz";
char data AnimacjaAntena[17]  = "Antena           ";
char data AnimacjaBateria[17] = "Bateria          ";
char data AnimacjaGlosnosc[17] = "Glosnosc        ";
char bateria;
char glosnosc;
char data licznikTimer;
char data kl;
char data kj;
char code stala = 9;
char code stala2 = 9;
char code statusAnteny[9]  = "wysuwanie";
char code statusAnteny2[9] = "wysunieto";
char code statusAnteny3[8] = "chowanie";
char code statusAnteny4[8] = "schowano";
char data dlugoscStatusAnteny2;
char odebrano;
bit wlaczono;
bit flagaOtworz;
char stanKrotkofalowki;
/* 0 - wylaczona
	 1 - wysuwanie anteny
	 2 - chowanie anteny
	 3 - wlaczona
*/
void t1_init(void);

const char code hex[16] = "0123456789ABCDEF";
//zwroc czesc dziesietna
char Konwertuj1(char system, char liczba) {
	return liczba/system;
}
//zwroc czesc jednosci
char Konwertuj2(char system, char liczba) {
	return liczba%system;
}
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
        if (bateria <= 0) {
					ET0 = 0;
					TR0 = 0;
					bateria = 0;
					wlaczono = 0;
					AnimacjaBateria[11] = ' ';
					AnimacjaBateria[12] = '0';
					AnimacjaBateria[13] = '%';
					Lcd_DisplayString(3,1,AnimacjaBateria);
					Lcd_DisplayString(1,1,AnimacjaPusta);
					Lcd_DisplayString(2,1,AnimacjaPusta);
					Lcd_DisplayString(3,1,AnimacjaPusta);
					Lcd_DisplayString(4,1,AnimacjaPusta);
					Send('R');
				}
        if(TI==1) {TI=0;}
        if(RI==1)
        {
            odebrano = SBUF;
						if (odebrano == '#' && stanKrotkofalowki == 0 && bateria > 0)// wlaczenie 
            {
							stanKrotkofalowki = 1;
							kl = 0;
							for (kl; kl < stala; kl++) {
								AnimacjaAntena[kl + dlugoscStatusAnteny2] = statusAnteny[kl];
							}								
							Lcd_DisplayString(4,1,AnimacjaAntena);
							
							if (glosnosc < 10) {
								AnimacjaGlosnosc[12] = hex[glosnosc];
							}
							else {
								//czesc dziesietna
								AnimacjaGlosnosc[11] = hex[glosnosc / 10];
								//czesc jednosci
								AnimacjaGlosnosc[12] = hex[glosnosc % 10];
							}
							Lcd_DisplayString(2,1,AnimacjaGlosnosc);
            }
						else if (odebrano == '#' && stanKrotkofalowki == 3 && bateria > 0)// wylaczenie 
            {
							stanKrotkofalowki = 2;	//chowanie anteny
							kl = 0;
							for (kl; kl < stala - 1; kl++) {
								AnimacjaAntena[kl + dlugoscStatusAnteny2] = statusAnteny3[kl];
							}			
							//wyczysc poprzednie znaki
							AnimacjaAntena[15] = ' ';		
							Lcd_DisplayString(4,1,AnimacjaAntena);
            }
						else if (odebrano == '*' && stanKrotkofalowki == 3 && bateria > 0)// reset pasma i glosnosci
            {
							glosnosc = 0;
							AnimacjaGlosnosc[11] = ' ';
							//czesc jednosci
							AnimacjaGlosnosc[12] = hex[glosnosc];
							Lcd_DisplayString(2,1,AnimacjaGlosnosc);
							
							bateria = bateria - 1;
							AnimacjaPasmo[10] = ' ';
							AnimacjaPasmo[11] = ' ';
							AnimacjaPasmo[12] = ' ';
							Lcd_DisplayString(1,1,AnimacjaPasmo);
            }
            else if (stanKrotkofalowki == 3 && (odebrano == '1' || odebrano == '2' || odebrano == '3' || odebrano == '4' || odebrano == '5' || odebrano == '6' || odebrano == '7' || odebrano == '8' || odebrano == '9' || odebrano == '0') && bateria > 0)// w rzeczywistosci sa 2 adresy SBUF 1 na wejscie i 1 na wyjscie
            {
							bateria = bateria - 1;
							if (AnimacjaPasmo[12] == ' ') {
								AnimacjaPasmo[12] = odebrano;
							}
							else {
								if (AnimacjaPasmo[11] == ' ') {
									AnimacjaPasmo[11] = AnimacjaPasmo[12];
									AnimacjaPasmo[12] = odebrano;
								}
								else {
									if (AnimacjaPasmo[10] == ' ') {
										AnimacjaPasmo[10] = AnimacjaPasmo[11];
										AnimacjaPasmo[11] = AnimacjaPasmo[12];
										AnimacjaPasmo[12] = odebrano;
									}
								}
							}
							
							Lcd_DisplayString(1,1,AnimacjaPasmo);
            }
						else if (stanKrotkofalowki == 1 && odebrano == 'S' && bateria > 0)// wysunieto antene
            {				
							bateria = bateria - 5;
							
							kl = 0;
							for (kl; kl < stala2; kl++) {
								AnimacjaAntena[kl + dlugoscStatusAnteny2] = statusAnteny2[kl];
							}				
							Lcd_DisplayString(4,1,AnimacjaAntena);
							stanKrotkofalowki = 3;	//poprawne dzialanie
							ET0 = 1;
							TR0 = 1;
            }
						else if (stanKrotkofalowki == 2 && odebrano == 'S' && bateria > 0)// schowano antene
            {
							ET0 = 0;
							TR0 = 0;
							bateria = bateria - 5;
							kl = 0;
							for (kl; kl < stala - 1; kl++) {
								AnimacjaAntena[kl + dlugoscStatusAnteny2] = statusAnteny4[kl];
							}				
							//wyczysc poprzednie znaki
							AnimacjaAntena[15] = ' ';	
							//reset pasma
							AnimacjaPasmo[10] = ' ';
							AnimacjaPasmo[11] = ' ';
							AnimacjaPasmo[12] = ' ';
							Lcd_DisplayString(1,1,AnimacjaPasmo);
							Lcd_DisplayString(2,1,AnimacjaGlosnosc);
							Lcd_DisplayString(3,1,AnimacjaBateria);
							Lcd_DisplayString(4,1,AnimacjaAntena);

							Lcd_DisplayString(1,1,AnimacjaPusta);
							Lcd_DisplayString(2,1,AnimacjaPusta);
							Lcd_DisplayString(3,1,AnimacjaPusta);
							Lcd_DisplayString(4,1,AnimacjaPusta);
							
							stanKrotkofalowki = 0;	//wylaczenie anteny
            }
						else if (stanKrotkofalowki == 3 && odebrano == 'G' && bateria > 0)// zwieksz glosnosc o 1
            {				
							if (glosnosc < 16) {
								glosnosc++;
							}
							bateria = bateria - 3;
							if (glosnosc < 10) {
								AnimacjaGlosnosc[12] = hex[glosnosc];
							}
							else {
								//czesc dziesietna
								AnimacjaGlosnosc[11] = hex[glosnosc / 10];
								//czesc jednosci
								AnimacjaGlosnosc[12] = hex[glosnosc % 10];
							}
							Lcd_DisplayString(2,1,AnimacjaGlosnosc);
            }
						if (bateria == 100) {
							AnimacjaBateria[10] = '1';
							AnimacjaBateria[11] = '0';
							AnimacjaBateria[12] = '0';
							AnimacjaBateria[13] = '%';
						}
						else if (bateria < 100 && bateria >= 10) {
							AnimacjaBateria[10] = ' ';
							AnimacjaBateria[11] = hex[Konwertuj1(10, bateria)];
							AnimacjaBateria[12] = hex[Konwertuj2(10, bateria)];
							AnimacjaBateria[13] = '%';
						}
						else if (bateria < 10 && bateria >= 0) {
							AnimacjaBateria[10] = ' ';
							AnimacjaBateria[11] = ' ';
							AnimacjaBateria[12] = hex[Konwertuj2(10, bateria)];
							AnimacjaBateria[13] = '%';
						}
						if (bateria > 0 && stanKrotkofalowki != 0) {
							Lcd_DisplayString(3,1,AnimacjaBateria);
							Lcd_DisplayString(1,1,AnimacjaPasmo);
						}
            RI=0;
        }
  }
void SerialInit(void)//inicjacja portu szeregowego
{
	P3_4 = 0;
	T2CON = 0x30;
	TCLK=1;
	RCLK=1;
	//timer2
	SCON=0x50;
	TH2=RCAP2H=0xFF;
	TL2=RCAP2L=0xDC;
	//timer2 koniec
	//IP=0x10; //podwyzszenie priorytetu portu szeregowego!!!!!!
	ES=1;
	EA  = 1;
	TR2=1;
}
void setup(void)
{
	flagaOtworz = 0;
	licznikTimer = 0;
	glosnosc = 0;
	stanKrotkofalowki = 0;
	bateria = 11;
	wlaczono = 0;
	kl = 0;
	dlugoscStatusAnteny2 = 7;
	P3_4 = 0;
	/**/
	EA = 1;

	LcdInit();
	LcdWelcome();
}

int main(void)
{
	SerialInit();
	setup();
	t1_init();


	while (1); //
}
void t1_init(void)
{
	EA = 1;
	TCON = 0x00;	//TCOn = 0x10?
		/**/// t1_isr
	TMOD |= 0x01; // 16bit
	//TMOD = 0x03 8bit
	/**/
	ET0 = 1;
	TR0 = 1;
}
void t0_isr(void) interrupt 1
{
	licznikTimer++;
	if (licznikTimer > 253) {
		licznikTimer = 1;
	}
	if (licznikTimer % 3 == 0 && stanKrotkofalowki == 3 && bateria > 0) {
		
		bateria = bateria - 1;
		if (bateria < 100 && bateria >= 10) {
			AnimacjaBateria[10] = ' ';
			AnimacjaBateria[11] = hex[Konwertuj1(10, bateria)];
			AnimacjaBateria[12] = hex[Konwertuj2(10, bateria)];
			AnimacjaBateria[13] = '%';
		}
		else if (bateria < 10 && bateria >= 0) {
			AnimacjaBateria[10] = ' ';
			AnimacjaBateria[11] = ' ';
			AnimacjaBateria[12] = hex[Konwertuj2(10, bateria)];
			AnimacjaBateria[13] = '%';
		}
		Lcd_DisplayStringv2(3,1,AnimacjaBateria);
	}
	if (stanKrotkofalowki == 3 && bateria <= 0) {
		stanKrotkofalowki = 0;
		Lcd_DisplayStringv2(1,1,AnimacjaPusta);
		Lcd_DisplayStringv2(2,1,AnimacjaPusta);
		Lcd_DisplayStringv2(3,1,AnimacjaPusta);
		Lcd_DisplayStringv2(4,1,AnimacjaPusta);
		Send('R');
	}
	TL0 = TH0 = 0x11;
}
