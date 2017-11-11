/*******************************************************************************
* File Name: Trigger_Timer_PM.c
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

#include "Trigger_Timer.h"

static Trigger_Timer_backupStruct Trigger_Timer_backup;


/*******************************************************************************
* Function Name: Trigger_Timer_SaveConfig
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
*  Trigger_Timer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Trigger_Timer_SaveConfig(void) 
{
    #if (!Trigger_Timer_UsingFixedFunction)
        Trigger_Timer_backup.TimerUdb = Trigger_Timer_ReadCounter();
        Trigger_Timer_backup.InterruptMaskValue = Trigger_Timer_STATUS_MASK;
        #if (Trigger_Timer_UsingHWCaptureCounter)
            Trigger_Timer_backup.TimerCaptureCounter = Trigger_Timer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Trigger_Timer_UDB_CONTROL_REG_REMOVED)
            Trigger_Timer_backup.TimerControlRegister = Trigger_Timer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Trigger_Timer_RestoreConfig
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
*  Trigger_Timer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Trigger_Timer_RestoreConfig(void) 
{   
    #if (!Trigger_Timer_UsingFixedFunction)

        Trigger_Timer_WriteCounter(Trigger_Timer_backup.TimerUdb);
        Trigger_Timer_STATUS_MASK =Trigger_Timer_backup.InterruptMaskValue;
        #if (Trigger_Timer_UsingHWCaptureCounter)
            Trigger_Timer_SetCaptureCount(Trigger_Timer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Trigger_Timer_UDB_CONTROL_REG_REMOVED)
            Trigger_Timer_WriteControlRegister(Trigger_Timer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Trigger_Timer_Sleep
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
*  Trigger_Timer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Trigger_Timer_Sleep(void) 
{
    #if(!Trigger_Timer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Trigger_Timer_CTRL_ENABLE == (Trigger_Timer_CONTROL & Trigger_Timer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Trigger_Timer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Trigger_Timer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Trigger_Timer_Stop();
    Trigger_Timer_SaveConfig();
}


/*******************************************************************************
* Function Name: Trigger_Timer_Wakeup
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
*  Trigger_Timer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Trigger_Timer_Wakeup(void) 
{
    Trigger_Timer_RestoreConfig();
    #if(!Trigger_Timer_UDB_CONTROL_REG_REMOVED)
        if(Trigger_Timer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Trigger_Timer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
