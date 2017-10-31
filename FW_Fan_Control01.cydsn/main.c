/*********************************************************************************
* Project Name:      Firmware Fan Control 
* Version:           1.20
*
* Device Used:       PSoC 3 CY8C3866AXI-040 or PSoC 5 CY8C5588AXI-060ES1
* Software Used:     PSoC Creator v2.1
* Compiler Used:     Keil(C51), ARM GNU CC
* Related Hardware:  CY8CKIT-001 PSoC DVK
*********************************************************************************
* Theory of Operation:
*
*  The Fan Controller is configured into the firmware (CPU) fan control mode
*  The speed control algorithm is implemented in the firmware
*  The firmware synchornizes to hardware using the end-of-cycle (eoc) pulse
*  2 Fans are supported (individual PWMs - no banks)
*
*  1st line of LCD displays Desired Speed and Actual Speed and PWM Duty Cycle of Fan 1
*  2nd line of LCD displays Actual Speed and PWM Duty Cycle of Fan 2
*  SW1 decreases desired speed in RPM
*  SW2 increases desired speed in RPM
*
********************************************************************************
* Copyright 2013, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include <device.h>
#include <stdio.h>

#if defined (__GNUC__)
    /* Add an explicit reference to the floating point printf library */
    /* to allow the usage of floating point conversion specifiers. */
    /* This is not linked in by default with the newlib-nano library. */
    asm (".global _printf_float");
#endif


/* PWM duty cycle controls - units are hundredths of a percent */
#define MIN_DUTY            (50u)
#define MAX_DUTY            (9950u)
#define DUTY_STEP_COARSE    (100u)
#define DUTY_STEP_FINE      (2u)

/* Speed controls - units are RPM */
#define MIN_RPM             (2500u)
#define MAX_RPM             (9500u)
#define INIT_RPM            (4500u)
#define RPM_STEP            (500u)
#define RPM_DELTA_LARGE     (500u)
#define RPM_TOLERANCE       (100u)

#define FAN                 (1u)

void main()
{
    uint16  desiredSpeed = INIT_RPM;
    uint16  dutyCycle;
    char    displayString[6];
    
    /* Globally Enable Interrupts to the CPU Core */
    CyGlobalIntEnable;
    
    FanController_Start();
    FanController_SetDesiredSpeed(FAN, desiredSpeed);
    dutyCycle = FanController_GetDutyCycle(FAN);
    
    LCD_Start();
    LCD_Position(0u, 0u);
    LCD_PrintDecUint16(desiredSpeed);
    
    ADC_DelSig_1_Start();

    /* Start the ADC conversion */
    ADC_DelSig_1_StartConvert();
    
    while(1u)
    {
        /* Synchronize firmware to end-of-cycle pulse from FanController */
        if(EOC_SR_Read())
        {
        
            /* Display Fan Actual Speeds */
            LCD_Position(0,5u);
            LCD_PrintDecUint16(FanController_GetActualSpeed(FAN));
            LCD_PrintString("   ");

            /* Firmware Speed Regulation */
            //LCD_Position(0,9u);
            
            /* Fan Below Desired Speed */
            /*if(FanController_GetActualSpeed(FAN) < desiredSpeed)
            {
                if((desiredSpeed - FanController_GetActualSpeed(FAN)) > RPM_DELTA_LARGE)
                {
                    dutyCycle += DUTY_STEP_COARSE;
                }
                else
                {
                    dutyCycle += DUTY_STEP_FINE;
                }
                if(dutyCycle > MAX_DUTY)
                {
                    dutyCycle = MAX_DUTY;
                }
            }*/
            /* Fan Above Desired Speed */
            /*else if(FanController_GetActualSpeed(FAN) > desiredSpeed)
            {
                if((FanController_GetActualSpeed(FAN) - desiredSpeed) > RPM_DELTA_LARGE)
                {
                    if(dutyCycle > (MIN_DUTY+DUTY_STEP_COARSE))
                    {
                        dutyCycle -= DUTY_STEP_COARSE;
                    }
                }
                else if((FanController_GetActualSpeed(FAN) - desiredSpeed) > RPM_TOLERANCE)
                {
                    if(dutyCycle > MIN_DUTY)
                    {
                        dutyCycle -= DUTY_STEP_FINE;
                    }
                }
            }
            FanController_SetDutyCycle(FAN, dutyCycle);*/

            /* Display Current Duty Cycle Settings (in 100ths of a percent) */
            LCD_Position(0,10u);
            sprintf(displayString, "%5.2f", (((float)dutyCycle)/100));
            LCD_PrintString(displayString);
            LCD_PrintString("%    ");
            
            
            // Check for potentiometer reading to change speed
            if(ADC_DelSig_1_IsEndConversion(ADC_DelSig_1_RETURN_STATUS))
            {
                uint16 output = ADC_DelSig_1_GetResult16() + 1; // Add one to overflow int at minimum value (was being read as max in value)
                LCD_Position(1u, 8u);
                LCD_PrintInt16(output);
                float speed = (((float) output)/(float)256 * (((float)MAX_RPM) - ((float)MIN_RPM))) + (float)MIN_RPM;
                float duty = (((float) output)/(float)256 * (((float)MAX_DUTY) - ((float)MIN_DUTY))) + (float)MIN_DUTY;
                dutyCycle = (uint16) duty;
                desiredSpeed = (uint16) speed;
                /* Display Updated Desired Speed */
                LCD_Position(1u, 0u);
                LCD_PrintDecUint16(desiredSpeed);
                FanController_SetDesiredSpeed(FAN, desiredSpeed);
                FanController_SetDutyCycle(FAN, dutyCycle);
                
            }
            
            //CyDelay(250u);
            
            /* Check for Button Press to Change Speed */
            /*if((!SW1_Read()) || (!SW2_Read()))
            {*/
                /* Decrease Speed */
                /*if(!SW1_Read())
                {
                    if(desiredSpeed > MIN_RPM)
                    {
                        desiredSpeed -= RPM_STEP;
                    }
                }*/
                
                /* Increase Speed */
                /*else
                {
                    desiredSpeed += RPM_STEP;
                    if(desiredSpeed > MAX_RPM)
                    {
                        desiredSpeed = MAX_RPM;
                    }
                }*/
    
                /* Display Updated Desired Speed */
                /*LCD_Position(0u, 0u);
                LCD_PrintDecUint16(desiredSpeed);
                FanController_SetDesiredSpeed(1u, desiredSpeed);
                dutyCycle = FanController_GetDutyCycle(1u);*/
                
                /* Switch Debounce */
                //CyDelay(250u);
            //}
        }
    }
}


/* [] END OF FILE */
