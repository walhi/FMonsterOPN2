/************************************************************************/
/**
 * \file   vgm.h
 * \brief  Parses VGM files (only Genesis/Megadrive and Master System ones)
 *         and sends commands to YM2612 and SN76489 chips.
 * \author Sergey V. Karpesh (walhi)
 ****************************************************************************/

#ifndef _VGM_H_
#define _VGM_H_

#include <limits.h>
#include "types.h"

/* Dirty trick to check things at compile time and error if check fails */
#define COMPILE_TIME_ASSERT(expr) typedef uint8_t COMP_TIME_ASSERT[((!!(expr))*2-1)]

#define VGM_MAX_HEADLEN     192

#define VGM_MIN_HEADLEN     64

/* Function completed without error */
enum VGMErrorCode {
  VGM_OK=0,              /* Function failed */
  VGM_ERROR=-1,          /* An error opening/reading a file occurred */
  VGM_FILE_ERR=-2,       /* The VGM header was not correct */
  VGM_HEAD_ERR=-3,       /* The VGM stream was not correct */
  VGM_STREAM_ERR=-4,     /* Unsupported VGM file */
  VGM_NOT_SUPPORTED=-5,  /* Reached end of VGM stream */
  VGM_EOF=-6,            /* Reached start of VGM stream */
  VGM_SOF=-7,            /* Cannot complete request because module is busy */
  VGM_BUSY=-8
};
typedef enum VGMErrorCode VGMErrorCode;

typedef struct
{
  int8_t reg;
  uint8_t value;
} /*__attribute__((packed))*/ YM2612Data;

typedef struct
{
  uint8_t fix;   /* 0x66 compatibility command to make older players stop parsing the stream */
  uint8_t type;  /* Data type */
  uint32_t size; /* Data size */
  uint8_t *data; /* Data */
  uint8_t *current;
} /*__attribute__((packed))*/ VgmDataBlock;

/* VGM header version 1.61 */
typedef struct
{
  uint8_t ident[4];
  uint32_t eofOffset;
  uint32_t version;
  uint32_t sn76489Clk;
  uint32_t ym2413Clk;
  uint32_t gd3Offset;
  uint32_t totalSamples;
  uint32_t loopOffset;
  uint32_t loopNSamples;
  /* VGM 1.01 additions */
  uint32_t rate;
  uint16_t snFeedback; /* 0x0009 for versions prior to 1.01 */
  uint8_t snNfsrLen;   /* 16 for versions prior to 1.01 */
  /* VGM 1.51 additions */
  uint8_t snFlags;
  /* VGM 1.10 additions */
  uint32_t ym2612Clk;
  uint32_t ym2151Clk;
  /* VGM 1.50 additions */
  uint32_t VgmStreamOffset;   /* 0x40 for versions prior to 1.50 */
  /* VGM 1.51 additions */
  uint32_t SegaPcmClk;
  uint32_t SegaPcmIfReg;
  uint32_t rf5c68Clk;
  uint32_t ym2203Clk;
  uint32_t ym2608Clk;
  uint32_t ym2610Clk;
  uint32_t ym3812Clk;
  uint32_t ym3526Clk;
  uint32_t y8950Clk;
  uint32_t ymf262Clk;
  uint32_t ymf278bClk;
  uint32_t ymf271Clk;
  uint32_t ymf280bClk;
  uint32_t rf5c164Clk;
  uint32_t pwmClk;
  uint32_t ay8910Clk;
  uint8_t ay8910Chip;
  uint8_t ay8910Flags;
  uint8_t ym2203Flags;
  uint8_t ym2608Flags;
  /* VGM 1.60 additions */
  uint8_t volModifier;
  uint8_t reserved1;
  uint8_t loopBase;
  /* VGM 1.51 additions */
  uint8_t loopModif;
  uint32_t GmbDmgClk;
  uint32_t NesApuClk;
  uint32_t MultiPcmClk;
  uint32_t upd7759Clk;
  uint32_t okim6258Clk;
  uint8_t okim6258Flags;
  uint8_t k054539Flags;
  uint8_t c140Type;
  uint8_t reserved2;
  uint32_t okim6295Clk;
  uint32_t k051649Clk;
  uint32_t k054539Clk;
  uint32_t huc6280Clk;
  uint32_t c140Clk;
  uint32_t k053260Clk;
  uint32_t pokeyClk;
  uint32_t qSoundClk;
  uint32_t reserved3;
  uint32_t reserved4;
} /*__attribute__((packed))*/ VgmHead;

