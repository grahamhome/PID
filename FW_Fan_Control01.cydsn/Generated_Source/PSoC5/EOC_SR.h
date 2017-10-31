/*******************************************************************************
* File Name: EOC_SR.h  
* Version 1.80
*
* Description:
*  This file containts Status Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_STATUS_REG_EOC_SR_H) /* CY_STATUS_REG_EOC_SR_H */
#define CY_STATUS_REG_EOC_SR_H

#include "cytypes.h"
#include "CyLib.h"


/***************************************
*        Function Prototypes
***************************************/

uint8 EOC_SR_Read(void) ;
void EOC_SR_InterruptEnable(void) ;
void EOC_SR_InterruptDisable(void) ;
void EOC_SR_WriteMask(uint8 mask) ;
uint8 EOC_SR_ReadMask(void) ;


/***************************************
*           API Constants
***************************************/

#define EOC_SR_STATUS_INTR_ENBL    0x10u


/***************************************
*         Parameter Constants
***************************************/

/* Status Register Inputs */
#define EOC_SR_INPUTS              1


/***************************************
*             Registers
***************************************/

/* Status Register */
#define EOC_SR_Status             (* (reg8 *) EOC_SR_sts_sts_reg__STATUS_REG )
#define EOC_SR_Status_PTR         (  (reg8 *) EOC_SR_sts_sts_reg__STATUS_REG )
#define EOC_SR_Status_Mask        (* (reg8 *) EOC_SR_sts_sts_reg__MASK_REG )
#define EOC_SR_Status_Aux_Ctrl    (* (reg8 *) EOC_SR_sts_sts_reg__STATUS_AUX_CTL_REG )

#endif /* End CY_STATUS_REG_EOC_SR_H */


/* [] END OF FILE */
