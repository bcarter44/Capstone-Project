#include "mbed.h"

I2C lcd(p28, p27);

int lcdAddress = 0x27;
int lcdAddressWrite = 0x7E;
int lcdAddressRead = 0x7F; 
char data[2]; 
void start();
void main()
{
    lcd.write(lcdAddressWrite,data,0x30);
    wait (100);
    while(1)
    {
    
    lcd.write(lcdAddressWrite,data,0x0A);
    }


}
