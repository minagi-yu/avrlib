#include "i2c.h"

#define I2C_CLK_PER 3333333
#define I2C_FSCL    I2C_SM

void i2c_init(int khz)
{
    TWI0.MBAUD = (I2C_CLK_PER / I2C_FSCL - 10) / 2;

    TWI0.MCTRLA = (1 << TWI_ENABLE_bm);
}

int i2c_send(uint8_t addr, void *data, size_t len)
{
    TWI0.MCTRLB  |= TWI_FLUSH_bm;
	TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
	TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);

// M2
    TWI0.MADDR = addr << 1;

    while (!(TWI0.MSTATUS & TWI_WIF_bm));

    if (TWI0.MSTATUS & TWI_ARBLOST_bm) {
        TWI0.MCTRLB |= TWI_FLUSH_bm;
        TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
        return 1; //fail
        // M1
    }

    if (TWI0.MSTATUS & TWI_RXACK_bm) { //NACK
        TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
        // goto M2
    }

    // receive ACK
    

}
