
// driver for CYPRESS FM24Vxx f-ram chips
//
//   author:          Petre Rodan <petre.rodan@simplex.ro>
//   available from:  https://github.com/rodan/
//   license:         GNU GPLv3

#include <inttypes.h>

#include "fm24.h"

#include "serial_bitbang.h"

uint8_t fm24_seek(const uint32_t addr)
{
    uint8_t rv = 0;
    uint8_t retry;

    for (retry = 0; retry < FM24V10_MAX_RETRY; retry++) {
        rv = i2cm_start();

        if (rv != I2C_OK) {
            return EXIT_FAILURE;
        }

        rv = i2cm_tx(FM24V10_BA | (addr >> 16), I2C_WRITE);

        if (rv == I2C_ACK) {
            // f-ram memory address
            i2cm_tx((addr & 0xff00) >> 8, I2C_NO_ADDR_SHIFT);
            i2cm_tx(addr & 0xff, I2C_NO_ADDR_SHIFT);
            i2cm_stop();
            break;
        } else if (rv == I2C_NAK) {
            // device is sleeping and it should wake up in 400us max
            i2cm_stop();
        }
    }


    if (rv != I2C_ACK) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

uint32_t fm24_read(uint8_t *buf, const uint32_t nbyte)
{
    uint8_t rv = 0;

    rv = i2cm_start();

    if (rv != I2C_OK) {
        return EXIT_FAILURE;
    }

    rv = i2cm_tx(FM24V10_BA, I2C_READ);

    if (rv == I2C_ACK) {
        rv = i2cm_rx(buf, nbyte, I2C_LAST_NAK);
    }

    i2cm_stop();

    if (rv != I2C_ACK) {
        return EXIT_FAILURE;
    }

    return nbyte;
}

uint32_t fm24_read_from(uint8_t *buf, const uint32_t addr, const uint32_t nbyte)
{
    uint32_t rv;

    rv = fm24_seek(addr);

    if (rv == EXIT_SUCCESS) {
        rv = fm24_read(buf, nbyte);
    }

    return rv;
}


uint32_t fm24_write(const uint8_t *buf, const uint32_t addr, const uint32_t nbyte)
{
    uint8_t rv = 0;
    uint32_t i = 0;
    uint8_t retry;

    for (retry = 0; retry < FM24V10_MAX_RETRY; retry++) {
        rv = i2cm_start();

        if (rv != I2C_OK) {
            return 0;
        }

        // device slave address + memory page bit
        rv = i2cm_tx(FM24V10_BA | (addr >> 16), I2C_WRITE);

        if (rv == I2C_ACK) {
            // f-ram memory address
            i2cm_tx((addr & 0xff00) >> 8, I2C_NO_ADDR_SHIFT);
            i2cm_tx(addr & 0xff, I2C_NO_ADDR_SHIFT);

            // send data
            for (i=0;i<nbyte;i++) {
                rv = i2cm_tx(buf[i], I2C_NO_ADDR_SHIFT);
                if (rv != I2C_ACK) {
                    break;
                }
            }

            i2cm_stop();
            return i;
        } else if (rv == I2C_NAK) {
            // device is sleeping and it should wake up in 400us max
            i2cm_stop();
        }
    }

    return 0;
}

uint8_t fm24_sleep(void)
{
    uint8_t rv = 0;

    rv = i2cm_start();

    if (rv != I2C_OK) {
        return EXIT_FAILURE;
    }

    rv = i2cm_tx(FM24V10_RSVD, I2C_NO_ADDR_SHIFT);

    if (rv == I2C_ACK) {
        rv = i2cm_tx(FM24V10_BA, I2C_WRITE);
    } else {
        rv = EXIT_FAILURE;
    }

    if (rv == I2C_ACK) {
        i2cm_start();
        i2cm_tx(FM24V10_SLEEP, I2C_NO_ADDR_SHIFT);
        rv = EXIT_SUCCESS;
    } else {
        rv = EXIT_FAILURE;
    }

    i2cm_stop();
    return rv;
}

