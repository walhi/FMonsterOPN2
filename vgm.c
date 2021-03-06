/************************************************************************/
/**
 * \file   vgm.c
 * \brief  Parses VGM files (only Genesis/Megadrive and Master System ones)
 *         and sends commands to YM2612 and SN76489 chips.
 * \author Sergey V. Karpesh (walhi)
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vgm.h"
#include "ym2612.h"

typedef struct
{
  VgmHead h;
  FILE *f;
  VgmStat s;
} VgmData;

/* VGM module datam */
static VgmData vd;

void VgmTimerHandler(void)
{
  /* Clear interrupt flag */
}

/**
 * \brief Module initialization. Must be called once before any other
 * function.
 ****************************************************************************/
void VgmInit(void)
{
  /* Initialize submodules */
  Ym2612Init();
}

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
VGMErrorCode VgmOpen(char *fileName)
{
  size_t readed;
  VgmDataBlock block;
  YM2612Data data;
  uint32_t pointer;
  uint16_t wait;
  uint8_t command;
  uint8_t i;
  uint32_t pos;

  block.data = NULL;

  /* Set some default values */
  memset(&vd.h, 0, VGM_MAX_HEADLEN);
  vd.h.snFeedback = 0x0009;
  vd.h.snNfsrLen = 16;
  /* Open the file */
  vd.s = VGM_CLOSE;
  if ((vd.f = fopen(fileName, "r")) == NULL){
    fprintf(stderr, "File %s open error.\r\n", fileName);
    return VGM_FILE_ERR;
  }

  /* Read fields of 1.00 VGM header, checking for errors */
  readed = fread(&vd.h, 1, VGM_MIN_HEADLEN, vd.f);
  if (!readed){
    fclose(vd.f);
    fprintf(stderr, "VGM_FILE_ERR 1\r\n");
    return VGM_FILE_ERR;
  }
  if (readed < VGM_MIN_HEADLEN){
    fclose(vd.f);
    fprintf(stderr, "VGM_HEAD_ERR 1\r\n");
    return VGM_HEAD_ERR;
  }

  /* Check for file identification "VGM " string */
  /* if (0x206D6756 != vd.h.ident){ */
  if (vd.h.ident[0] != 0x56 || vd.h.ident[1] != 0x67 || vd.h.ident[2] != 0x6d || vd.h.ident[3] != 0x20){
    fclose(vd.f);
    printf("VGM_HEAD_ERR 2 |0x%08x|\r\n", (long int)vd.h.ident);
    return VGM_HEAD_ERR;
  }

  /* Read extra fields if header version is 1.51 or greater */
  if (1.51 <= vd.h.version){
    readed = fread(&vd.h.rf5c68Clk, 1, VGM_MAX_HEADLEN - VGM_MIN_HEADLEN, vd.f);
    if (!readed || ((VGM_MAX_HEADLEN - VGM_MIN_HEADLEN) != readed)){
      fclose(vd.f);
      fprintf(stderr, "VGM_HEAD_ERR 3\r\n");
      return VGM_HEAD_ERR;
    }
  }

  /* Check this is a Master System or Megadrive/Genesis VGM file */
  if (!vd.h.ym2612Clk && !vd.h.sn76489Clk){
    fclose(vd.f);
    fprintf(stderr, "VGM_HEAD_ERR 4\r\n");
    return VGM_HEAD_ERR;
  }

  /* File OK, go to stop state */
  vd.s = VGM_STOP;

  fprintf(stderr, "Version: 0x%08x\r\n", vd.h.version);
  fprintf(stderr, "OPN2 clock: 0x%08x\r\n", vd.h.ym2612Clk);
  fprintf(stderr, "OPN2 clock: %u\r\n", vd.h.ym2612Clk);
  fprintf(stderr, "VGM rate: %d\r\n", vd.h.rate);
  fprintf(stderr, "VGM Data offset: 0x%08x\r\n", vd.h.VgmStreamOffset);
  /* return VGM_OK; */

  /* go to data offset value */
  fseek(vd.f, (long int)0x34, SEEK_SET);
  fseek(vd.f, (long int)vd.h.VgmStreamOffset, SEEK_CUR);

  while (1){
    fread(&command, sizeof(uint8_t), 1, vd.f);
    switch (command){
    case 0x67:
      fread(&block, 1, 6, vd.f);
      if (block.data != NULL){
        free(block.data);
      }
      if (block.size > UINT_MAX) {
        return VGM_STREAM_ERR;
      }
      block.data = (uint8_t *)malloc((size_t)block.size);
      if (block.data == NULL){
        return VGM_ERROR;
      }
      block.current = block.data;
      readed = fread(block.data, 1, block.size, vd.f);
      fseek(vd.f, block.size, SEEK_CUR); /* fix dos? */
      /* fprintf(stderr, "Data block (%d/%d bytes)\n", readed, block.size); */
      break;
    case 0x52:
    case 0x53:
      fread(&data, 1, sizeof(YM2612Data), vd.f);
      /* fprintf(stderr, "Port %d, reg 0x%02x, value 0x%02x\n", (command==0x52)?0:1, data.reg, data.value); */
      break;
    case 0x61:
      fread(&wait, 1, sizeof(uint16_t), vd.f);
      /* fprintf(stderr, "Wait %d samples\n", wait); */
      break;
    case 0x66:
      /* fprintf(stderr, "End of data\n", wait); */
      return VGM_OK;

      break;
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7A:
    case 0x7B:
    case 0x7C:
    case 0x7D:
    case 0x7E:
    case 0x7F:
      /* fprintf(stderr, "Wait %d samples\n", command & 0x0f + 1); */
      break;
    case 0x80:
    case 0x81:
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x86:
    case 0x87:
    case 0x88:
    case 0x89:
    case 0x8A:
    case 0x8B:
    case 0x8C:
    case 0x8D:
    case 0x8E:
    case 0x8F:
      if ((block.data + block.size) > block.current){
        Ym2612RegWrite((uint8_t)0, (uint8_t)0x2A, *block.current++);
      }
      /* wait (command & 0x0f); */
      /* fprintf(stderr, "Send PCM Data. Wait %d samples.\n", (command & 0x0f)); */
      break;
    case 0xE0:
      fread(&pointer, 1, sizeof(uint32_t), vd.f);
      if (pointer != 0)
        fseek(vd.f, (long int)pointer, SEEK_CUR);

      /* fprintf(stderr, "Go to data block.\n", wait); */
      break;
    default:
      fprintf(stderr, "wtf? 0x%02x\n", command);
      return VGM_ERROR;
    }


  }

  return VGM_OK;
}

