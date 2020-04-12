#ifdef __cplusplus
extern "C"
{
#endif

#include "ym2612.h"

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
}


/** \} */

#ifdef __cplusplus
}
#endif
