#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

int main() {
    // First, let's ensure that our clock is running off the high-speed
    // internal oscillator (HSI) at 48MHz
    rcc_clock_setup_in_hsi_out_48mhz();

    // Since our LED is on GPIO bank A, we need to enable
    // the peripheral clock to this GPIO bank in order to use it.
    rcc_periph_clock_enable(RCC_GPIOC);

    // Our test LED is connected to Port A pin 11, so let's set it as output
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

    // Now, let's forever toggle this LED back and forth
    uint32_t i;
    while (true) {
        gpio_toggle(GPIOC, GPIO13);
	for (i=0; i<1000000; i++) {
		__asm__("nop");
	}
    }

    return 0;
}
