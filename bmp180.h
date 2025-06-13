#include <avr/io.h>

#define	I2CaddrW 0xEE
#define I2CaddrR 0xEF

#define Temp   0x2E
#define Posrs0 0x34
#define Posrs1 0x74
#define Posrs2 0xB4
#define Posrs3 0xF4

struct cal
{
	short          AC1, AC2, AC3;
	unsigned short AC4, AC5, AC6;
	short          B1, B2;
	short          MB, MC, MD;
};

void BMP180startM(unsigned char);

long BMP180readT(void);
long BMP180readP(void);

struct cal BMP180readC(void);
