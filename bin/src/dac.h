/*****************************************************************************
 *   da.h:  Header file for Digital to Analog functions
 *
 *   Copyright(C) 2010, Tecnequip Tecnologia Ltda.
 *   All rights reserved.
 *
 *   History
 *   2010.04.13  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __DAC_H
#define __DAC_H

void			DAC_Init (void);
unsigned int	DAC_Read (void);
void			DAC_Write(unsigned int val);

#endif /* end __DAC_H */

/******************************************************************************
**                            End Of File
******************************************************************************/
