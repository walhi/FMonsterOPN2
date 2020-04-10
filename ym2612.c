/************************************************************************//**
 * \file    ym2612.c
 * \brief   Handles Yamaha YM2612 synthesizer chip
 *
 * YM2612 chip must be wired using PB and PE as follows:
 * D0~D7 (2~9)  PB0~PB7
 * /IC   (11)   PE0
 * A0    (17)   PE1
 * A1    (18)   PE2
 * /RD   (16)   PE3
 * /WR   (15)   PE4
 * /CS   (14)   PE5
 *
 * \author  Jesús Alonso (doragasu)
 ****************************************************************************/
/* This file is part of vmg-pod source package.
 *
 * vgm-pod is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Some open source application is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with vgm-pod.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ym2612.h"

/** \addtogroup ym2612_api
 *  \brief Module for controlling the YM2612 FM syntesizer. This module
 *  allows to use the YM2612 and write to its registers.
 *  \{ */

/// System clock. Could be obtained by SysCtlClockGet(), but this definition
/// avoids computing some divisions (to calculate delay loops, etc.).
#define SYS_CLK         60000000UL


/// Obtains the loop count to delay nanoseconds using SysCtlDelay(). Minimum
/// delay when using a 60 MHz SYSCLK is 50 ns.
#define DELAY_NS_COUNT(ns)  ((SYS_CLK/1000000UL)*(ns)/3000)
/// Obtains the loop count to delay microseconds using SysCtlDelay()
#define DELAY_US_COUNT(us)  ((SYS_CLK/1000000UL)*(us)/3)
/// Obtains the loop count to delay milliseconds using SysCtlDelay()
#define DELAY_MS_COUNT(ms)  ((SYS_CLK/1000UL)*(ns)/3)


/// Delay count for YM2612 RESET (26 ms)
#define DELAY_COUNT_YM2612_RESET    DELAY_US_COUNT(26)
/// Delay count for YM2612 writes (200 ns)
#define DELAY_COUNT_TWW             DELAY_NS_COUNT(200)

/// YM2612 RESET delay
#define DELAY_YM2612_RESET()        do{MAP_SysCtlDelay(\
                                        DELAY_COUNT_YM2612_RESET);}while(0)
/// Setup delay of 10 ns is lower than 50 ns, so we use NOP for a 17 ns delay
#define DELAY_TAS()                 asm(" NOP")
/// Hold delay of 10 ns is lower than 50 ns, so we use NOP for a 17 ns delay
#define DELAY_TAH()                 asm(" NOP")
/// Delay for YM2612 writes
#define DELAY_TWW()                 do{MAP_SysCtlDelay(DELAY_COUNT_TWW);\
                                        }while(0)


/************************************************************************//**
 * \brief Initializes the module, including hardware ports and YM2612 chip
 * itself. Must be called before using any other function in this module.
 ****************************************************************************/
void Ym2612Init(void)
{
}

/************************************************************************//**
 * \brief Writes a value to the specified YM2612 address.
 *
 * \param[in] addr Addres of the YM2612 to write to.
 * \param[in] val  Value to write to the specified address.
 ****************************************************************************/
static inline void Ym2612Write(u8 addr, u8 val)
{
	/*
    /// Write A0 and A1
    MAP_GPIOPinWrite(CTRL_PORT_BASE, YM2612_A0 | YM2612_A1, addr<<1);
    /// Delay Tas
    DELAY_TAS();
    /// Clear CS and WR
    MAP_GPIOPinWrite(CTRL_PORT_BASE, YM2612_CS | YM2612_WR, 0);
    /// Put data
    MAP_GPIOPinWrite(DATA_PORT_BASE, PIN_D0_D7, val);
    /// Delay Tww/Tcsw
    DELAY_TWW();
    /// Set CS and WR
    MAP_GPIOPinWrite(CTRL_PORT_BASE, YM2612_CS | YM2612_WR,
                     YM2612_CS | YM2612_WR);
    /// Delay Tah
    DELAY_TAH();
	*/
}

/************************************************************************//**
 * \brief Writes a value to the specified port and register of the YM2612.
 *
 * \param[in] port Port number to write to. Can be 0 or 1.
 * \param[in] reg  YM2612 register to write to.
 * \param[in] val  Value to write to the YM2612.
 ****************************************************************************/
void Ym2612RegWrite(u8 port, u8 reg, u8 val)
{
    /// Write register address
    Ym2612Write(port<<1, reg);
    /// \warning A wait of 17, 83 or 47 cycles must be done here!!!
    /// Write value
    Ym2612Write((port<<1) + 1, val);
    /// \warning A wait of 17, 83 or 47 cycles must be done here!!! (maybe)
}


/** \} */
