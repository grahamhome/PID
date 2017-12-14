/*******************************************************************************
* File Name: left.c  
* Version 1.70
*
*  Description:
*   API for controlling the state of an interrupt.
*
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/


#include <cydevice_trm.h>
#include <CyLib.h>
#include <left.h>

#if !defined(left__REMOVED) /* Check for removal by optimization */

/*******************************************************************************
*  Place your includes, defines and code here 
********************************************************************************/
/* `#START left_intc` */

/* `#END` */

#ifndef CYINT_IRQ_BASE
#define CYINT_IRQ_BASE      16
#endif /* CYINT_IRQ_BASE */
#ifndef CYINT_VECT_TABLE
#define CYINT_VECT_TABLE    ((cyisraddress **) CYREG_NVIC_VECT_OFFSET)
#endif /* CYINT_VECT_TABLE */

/* Declared in startup, used to set unused interrupts to. */
CY_ISR_PROTO(IntDefaultHandler);


/*******************************************************************************
* Function Name: left_Start
********************************************************************************
*
* Summary:
*  Set up the interrupt and enable it.
*
* Parameters:  
*   None
*
* Return:
*   None
*
*******************************************************************************/
void left_Start(void)
{
    /* For all we know the interrupt is active. */
    left_Disable();

    /* Set the ISR to point to the left Interrupt. */
    left_SetVector(&left_Interrupt);

    /* Set the priority. */
    left_SetPriority((uint8)left_INTC_PRIOR_NUMBER);

    /* Enable it. */
    left_Enable();
}


/*******************************************************************************
* Function Name: left_StartEx
********************************************************************************
*
* Summary:
*  Set up the interrupt and enable it.
*
* Parameters:  
*   address: Address of the ISR to set in the interrupt vector table.
*
* Return:
*   None
*
*******************************************************************************/
void left_StartEx(cyisraddress address)
{
    /* For all we know the interrupt is active. */
    left_Disable();

    /* Set the ISR to point to the left Interrupt. */
    left_SetVector(address);

    /* Set the priority. */
    left_SetPriority((uint8)left_INTC_PRIOR_NUMBER);

    /* Enable it. */
    left_Enable();
}


/*******************************************************************************
* Function Name: left_Stop
********************************************************************************
*
* Summary:
*   Disables and removes the interrupt.
*
* Parameters:  
*
* Return:
*   None
*
*******************************************************************************/
void left_Stop(void)
{
    /* Disable this interrupt. */
    left_Disable();

    /* Set the ISR to point to the passive one. */
    left_SetVector(&IntDefaultHandler);
}


/*******************************************************************************
* Function Name: left_Interrupt
********************************************************************************
*
* Summary:
*   The default Interrupt Service Routine for left.
*
*   Add custom code between the coments to keep the next version of this file
*   from over writting your code.
*
* Parameters:  
*
* Return:
*   None
*
*******************************************************************************/
CY_ISR(left_Interrupt)
{
    /*  Place your Interrupt code here. */
    /* `#START left_Interrupt` */

    /* `#END` */
}


/*******************************************************************************
* Function Name: left_SetVector
********************************************************************************
*
* Summary:
*   Change the ISR vector for the Interrupt. Note calling left_Start
*   will override any effect this method would have had. To set the vector 
*   before the component has been started use left_StartEx instead.
*
* Parameters:
*   address: Address of the ISR to set in the interrupt vector table.
*
* Return:
*   None
*
*******************************************************************************/
void left_SetVector(cyisraddress address)
{
    cyisraddress * ramVectorTable;

    ramVectorTable = (cyisraddress *) *CYINT_VECT_TABLE;

    ramVectorTable[CYINT_IRQ_BASE + (uint32)left__INTC_NUMBER] = address;
}


/*******************************************************************************
* Function Name: left_GetVector
********************************************************************************
*
* Summary:
*   Gets the "address" of the current ISR vector for the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   Address of the ISR in the interrupt vector table.
*
*******************************************************************************/
cyisraddress left_GetVector(void)
{
    cyisraddress * ramVectorTable;

    ramVectorTable = (cyisraddress *) *CYINT_VECT_TABLE;

    return ramVectorTable[CYINT_IRQ_BASE + (uint32)left__INTC_NUMBER];
}


/*******************************************************************************
* Function Name: left_SetPriority
********************************************************************************
*
* Summary:
*   Sets the Priority of the Interrupt. Note calling left_Start
*   or left_StartEx will override any effect this method 
*   would have had. This method should only be called after 
*   left_Start or left_StartEx has been called. To set 
*   the initial priority for the component use the cydwr file in the tool.
*
* Parameters:
*   priority: Priority of the interrupt. 0 - 7, 0 being the highest.
*
* Return:
*   None
*
*******************************************************************************/
void left_SetPriority(uint8 priority)
{
    *left_INTC_PRIOR = priority << 5;
}


/*******************************************************************************
* Function Name: left_GetPriority
********************************************************************************
*
* Summary:
*   Gets the Priority of the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   Priority of the interrupt. 0 - 7, 0 being the highest.
*
*******************************************************************************/
uint8 left_GetPriority(void)
{
    uint8 priority;


    priority = *left_INTC_PRIOR >> 5;

    return priority;
}


/*******************************************************************************
* Function Name: left_Enable
********************************************************************************
*
* Summary:
*   Enables the interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void left_Enable(void)
{
    /* Enable the general interrupt. */
    *left_INTC_SET_EN = left__INTC_MASK;
}


/*******************************************************************************
* Function Name: left_GetState
********************************************************************************
*
* Summary:
*   Gets the state (enabled, disabled) of the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   1 if enabled, 0 if disabled.
*
*******************************************************************************/
uint8 left_GetState(void)
{
    /* Get the state of the general interrupt. */
    return ((*left_INTC_SET_EN & (uint32)left__INTC_MASK) != 0u) ? 1u:0u;
}


/*******************************************************************************
* Function Name: left_Disable
********************************************************************************
*
* Summary:
*   Disables the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void left_Disable(void)
{
    /* Disable the general interrupt. */
    *left_INTC_CLR_EN = left__INTC_MASK;
}


/*******************************************************************************
* Function Name: left_SetPending
********************************************************************************
*
* Summary:
*   Causes the Interrupt to enter the pending state, a software method of
*   generating the interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void left_SetPending(void)
{
    *left_INTC_SET_PD = left__INTC_MASK;
}


/*******************************************************************************
* Function Name: left_ClearPending
********************************************************************************
*
* Summary:
*   Clears a pending interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void left_ClearPending(void)
{
    *left_INTC_CLR_PD = left__INTC_MASK;
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
