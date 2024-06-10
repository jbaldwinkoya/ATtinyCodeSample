#include <avr/io.h>
#include <avr/interrupt.h>

void init_timer(uint8_t duration_ms, void (*funt)());
void init_pwm();
void timer_c();
void pwm_c();
void delay_c();
void start_pwm();
void stop_pwm();
void delay_ms(uint16_t ms, void (*funt)());

int main() {
    init_pwm();
    init_timer(1000, pwm_c);
    delay_ms(5000, delay_c);
    sei(); //Enables interrupts by setting the global interrupt mask. 
    while (1) {}
    return 0;
}

void init_timer(uint8_t duration_ms, void (*funt)()) {
    uint16_t compare_value = (uint16_t)((duration_ms * 1000) / 64) - 1;

    // Set timer mode and prescaler
    TCB0.CTRLA |= (1 << TCB_CLKSEL0_bp) | (1 << TCB_CLKSEL1_bp); // Clock source: CLK_PER / 64
    TCB0.CCMP = compare_value;

    // Enable timer interrupt
    TCB0.INTCTRL |= (1 << TCB_CAPT_bp); 

    timer_c = funt;
}

void init_pwm() {
    // Set PB0 (PWM0) as output
    PORTB.DIR |= (1 << PIN0_bp);

    // Set non-inverting PWM mode
    TCA0.SINGLE.CTRLB |= (1 << TCA_SINGLE_CMP0EN_bp) | (1 << TCA_SINGLE_CMP0OV_bp); // Enable PWM output on compare channel 0

    // Set clock source and prescaler
    TCA0.SINGLE.CTRLA |= (1 << TCA_SINGLE_CLKSEL0_bp) | (1 << TCA_SINGLE_CLKSEL1_bp); // Clock source: CLK_PER / 2
}

void start_pwm() {
    // Start timer
    TCA0.SINGLE.CTRLA |= (1 << TCA_SINGLE_ENABLE_bp);
}

void stop_pwm() {
    // Stop timer
    TCA0.SINGLE.CTRLA &= ~(1 << TCA_SINGLE_ENABLE_bp);
}

void delay_ms(uint16_t ms, void (*funt)()) {
    // Calculate timer compare value for the given duration
    uint16_t compare_value = (uint16_t)((ms * 1000) / 64) - 1;

    // Set timer mode and prescaler
    TCB0.CTRLA |= (1 << TCB_ENABLE_bp) | (1 << TCB_CLKSEL0_bp) | (1 << TCB_CLKSEL1_bp); // Enable timer, clock source: CLK_PER / 64
    TCB0.CCMP = compare_value;

    // Enable timer interrupt
    TCB0.INTCTRL |= (1 << TCB_CAPT_bp);

    delay_c = funt;
}

ISR(TCB1_INT_vect) {
    timer_c();
}

ISR(TCB0_INT_vect) {
    delay_c();
}

void pwm_c() {
    PORTB.OUTTGL |= (1 << PIN0_bp); // Toggle PWM pin
}

void delay_c() {
    printf("Delay completed.\n");
}