#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>
#include "display.h"
#include "bmp180.h"
#include "i2c.h"

void init_USART0()
{
    UBRR0H = 0;
    UBRR0L = 103;           // 9600 at 16MHz
    //UBRR1H = 0;
    UCSR0B |= (1<<RXEN0)|(1<<TXEN0);   // TX enable
    //UCSR1C |= (1<<UCSZ11)|(1<<UCSZ10);  // 8 bits DATA
    UCSR0C = 0x06;
    //DDRD |= (1<<3);
}

unsigned char USART_receive(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

int main(void)
{
	struct cal c;
	int i;
	long UT, TT, T;
	long X1, X2, B5;
	long UP, TP, P;
	
    init_display();
    init_I2C();
	init_USART0();	  // initialize USART
	char ch;
	char qnh_buf[4];
	i = 0;
	int QNH = 0;

	while (1)
	{
		// Poll until 4 digits are received via USART0
		while (i < 4)
		{
			if (UCSR0A & (1 << RXC0))  // Check if data received on USART0
			{
				ch = UDR0;        // Read from USART0 data register

				if (ch >= '0' && ch <= '9')  // Accept only digits
				{
					qnh_buf[i++] = ch;
				}
			}
		}
		break;  // Exit the outer while(1) loop
	}

	for (int i = 0; i < 4; i++){
		QNH += (qnh_buf[i]-'0')*pow(10,(3-i));
	}

	sei();            
    c = BMP180readC();
	int OSS = 0;
    // loop forever
    while(1)
	{
		// === Temperature Measurement ===
		BMP180startM(Temp);
		_delay_ms(40);  // delay for temperature measurement
		UT = BMP180readT();

		// Temperature compensation
		X1 = ((UT - (long)c.AC6) * (long)c.AC5) >> 15;
		X2 = ((long)c.MC << 11) / (X1 + (long)c.MD);
		B5 = X1 + X2;
		T = (B5 + 8) >> 4;  // T in 0.1°C

		// Display temperature (as before)
		long temp_disp = T;
		for(i = 0; i < 5; i++)
		{
			TT = temp_disp / 10;
			LCDline[13 - i] = (temp_disp - 10 * TT) + '0';
			temp_disp = TT;
		}

		// === Pressure Measurement ===
		BMP180startM(Posrs1);  // trigger pressure measurement (0x34 for OSS = 0)
		_delay_ms(8);        // wait at least 4.5ms for OSS=0
		UP = BMP180readP();

		long B6 = B5 - 4000;
		long X1p = ((long)c.B2 * ((B6 * B6) >> 12)) >> 11;
		long X2p = ((long)c.AC2 * B6) >> 11;
		long X3 = X1p + X2p;
		long B3 = ((((long)c.AC1 * 4 + X3) << OSS) + 2) / 4;


		X1p = ((long)c.AC3 * B6) >> 13;
		X2p = ((long)c.B1 * ((B6 * B6) >> 12)) >> 16;
		X3 = ((X1p + X2p) + 2) >> 2;
		uint32_t B4 = ((uint32_t)c.AC4 * (uint32_t)(X3 + 32768)) >> 15;
		uint32_t B7 = ((uint32_t)UP - B3) * (50000 >> OSS);

		if (B7 < 0x80000000)
			P = (B7 * 2) / B4;
		else
			P = (B7 / B4) * 2;

		X1p = (P >> 8) * (P >> 8);
		X1p = (X1p * 3038) >> 16;
		X2p = (-7357 * P) >> 16;
		P = P + ((X1p + X2p + 3791) >> 4);
		
		float pressure_hPa = P / 100;      
		float ratio = pressure_hPa / QNH;
		float altitude = 44330.0 * (1.0 - pow(ratio, 0.1903));
		
		long pres_disp = P;
		long alt_disp = altitude;
		for(i = 0; i < 5; i++)
		{
			TP = alt_disp / 10;
			LCDline[29 - i] = (alt_disp - 10 * TP) + '0';
			alt_disp = TP;
		}

		_delay_ms(1000);
	}

}


