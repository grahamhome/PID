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

#define LCD_CUR_ECHO (LCD_Position(0u, 8u))
#define LCD_MAX_ECHO (LCD_Position(0u, 0u))
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

// Echo averaging factor - determined to give high accuracy and low latency of distance mesaurements
#define FACTOR              (8u)

#include <device.h>
#include <end_echo_IRR.h>
#include <Echo_Timer.h>
#include <start_echo_IRR.h>
#include <stdio.h>
#include <time.h>

CY_ISR(start_echo_IRR_Interrupt)
{
    start_echo_IRR_ClearPending(); 
    
    Echo_Timer_Stop();
    Echo_Timer_Init();
    Echo_Timer_Start();
}

uint32 echoes[FACTOR] = {0}; // The most recent echo values
uint8 position = 0; // The current index in the 'echoes' list

CY_ISR(end_echo_IRR_Interrupt)
{

    end_echo_IRR_ClearPending();    
    echoes[position++] = Echo_Timer_ReadCounter();
    position %= FACTOR;
    Echo_Timer_Stop();
}

/* Returns the average of the most recent echo values */
uint16 getEcho() {
    uint32 sum = 0;
    int i;
    for (i=0; i<FACTOR; i++) {
        sum += echoes[i];
    }
    return (sum/FACTOR)>>12; // Reduce the result to the significant digits only
}

// Constants for proportinal, integral and derivative terms
uint16 kP = 1;
uint16 kI = 0;
uint16 kD = 0;
    

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
    uint16 dutyCycle = MIN_DUTY;
    
    
    /* Globally Enable Interrupts to the CPU Core */
    CyGlobalIntEnable;
    FanController_Start();
    FanController_SetDesiredSpeed(FAN, desiredSpeed);
    
    ADC_DelSig_1_Start();

    /* Start the ADC conversion */
    ADC_DelSig_1_StartConvert();
    
    // Wait for the ball to reach the bottom of the tube, and store the distance measured by the sensor at this time
    CyDelay(4000);
    uint16 min_echo = getEcho();
    LCD_Position(0u, 0u);
    LCD_PrintInt16(min_echo);
    
    // Figure out min. duty cycle by increasing fan speed from 0 until the ball begins to move
    // Start low for the min. and capture the value at which the ball starts to move.
    uint16 minDuty = MIN_DUTY;
    FanController_SetDesiredSpeed(FAN, MAX_RPM);
    // Set the fan to min (not spinning)
    FanController_SetDutyCycle(FAN, minDuty);
    
    // Detect the minimum duty cycle needed to move the ball with this fan.
    while (1u) {
        
        /* Synchronize firmware to end-of-cycle pulse from FanController */
        if(EOC_SR_Read())
        {
            
            // Increase fan speed
            minDuty += DUTY_STEP_COARSE;
            FanController_SetDutyCycle(FAN, minDuty);
            CyDelay(500u);
            uint16 echo = getEcho();
        
            // Check if min duty should be set (ball has moved)
            if ((echo)>((min_echo))){
                break;
            }
            
            /* Display duty */
            LCD_Position(0u, 12u);
            LCD_PrintInt16(minDuty);
            
    		CyDelay(100u);
         }
    }
    // The duty cycle at which the ball will neither rise nor fall.
    // This duty cycle will be used when the control signal is 0.
    uint16 zero_point = minDuty - DUTY_STEP_COARSE;
    // The duty cycle at which the ball will fall as slowly as possible.
    // This duty cycle will be used whenever the control signal is negative
    uint16 negative_point = zero_point - DUTY_STEP_COARSE;
    
    
    //Let the ball move upwards until the maximum echo (minimum distance) reading is found, then stop the fan & wait 4 sec before continuing.
    uint16 max_echo = min_echo;
    uint16 no_change_counter = 0;
    while(1u) {
        CyDelay(500u);
        uint16 echo = getEcho();
        if (echo > max_echo) {
            max_echo = echo;
            no_change_counter = 0;
        } else {
            if (++no_change_counter > 10) {
                break;
            }
        }
    }
    
    LCD_Position(0u, 6u);
    LCD_PrintInt16(max_echo);
    
    FanController_SetDutyCycle(FAN, MIN_DUTY);
    CyDelay(4000u);
    
    
    while(1u) {
        
        /* Update fan speed */
        
        /* Synchronize firmware to end-of-cycle pulse from FanController */
        if(EOC_SR_Read())
        {
            
             // Check for potentiometer reading to change speed
            if(ADC_DelSig_1_IsEndConversion(ADC_DelSig_1_RETURN_STATUS))
            {
                uint16 output = ADC_DelSig_1_GetResult16() + 1; // Add one to overflow int at minimum value (was being read as max in value)
                //uint16 duty = (uint16) (((float) output)/(float)256 * (((float)MAX_DUTY) - ((float)MIN_DUTY))) + (float)MIN_DUTY;
                uint16 setPoint = (uint16)((((float) output)/(float)256 * (((float)max_echo) - ((float)min_echo))) + (float) min_echo);
                uint16 echo = getEcho();
                uint16 delta = setPoint - echo;
                uint16 control = kP*delta;
                
                LCD_Position(1u, 0u);
                LCD_PrintInt16(setPoint);
                LCD_Position(1u, 6u);
                LCD_PrintInt16(delta);
                LCD_Position(1u, 12u);
                LCD_PrintInt16(control);
                if (control > 0) {
                    FanController_SetDutyCycle(FAN, control);
                } else if (control < 0) {
                    FanController_SetDutyCycle(FAN, negative_point);
                } else {
                    FanController_SetDutyCycle(FAN, zero_point);
                }
            }
        }
    }
}


/* [] END OF FILE */