typedef enum
  {
    VGM_CLOSE,      /* < No VGM file is opened */
    VGM_STOP,       /* < VGM file is opened, playback is stopped */
    VGM_PLAY,       /* < VGM file is being played */
    VGM_PAUSE,      /* < VGM playback is paused */
    VGM_ERROR_STOP  /* < An error occurred while playing the file */
  } VgmStat;

/* Check header length is correct */
COMPILE_TIME_ASSERT(sizeof(VgmHead) == VGM_MAX_HEADLEN);

/************************************************************************/
/**
 * \brief Module initialization. Must be called once before any other
 * function.
 ****************************************************************************/

void VgmInit(void);

/************************************************************************/
/**
 * \brief Opens a VGM file and parses its header, to get ready to play it.
 *
 * \param[in] fileName Name of the file to open.
 * \return
 * - VGM_OK File has been opened and parsed correctly.
 * - VGM_FILE_ERR File couldn't be opened and read correctly.
 * - VGM_HEAD_ERR VGM header checks failed.
 * - VGM_STREAM_ERR Stream format is not correct.
 * - VGM_NOT_SUPPORTED VGM header looks correct but file is not supported.
 ****************************************************************************/
VGMErrorCode VgmOpen(char *fileName);

/************************************************************************/
/**
 * \brief Stars playing a previously opened VGM file.
 *
 * \return
 * - VGM_OK Playback has started
 * - VGM_BUSY Already busy playing a file
 * - VGM_ERROR Couldn't play file because it is not opened or has errors.
 ****************************************************************************/
int VgmPlay(void);

/************************************************************************/
/**
 * \brief Advances the VGM file stream pointer
 *
 * \param[in] timeMs Time to advance in milliseconds.
 * \return
 * - VGM_OK Stream successfully advanced
 * - VGM_EOF Reached end of file
 * - VGM_ERROR Couldn't advance because file is not opened or has errors.
 ****************************************************************************/
int VgmFf(uint32_t timeMs);

/************************************************************************/
/**
 * \brief Rewinds the VGM file stream pointer
 *
 * \param[in] timeMs Time to rewind in milliseconds.
 * \return
 * - VGM_OK Stream successfully rewinded
 * - VGM_SOF Reached start of file
 * - VGM_ERROR Couldn't rewind because file is not opened or has errors.
 ****************************************************************************/
int VgmRew(uint32_t timeMs);

/************************************************************************/
/**
 * \brief Pauses playback
 *
 * \return
 * - VGM_OK Playback paused
 * - VGM_ERROR Couldn't pause because file is not opened/playing.
 ****************************************************************************/
int VgmPause(void);

/************************************************************************/
/**
 * \brief Stops playback
 *
 * \return
 * - VGM_OK Playback stopped
 * - VGM_ERROR Couldn't stop because file is not opened/playing.
 ****************************************************************************/
int VgmStop(void);

/************************************************************************/
/**
 * \brief Closes VGM file
 *
 * \return
 * - VGM_OK VGM File closed
 * - VGM_BUSY Couldn't close file because it is being played.
 * - VGM_ERROR Couldn't close file because it is not opened and stopped.
 ****************************************************************************/
int VgmClose(void);

/************************************************************************/
/**
 * \brief Returns VGM file header.
 *
 * \return The VGM header of the opened file, or NULL if the file is not
 * properly opened.
 ****************************************************************************/
VgmHead *VgmGetHead(void);

/************************************************************************/
/**
 * \brief Returns playback cursor
 *
 * \return The playback cursor
 ****************************************************************************/
uint32_t VgmGetCursor(void);

/************************************************************************/
/**
 * \brief Returns the playback/module status
 *
 * \return The playback/module status
 ****************************************************************************/
VgmStat VgmGetStat(void);

#endif // _VGM_H_
