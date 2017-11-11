/*******************************************************************************
* File Name: start_echo_IRR.h
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
#if !defined(CY_ISR_start_echo_IRR_H)
#define CY_ISR_start_echo_IRR_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void start_echo_IRR_Start(void);
void start_echo_IRR_StartEx(cyisraddress address);
void start_echo_IRR_Stop(void);

CY_ISR_PROTO(start_echo_IRR_Interrupt);

void start_echo_IRR_SetVector(cyisraddress address);
cyisraddress start_echo_IRR_GetVector(void);

void start_echo_IRR_SetPriority(uint8 priority);
uint8 start_echo_IRR_GetPriority(void);

void start_echo_IRR_Enable(void);
uint8 start_echo_IRR_GetState(void);
void start_echo_IRR_Disable(void);

void start_echo_IRR_SetPending(void);
void start_echo_IRR_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the start_echo_IRR ISR. */
#define start_echo_IRR_INTC_VECTOR            ((reg32 *) start_echo_IRR__INTC_VECT)

/* Address of the start_echo_IRR ISR priority. */
#define start_echo_IRR_INTC_PRIOR             ((reg8 *) start_echo_IRR__INTC_PRIOR_REG)

/* Priority of the start_echo_IRR interrupt. */
#define start_echo_IRR_INTC_PRIOR_NUMBER      start_echo_IRR__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable start_echo_IRR interrupt. */
#define start_echo_IRR_INTC_SET_EN            ((reg32 *) start_echo_IRR__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the start_echo_IRR interrupt. */
#define start_echo_IRR_INTC_CLR_EN            ((reg32 *) start_echo_IRR__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the start_echo_IRR interrupt state to pending. */
#define start_echo_IRR_INTC_SET_PD            ((reg32 *) start_echo_IRR__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the start_echo_IRR interrupt. */
#define start_echo_IRR_INTC_CLR_PD            ((reg32 *) start_echo_IRR__INTC_CLR_PD_REG)


#endif /* CY_ISR_start_echo_IRR_H */


/* [] END OF FILE */
