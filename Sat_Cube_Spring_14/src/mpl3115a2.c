/* 
 * mpl3115a2.c 
 * 
 * Created: 5/21/2014 11:32:11 AM 
 *  Author: justin 
 */ 

#include "mpl3115a2.h" 

long altitudeWhole = 0;
long pressureWhole = 0;
long temperatureWhole = 0;

uint8_t altStatus = 0x00;

/************************************************************************/
/* Initialization Routines                                              */
/************************************************************************/

void mpl_init (void) 
{ 
   i2c_init(); 
   alt_set_mode();
   alt_set_eventFlags(); 
   return;
} 

void alt_set_mode (void)
{
	i2c_start_wait(MPL3115a2+I2C_WRITE);
	i2c_write(CTRL_REG1);
	i2c_write(0xB8);		// Altimeter mode
	i2c_stop();
	return;
}

void alt_set_eventFlags (void)
{
	i2c_start_wait(MPL3115a2+I2C_WRITE);
	i2c_write(PT_DATA_CFG);
	i2c_write(0x07);		// Enable all 3 pressure and temp flags
	i2c_stop();
	return;
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

void mpl_getAlt (uint8_t altStatus) 
{    
   long temp;
   
   alt_set_active();
    
   alt_get_status(); 
    
   char msbA,csbA,lsbA,msbT,lsbT = 0x00; 
       
   i2c_start_wait(MPL3115a2+I2C_WRITE); 
   i2c_write(OUT_P_MSB); 
   i2c_rep_start(MPL3115a2+I2C_READ); 
   //_delay_ms(10);
   msbA = i2c_readAck();		// Read the registers with i2c
   csbA = i2c_readAck(); 
   lsbA = i2c_readAck();
   msbT = i2c_readAck();
   lsbT = i2c_readNak(); 
   i2c_stop(); 
   

	altitudeWhole = ((msbA << 8) | csbA);
	
	pressureWhole =  (long)msbA<<16 | (long)csbA<<8 | (long)lsbA; // Whole number pressure
	pressureWhole >>= 6;

if(msbT > 0x7F) 
	{
		temp = ~(msbT << 8 | lsbT) + 1 ;			// 2's complement
		temperatureWhole = (long) (temp >> 8);		// Whole part of temperature
		temperatureWhole *= -1.;
	}
else 
	{
		temperatureWhole = (long) (msbT);			// Whole
	}
} 

/************************************************************************/
/* Subroutines                                                          */
/************************************************************************/

void alt_set_active (void)
{
	i2c_start_wait(MPL3115a2+I2C_WRITE);
	i2c_write(CTRL_REG1);
	i2c_write(0xB9);			// Set bit one to enable
	i2c_stop();
	return;
}

uint8_t alt_get_status (void) 
{ 
   uint8_t altStatus = 0x00; 
   uint8_t i = 0;
    
   while (((altStatus & 0x08) == 0) || (i <= 10))
   { 
      i2c_start_wait(MPL3115a2+I2C_WRITE); 
      i2c_write(STATUS); 
      i2c_rep_start(MPL3115a2+I2C_READ); 
      altStatus = i2c_readNak();	
      i2c_stop(); 
	  _delay_us(100);
	  i++;
   } 
   return altStatus;
} 

/*void alt_toggle_oneShot (void) 
{ 
   i2c_start_wait(MPL3115a2+I2C_WRITE); 
   i2c_write(CTRL_REG1); 
   i2c_write(0xB8); 
    
   i2c_start_wait(MPL3115a2+I2C_WRITE); 
   i2c_write(CTRL_REG1); 
   i2c_write(0xBB); 
   i2c_stop(); 
}*/ 

