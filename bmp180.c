#include "bmp180.h"
#include "i2c.h"
#include <util/delay.h>

void BMP180startM(unsigned char comm)
{
	I2Cstart();
	I2Csend(I2CaddrW);
	I2Csend(0xF4);		// BMP180 control register
	I2Csend(comm);		// start measurement
	I2Cstop();
}

long BMP180readT(void)
{
	unsigned char msb, lsb;
	long ut;
	I2Cstart();
	I2Csend(I2CaddrW);
	I2Csend(0xF6);		// MSB register
	I2Cstart(); // I2Crestart();
	I2Csend(I2CaddrR);
	msb = I2CreadACK();
	lsb = I2CreadNACK();
	I2Cstop();
	//return( msb<<8 + lsb );
	ut = (((long)msb)<<8) + (long)lsb;
	//return(lsb);
	return(ut);
}

long BMP180readP(void)
{
    unsigned char msb, lsb, xlsb;
    long up;

    I2Cstart();
    I2Csend(I2CaddrW);
    I2Csend(0xF4);
    I2Csend(0x34);  
    I2Cstop();

    _delay_ms(5);  

    I2Cstart();
    I2Csend(I2CaddrW);
    I2Csend(0xF6);  // MSB register
    I2Cstart();     // Repeated start
    I2Csend(I2CaddrR);
    msb  = I2CreadACK();
    lsb  = I2CreadACK();
    xlsb = I2CreadNACK();
    I2Cstop();

    up = (((long)msb << 16) | ((long)lsb << 8) | xlsb) >> 8;

    return up;
}


struct cal BMP180readC(void)
{
	struct cal c;
	unsigned char msb, lsb;

	I2Cstart();
	I2Csend(I2CaddrW);
	I2Csend(0xAA);		// 1st calibration register
	I2Cstart();         // I2Crestart();
	I2Csend(I2CaddrR);

	msb = I2CreadACK();
	lsb = I2CreadACK();
	c.AC1 = (msb<<8) + lsb;

	msb = I2CreadACK();
	lsb = I2CreadACK();
	c.AC2 = (msb<<8) + lsb;

	msb = I2CreadACK();
	lsb = I2CreadACK();
	c.AC3 = (msb<<8) + lsb;
	
	msb = I2CreadACK();
	lsb = I2CreadACK();
	c.AC4 = (msb<<8) + lsb;
	
	msb = I2CreadACK();
	lsb = I2CreadACK();
	c.AC5 = (msb<<8) + lsb;
	
	msb = I2CreadACK();
	lsb = I2CreadACK();
	c.AC6 = (msb<<8) + lsb;
	
	msb = I2CreadACK();
	lsb = I2CreadACK();
	c.B1 = (msb<<8) + lsb;
	
	msb = I2CreadACK();
	lsb = I2CreadACK();
	c.B2 = (msb<<8) + lsb;
	
	msb = I2CreadACK();
	lsb = I2CreadACK();
	c.MB = (msb<<8) + lsb;
	
	msb = I2CreadACK();
	lsb = I2CreadACK();
	c.MC = (msb<<8) + lsb;

	msb = I2CreadACK();
	lsb = I2CreadNACK();
	c.MD = (msb<<8) + lsb;
	
	return(c);
}









