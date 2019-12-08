#include "i2c.h"

#define I2C_CLK_PER 3333333
#define I2C_FSCL I2C_SM

void i2c_init(int khz)
{
    TWI0.MBAUD = (I2C_CLK_PER / I2C_FSCL - 10) / 2;

    TWI0.MCTRLA = (1 << TWI_ENABLE_bm);
}

static void i2c_wait()
{
    while (!(TWI0.MSTATUS & TWI_RIF_bm) && !(TWI0.MSTATUS & TWI_WIF_bm))
        ;
    TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);
}

static void i2c_reset()
{
    TWI0.MCTRLB |= TWI_FLUSH_bm;
    TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
}

int i2c_send(uint8_t addr, void *data, size_t len)
{
    uint8_t *dp = data;

    if (len == 0)
        return 1;

    i2c_reset();
    TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);

    do {
        TWI0.MADDR = addr << 1;

        i2c_wait();

        if (TWI0.MSTATUS & TWI_ARBLOST_bm) {
            i2c_reset();
            return 1;
        }
    } while (TWI0.MSTATUS & TWI_RXACK_bm); // NACK: rewrite address

    // ACK: write data
    do {
        TWI0.MDATA = *dp++;

        i2c_wait();

        if (TWI0.MSTATUS & TWI_ARBLOST_bm) {
            i2c_reset();
            return 1;
        }

        if (TWI0.MSTATUS & TWI_RXACK_bm)
            break;
    } while (--len);

    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;

    return 0;
}