/************************************************************************//**
                                                                           * \brief Stars playing a previously opened VGM file.
                                                                           *
                                                                           * \return
                                                                           * - VGM_OK Playback has started
                                                                           * - VGM_BUSY Already busy playing a file
                                                                           * - VGM_ERROR Couldn't play file because it is not opened or has errors.
                                                                           ****************************************************************************/
int VgmPlay(void)
{
  switch (vd.s)
    {
    case VGM_ERROR_STOP:
    case VGM_CLOSE: return VGM_ERROR;
    case VGM_PLAY: return VGM_BUSY;
    case VGM_STOP:
      /* Go to start of data, preload a data sector */
      /* Load samples if any */
    case VGM_PAUSE:
      vd.s = VGM_PLAY;
      /* Enable timer. It will do all the work */
      return VGM_OK;
    }
  return VGM_OK;
}

/************************************************************************//**
                                                                           * \brief Advances the VGM file stream pointer
                                                                           *
                                                                           * \param[in] timeMs Time to advance in milliseconds.
                                                                           * \return
                                                                           * - VGM_OK Stream successfully advanced
                                                                           * - VGM_EOF Reached end of file
                                                                           * - VGM_ERROR Couldn't advance because file is not opened or has errors.
                                                                           ****************************************************************************/
int VgmFf(uint32_t timeMs)
{
  return VGM_OK;
}

/************************************************************************//**
                                                                           * \brief Rewinds the VGM file stream pointer
                                                                           *
                                                                           * \param[in] timeMs Time to rewind in milliseconds.
                                                                           * \return
                                                                           * - VGM_OK Stream successfully rewinded
                                                                           * - VGM_SOF Reached start of file
                                                                           * - VGM_ERROR Couldn't rewind because file is not opened or has errors.
                                                                           ****************************************************************************/
int VgmRew(uint32_t timeMs)
{
  return VGM_OK;
}

/************************************************************************//**
                                                                           * \brief Pauses playback
                                                                           *
                                                                           * \return
                                                                           * - VGM_OK Playback paused
                                                                           * - VGM_ERROR Couldn't pause because file is not opened/playing.
                                                                           ****************************************************************************/
int VgmPause(void)
{
  return VGM_OK;
}

/************************************************************************//**
                                                                           * \brief Stops playback
                                                                           *
                                                                           * \return
                                                                           * - VGM_OK Playback stopped
                                                                           * - VGM_ERROR Couldn't stop because file is not opened/playing.
                                                                           ****************************************************************************/
int VgmStop(void)
{
  return VGM_OK;
}

/************************************************************************//**
                                                                           * \brief Closes VGM file
                                                                           *
                                                                           * \return
                                                                           * - VGM_OK VGM File closed
                                                                           * - VGM_BUSY Couldn't close file because it is being played.
                                                                           * - VGM_ERROR Couldn't close file because it is not opened and stopped.
                                                                           ****************************************************************************/
int VgmClose(void)
{
  return VGM_OK;
}

/************************************************************************//**
                                                                           * \brief Returns VGM file header.
                                                                           *
                                                                           * \return The VGM header of the opened file, or NULL if the file is not
                                                                           * properly opened.
                                                                           ****************************************************************************/
VgmHead *VgmGetHead(void)
{
  return &vd.h;
}

/************************************************************************//**
                                                                           * \brief Returns playback cursor
                                                                           *
                                                                           * \return The playback cursor
                                                                           ****************************************************************************/
uint32_t VgmGetCursor(void)
{
  return 0;
}

/************************************************************************//**
                                                                           * \brief Returns the playback/module status
                                                                           *
                                                                           * \return The playback/module status
                                                                           ****************************************************************************/
VgmStat VgmGetStat(void)
{
  return vd.s;
}
