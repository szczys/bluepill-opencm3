#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>


void init_clocks(void);
void init_io(void);
int main(void);

void init_clocks(void) {
  // internal oscillator (HSI) at 48MHz
  rcc_clock_setup_in_hsi_out_48mhz();

  // setup systick
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8); // 48 MHz / 8 = 6,000,000 counts per second
  systick_set_reload(5999999); //Reload for 1 second interval
  systick_interrupt_enable();
  systick_counter_enable();  
}

void init_io(void) {
  //Enable GPIO C for LED on PC13
  rcc_periph_clock_enable(RCC_GPIOC);
  //Set PC13 as output
  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
}

int main() {
  init_clocks();
  init_io();

  while (true) { ;; }
    return 0;
}

void sys_tick_handler(void) {
  gpio_toggle(GPIOC, GPIO13);
}
