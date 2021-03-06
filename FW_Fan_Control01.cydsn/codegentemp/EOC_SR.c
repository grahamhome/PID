/*******************************************************************************
* File Name: EOC_SR.c  
* Version 1.80
*
* Description:
*  This file contains API to enable firmware to read the value of a Status 
*  Register.
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "EOC_SR.h"

#if !defined(EOC_SR_sts_sts_reg__REMOVED) /* Check for removal by optimization */


/*******************************************************************************
* Function Name: EOC_SR_Read
********************************************************************************
*
* Summary:
*  Reads the current value assigned to the Status Register.
*
* Parameters:
*  None.
*
* Return:
*  The current value in the Status Register.
*
*******************************************************************************/
uint8 EOC_SR_Read(void) 
{ 
    return EOC_SR_Status;
}


/*******************************************************************************
* Function Name: EOC_SR_InterruptEnable
********************************************************************************
*
* Summary:
*  Enables the Status Register interrupt.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void EOC_SR_InterruptEnable(void) 
{
    uint8 interruptState;
    interruptState = CyEnterCriticalSection();
    EOC_SR_Status_Aux_Ctrl |= EOC_SR_STATUS_INTR_ENBL;
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: EOC_SR_InterruptDisable
********************************************************************************
*
* Summary:
*  Disables the Status Register interrupt.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void EOC_SR_InterruptDisable(void) 
{
    uint8 interruptState;
    interruptState = CyEnterCriticalSection();
    EOC_SR_Status_Aux_Ctrl &= (uint8)(~EOC_SR_STATUS_INTR_ENBL);
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: EOC_SR_WriteMask
********************************************************************************
*
* Summary:
*  Writes the current mask value assigned to the Status Register.
*
* Parameters:
*  mask:  Value to write into the mask register.
*
* Return:
*  None.
*
*******************************************************************************/
void EOC_SR_WriteMask(uint8 mask) 
{
    #if(EOC_SR_INPUTS < 8u)
    	mask &= (uint8)((((uint8)1u) << EOC_SR_INPUTS) - 1u);
	#endif /* End EOC_SR_INPUTS < 8u */
    EOC_SR_Status_Mask = mask;
}


/*******************************************************************************
* Function Name: EOC_SR_ReadMask
********************************************************************************
*
* Summary:
*  Reads the current interrupt mask assigned to the Status Register.
*
* Parameters:
*  None.
*
* Return:
*  The value of the interrupt mask of the Status Register.
*
*******************************************************************************/
uint8 EOC_SR_ReadMask(void) 
{
    return EOC_SR_Status_Mask;
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
