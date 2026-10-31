// current code

#ifndef F_CPU
#define F_CPU 1000000UL // Assume 1 MHz clock frequency for _delay_ms() [18]
#endif

#include <avr/io.h>         // Provides definitions for I/O registers [18]
#include <stdint.h>    
#include <util/delay.h>     // For software delays, if needed [18]
#include <avr/interrupt.h>  // For global interrupt enable/disable [18]     // For integer types like uint8_t, uint16_t, uint32_t [18]
#include <stdlib.h>
#include <stdbool.h>
#include "USI_I2C_Master.h"
#include "DS3231.h"

// Global variable to store the time
volatile Time_t now = {1, 32};


// --- Button Pin Definitions (Port B) ---
#define HOUR_BUTTON PB0 // Button to add 1 hour [15]
#define MINUTE_BUTTON PB1 // Button to add 1 minute [15]

// to shift registers
#define SR_DATA   PA0
#define SR_LATCH  PA1
#define SR_CLOCK  PA2



// Reads time from DS3231 and updates PWM outputs for display.
void update_and_display_time(void){
    // Read current RTC time and update global variables `hours`, `minutes`
    now = RTC_get_time(); //update time
    // display rime here
    // Send time to shift registers
    displayTimeBinary(now.hh, now.mm); // Send hours/minutes to shift registers
}

void shiftOutByte(uint8_t data) {
    for (int i = 0; i < 4; i++) {
        if (data & (1 << i))
            PORTA |= (1 << SR_DATA);  // Set data high
        else
            PORTA &= ~(1 << SR_DATA); // Set data low
        
        // Clock pulse
        PORTA |= (1 << SR_CLOCK);
        _delay_ms(10);
        PORTA &= ~(1 << SR_CLOCK);
        _delay_ms(10);
    }
}

void displayTimeBinary(uint8_t hours, uint8_t minutes) {
    PORTA &= ~(1 << SR_LATCH); // Latch low

    shiftOutByte(minutes%10); 
    shiftOutByte(minutes/10);
    shiftOutByte(hours%10);
    shiftOutByte(hours/10);

    PORTA |= (1 << SR_LATCH); // Latch high
}

// --- Main Program ---
int main() {
    // Configure button pins (PB0, PB1) as inputs with internal pull-ups. [36, 62, 63]
    // DDRB &= ~((1 << PB0) | (1 << PB1)); // Clear DDB0 and DDB1 bits to configure as inputs [36, 62]
    DDRB &= ~((1 << HOUR_BUTTON) | (1 << MINUTE_BUTTON)); 
    // PORTB |= ((1 << PB0) | (1 << PB1)); // Set PORTB0 and PORTB1 bits to enable pull-ups [36, 62]
    PORTB |= ((1 << HOUR_BUTTON) | (1 << MINUTE_BUTTON)); 
    // configure shift regisers as outputs
    DDRA |= (1 << SR_DATA) | (1 << SR_LATCH) | (1 << SR_CLOCK);

    // initialize i2c
    I2C_init();
      
    update_and_display_time();
    
    // Initial delay for system stabilization [62]
    _delay_ms(100); 

    // --- Main Loop ---
    while (1) {
        // Continuously read time from DS3231 and update PWM displays
        update_and_display_time(); 
        for (int i = 0; i < 10; i++){
            // Button polling with debouncing for HOUR_BUTTON (PB0) [15]
            // Check if PB0 is low (button pressed) [64]
            if (!(PINB & (1 << HOUR_BUTTON))) { 
                _delay_ms(50); // Debounce delay [16]
                // Wait for button release to avoid multiple triggers from a single press [16]
                while (!(PINB & (1 << HOUR_BUTTON))); 
                _delay_ms(50); // Post-release debounce delay [16]
                // Increment hours, handling 24-hour rollover (0-23)
                now.hh = (now.hh + 1) % 24;
                // Write updated time to DS3231
                RTC_set_time(now);
                // Update display immediately after setting time
                update_and_display_time(); 
            }
            // Button polling with debouncing for MINUTE_BUTTON (PB1) [15]
            // Check if PB1 is low (button pressed) [17]
            if (!(PINB & (1 << MINUTE_BUTTON))) { 
                _delay_ms(50); // Debounce delay [17]
                // Wait for button release
                while (!(PINB & (1 << MINUTE_BUTTON))); 
                _delay_ms(50); // Post-release debounce
                // Increment minutes, handling 60-minute rollover (0-59)
                now.mm = (now.mm + 1) % 60;
                // Write updated time to DS3231
                RTC_set_time(now);
 
                // Update display immediately
                update_and_display_time(); 
            }
            _delay_ms(10); 
        }
    }
}