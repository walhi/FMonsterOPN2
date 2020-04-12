#include "ym2612.h"

#define OPN2 0x2b0

/** \addtogroup ym2612_api
 *  \brief Module for controlling the YM2612 FM syntesizer. This module
 *  allows to use the YM2612 and write to its registers.
 *  \{ */

void Ym2612Init(void)
{
}


/************************************************************************//**
 * \brief Writes a value to the specified port and register of the YM2612.
 *
 * \param[in] port Port number to write to. Can be 0 or 1.
 * \param[in] reg  YM2612 register to write to.
 * \param[in] val  Value to write to the YM2612.
 ****************************************************************************/
void Ym2612RegWrite(uint8_t port, uint8_t reg, uint8_t val)
{
  unsigned char hwaddr = OPN2 + 2 * (port > 0);
  do {} while(peekb(0, OPN2) & 0x80);
  pokeb(0, reg, hwaddr);
  pokeb(0, val, hwaddr + 1);
}
