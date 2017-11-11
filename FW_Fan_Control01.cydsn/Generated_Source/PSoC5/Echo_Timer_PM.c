/*******************************************************************************
* File Name: Echo_Timer_PM.c
* Version 2.60
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Echo_Timer.h"

static Echo_Timer_backupStruct Echo_Timer_backup;


/*******************************************************************************
* Function Name: Echo_Timer_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Echo_Timer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Echo_Timer_SaveConfig(void) 
{
    #if (!Echo_Timer_UsingFixedFunction)
        Echo_Timer_backup.TimerUdb = Echo_Timer_ReadCounter();
        Echo_Timer_backup.InterruptMaskValue = Echo_Timer_STATUS_MASK;
        #if (Echo_Timer_UsingHWCaptureCounter)
            Echo_Timer_backup.TimerCaptureCounter = Echo_Timer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Echo_Timer_UDB_CONTROL_REG_REMOVED)
            Echo_Timer_backup.TimerControlRegister = Echo_Timer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Echo_Timer_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Echo_Timer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Echo_Timer_RestoreConfig(void) 
{   
    #if (!Echo_Timer_UsingFixedFunction)

        Echo_Timer_WriteCounter(Echo_Timer_backup.TimerUdb);
        Echo_Timer_STATUS_MASK =Echo_Timer_backup.InterruptMaskValue;
        #if (Echo_Timer_UsingHWCaptureCounter)
            Echo_Timer_SetCaptureCount(Echo_Timer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Echo_Timer_UDB_CONTROL_REG_REMOVED)
            Echo_Timer_WriteControlRegister(Echo_Timer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Echo_Timer_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Echo_Timer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Echo_Timer_Sleep(void) 
{
    #if(!Echo_Timer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Echo_Timer_CTRL_ENABLE == (Echo_Timer_CONTROL & Echo_Timer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Echo_Timer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Echo_Timer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Echo_Timer_Stop();
    Echo_Timer_SaveConfig();
}


/*******************************************************************************
* Function Name: Echo_Timer_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Echo_Timer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Echo_Timer_Wakeup(void) 
{
    Echo_Timer_RestoreConfig();
    #if(!Echo_Timer_UDB_CONTROL_REG_REMOVED)
        if(Echo_Timer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Echo_Timer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
