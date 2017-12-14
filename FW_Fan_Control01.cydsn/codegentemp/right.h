/*******************************************************************************
* File Name: right.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_right_H)
#define CY_ISR_right_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void right_Start(void);
void right_StartEx(cyisraddress address);
void right_Stop(void);

CY_ISR_PROTO(right_Interrupt);

void right_SetVector(cyisraddress address);
cyisraddress right_GetVector(void);

void right_SetPriority(uint8 priority);
uint8 right_GetPriority(void);

void right_Enable(void);
uint8 right_GetState(void);
void right_Disable(void);

void right_SetPending(void);
void right_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the right ISR. */
#define right_INTC_VECTOR            ((reg32 *) right__INTC_VECT)

/* Address of the right ISR priority. */
#define right_INTC_PRIOR             ((reg8 *) right__INTC_PRIOR_REG)

/* Priority of the right interrupt. */
#define right_INTC_PRIOR_NUMBER      right__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable right interrupt. */
#define right_INTC_SET_EN            ((reg32 *) right__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the right interrupt. */
#define right_INTC_CLR_EN            ((reg32 *) right__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the right interrupt state to pending. */
#define right_INTC_SET_PD            ((reg32 *) right__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the right interrupt. */
#define right_INTC_CLR_PD            ((reg32 *) right__INTC_CLR_PD_REG)


#endif /* CY_ISR_right_H */


/* [] END OF FILE */
