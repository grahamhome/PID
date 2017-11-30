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

#define LCD_MAX_ECHO (LCD_Position(0u, 8u))
#define LCD_MIN_ECHO (LCD_Position(0u, 0u))
#define LCD_MIN_DUTY (LCD_Position(1u, 8u))
#define LCD_MAX_DUTY (LCD_Position(1u, 0u))

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

#include <device.h>
#include <end_echo_IRR.h>
#include <Echo_Timer.h>
#include <start_echo_IRR.h>
#include <stdio.h>
#include <time.h>


uint8 InterruptCnt;
uint32 echo=0;
uint32 max_echo = 0;

CY_ISR(start_echo_IRR_Interrupt)
{
    start_echo_IRR_ClearPending(); 
    
    Echo_Timer_Stop();
    Echo_Timer_Init();
    Echo_Timer_Start();
}
uint32 echoSum = 0;
uint8 factor = 5;
uint8 echoCount = 0;
CY_ISR(end_echo_IRR_Interrupt)
{

    end_echo_IRR_ClearPending();    
    uint32 rawEcho = Echo_Timer_ReadCounter();
    
    Echo_Timer_Stop();
}

void main()
{
    
    /* Enable the global interrupt */
    CyGlobalIntEnable;
    
    /* 1 - Enable the Interrupt component connected to Timer interrupt */
    /* 2 - Start the components */

    Trigger_Timer_Start();
    
    start_echo_IRR_StartEx(start_echo_IRR_Interrupt);
    start_echo_IRR_Start();
    
    end_echo_IRR_StartEx(end_echo_IRR_Interrupt);
    end_echo_IRR_Start();
    
    /* Start LCD */
    LCD_Start();
    
    uint16  desiredSpeed = INIT_RPM;
    
    
    /* Globally Enable Interrupts to the CPU Core */
    CyGlobalIntEnable;
    FanController_Start();
    FanController_SetDesiredSpeed(FAN, desiredSpeed);
    
    ADC_DelSig_1_Start();

    /* Start the ADC conversion */
    ADC_DelSig_1_StartConvert();
    
    
    // Determine minimum and maximum fan speed and min & max. distance readings
    //FanController_SetDesiredSpeed(FAN, desiredSpeed);
    
    
    // Figure out min. duty cycle by increasing fan speed from 0 until the ball begins to move
    // Start low for the min. and capture the value at which the ball starts to move.
    uint16 currentDuty = MIN_DUTY;
    FanController_SetDesiredSpeed(FAN, MAX_RPM);
    // Set the fan to min (not spinning)
    FanController_SetDutyCycle(FAN, currentDuty);
    // Wait for the ball to reach the bottom of the tube, and store the distance measured by the sensor at this time
    
    LCD_Position(0u, 0u); 
    LCD_PrintInt16(max_echo/2);
    while (1u) {
        // Check current distance compared to current max distance
        if (echo > max_echo) {
            max_echo = echo;
            LCD_Position(0u, 0u); 
            LCD_PrintInt16(max_echo/2);
        } else {
            break;
        }
        CyDelay(1000u);
    }
    
    while(1u) {
        LCD_Position(1u, 0u);
        if (echo > max_echo ) {
            LCD_PrintInt8(1u);
        } else {
            LCD_PrintInt8(0u);
        }
    }
    
    
    uint16 min_duty = 0;
    
    // Detect the minimum duty cycle needed to move the ball with this fan.
    while (1u) {
        
        /* Display position */
        //LCD_Position(0u, 0u);
        //LCD_PrintInt16(echo/2u);
        
        /* Synchronize firmware to end-of-cycle pulse from FanController */
        if(EOC_SR_Read())
        {
        
            // Check if min duty should be set (ball has moved)
            if (echo<max_echo){
                min_duty = currentDuty;
                /*LCD_MIN_ECHO;
                LCD_PrintInt16(min_duty/2u);
                LCD_MIN_DUTY;
                LCD_PrintInt16(min_duty);*/
                break;
            }
            
            currentDuty += DUTY_STEP_COARSE;
            FanController_SetDutyCycle(FAN, currentDuty);
            LCD_Position(0u, 8u); 
            LCD_PrintInt16(currentDuty);
    		CyDelay(100u);
         }
    }
    FanController_SetDutyCycle(FAN, MIN_DUTY);
    
    
    //while(1u) {
        
        /* Update fan speed */
        
        /* Synchronize firmware to end-of-cycle pulse from FanController */
        //if(EOC_SR_Read())
        //{
        
            /* Display Fan Actual Speeds */
            /*LCD_Position(0,5u);
            LCD_PrintDecUint16(FanController_GetActualSpeed(FAN));
            LCD_PrintString("   ");*/

            /* Display Current Duty Cycle Settings (in 100ths of a percent) */
           /* LCD_Position(0,10u);
            sprintf(displayString, "%5.2f", (((float)dutyCycle)/100));
            LCD_PrintString(displayString);
            LCD_PrintString("%    ");*/
            
            
            // Check for potentiometer reading to change speed
            //if(ADC_DelSig_1_IsEndConversion(ADC_DelSig_1_RETURN_STATUS))
            //{
                //uint16 output = ADC_DelSig_1_GetResult16() + 1; // Add one to overflow int at minimum value (was being read as max in value)
                //LCD_Position(1u, 8u);
                //LCD_PrintInt16(output);
                //float speed = (((float) output)/(float)256 * (((float)MAX_RPM) - ((float)MIN_RPM))) + (float)MIN_RPM;
                //float duty = (((float) output)/(float)256 * (((float)MAX_DUTY) - ((float)MIN_DUTY))) + (float)MIN_DUTY;
               // dutyCycle = (uint16) duty;
               // desiredSpeed = (uint16) speed;
                /* Display Updated Desired Speed */
                //LCD_Position(1u, 0u);
                //LCD_PrintDecUint16(desiredSpeed);
                //FanController_SetDesiredSpeed(FAN, desiredSpeed);
                //FanController_SetDutyCycle(FAN, dutyCycle);
                
            //}
        //}
    //}
}


/* [] END OF FILE */
