#include <REGX52.H>

int i = 0;
int licznik = 0;
bit blokada = 0;
bit blokadaSekwencja = 0;
bit wlaczony = 1;
bit zamiana = 0;
char ostatnioWcisniety = 20;
char wcisniecia2 = 0;
char wcisniecia3 = 0;
char puszczenia2 = 0;
char puszczenia3 = 0;
char odebrano;
void Send(unsigned char Value)//cala sekwencja nadawania
{    
	
	P3_4 = 1;
	ES = 0;
	TI=0;//TRANSMIT INTERRUPT, przyszykuj nadajnik
	SBUF = Value;//serial buffer
	
	while (TI == 0){;}
	ES = 1;
	P3_4 = 0;//przechodzisz w tryb nasluchiwania
	
}//RI receiver interrupt
void SerialInit(void)//inicjacja portu szeregowego
{
	P3_4 = 0;
	
	TCLK=1;
	RCLK=1;
	SCON=0x50;
	T2CON = 0x30;
	TH2=RCAP2H=0xFF;
	TL2=RCAP2L=0xDC;
	//IP=0x10; //podwyzszenie priorytetu portu szeregowego!!!!!!
	ES=1;
	EA  = 1;
	TR2=1;
}
void zmianaStanuDiody(void) {
	if (P0_0 == 0) {
		P0_0 = 1;
	}
	else {
		P0_0 = 0;
	}
}
void ISR_Serial(void) interrupt 4
{
	
	if(TI==1) {TI=0;}
	if(RI==1)
	{
		ES = 0;
		odebrano = SBUF;
		if (odebrano == 'S')// w rzeczywistosci sa 2 adresy SBUF 1 na wejscie i 1 na wyjscie
		{				
			zmianaStanuDiody();
		}
		else if (odebrano == 'R')// rozladowano baterie zablokuj
		{				
			blokada = 1;
			P0_0 = 1;
		}
		ES = 1;
		RI=0;
	}
	//TI = 1;
}
void main(void) {
	
	P0_0 = 1; //dioda zgaszona
	SerialInit();
	
	while(1) {
		//Pierwsza kolumna
		if (blokada != 1) {
			P2_1 = 1;
			P2_2 = 1;
			P2_3 = 0;
		}

		// 1 
		if (P2_4 == 0 && blokada == 0) {  
			while (P2_4 == 0) {	//kiedy przycisk wcisniete wykonaj
			}
			Send('1');
			ostatnioWcisniety = 1;
			zmianaStanuDiody();
			//sleep();
		}
		// 4
		if (P2_5 == 0 && blokada == 0) {  
			while(P2_5 == 0) {;}
			Send('4');
			//zmianaStanuDiody();
			ostatnioWcisniety = 4;
			zmianaStanuDiody();
			//sleep();
		}
		
		// 7
		if (P2_6 == 0 && blokada == 0) {  
			//funkcjonalnosc przycisku
			while (P2_6 == 0) {
			}
			puszczenia2++;
			if (puszczenia2 % 2 == 0) {	//funkcjonalnosc przycisku
				zamiana = 1;
		  }
			Send('7');
			ostatnioWcisniety = 7;
			zmianaStanuDiody();
		}
		// * 
		if (P2_7 == 0) {  
			while (P2_7 == 0) {
			}
			//funkcjonalnosc przycisku
			zmianaStanuDiody();
			Send('*');
			ostatnioWcisniety = 10;
			zmianaStanuDiody();
		}
		if (blokada != 1) {  //Druga kolumna klawiatury
			P2_1 = 1;
			P2_2 = 0;	  
			P2_3 = 1;
		}
		// 2
		if (P2_4 == 0) {  
			while (P2_4 == 0) {
			}
			Send('2');
			ostatnioWcisniety = 2;
			zmianaStanuDiody();
		}
		// 5 
		if (P2_5 == 0 && blokada == 0) {  
			while (P2_5 == 0) {
			}
			Send('5');
			//wlaczony = 0;
			ostatnioWcisniety = 5;
			zmianaStanuDiody();
		}
		// 8  
		if (P2_6 == 0 && blokada == 0) {  
			while (P2_6 == 0) {
			}
			Send('8');
			ostatnioWcisniety = 8;
			zmianaStanuDiody();
		}

		//0 
		if (P2_7 == 0 && blokada == 0) {  
			licznik = 0;
			while (P2_7 == 0) {
				;
			}
			Send('0');
			ostatnioWcisniety = 0;
			zmianaStanuDiody();
		}
		if (blokada != 1) {  //Trzecia kolumna klawiatury
			P2_1 = 0;
			P2_2 = 1;	  
			P2_3 = 1;
		}
		// 3 
		if (P2_4 == 0 && blokada == 0) {  
			while (P2_4 == 0) {
				;
			}
			Send('3');
			ostatnioWcisniety = 3;
			zmianaStanuDiody();
		}

		// 6
		if (P2_5 == 0 && blokada == 0) {  
			
			while (P2_5 == 0) {
				;
			}
			Send('6');
			ostatnioWcisniety = 6;
			zmianaStanuDiody();
		}
		// 9  
		if (P2_6 == 0 && blokada == 0) {  
			while (P2_6 == 0) {
			}
			Send('9');
			ostatnioWcisniety = 9;
			zmianaStanuDiody();
		}
		// # 
		if (P2_7 == 0 && blokada == 0) {
			while (P2_7 == 0) {}
			Send('#');
			ostatnioWcisniety = 11;
			zmianaStanuDiody();
		}
		if (ostatnioWcisniety == 11) {
				licznik++;
			}
			
	}
}