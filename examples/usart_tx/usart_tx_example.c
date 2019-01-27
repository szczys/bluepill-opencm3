#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>


void init_clocks(void);
void init_io(void);
void init_usart(void);
int main(void);

void init_clocks(void) {
  // internal oscillator (HSI) at 48MHz
  rcc_clock_setup_in_hsi_out_48mhz();
}

void init_io(void) {
  //Enable GPIO C for LED on PC13
  rcc_periph_clock_enable(RCC_GPIOC);
  //Set PC13 as output
  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
}

void init_usart(void) {
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_USART2);
  //Setup USART2 TX on PA2
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);

  usart_set_baudrate(USART2, 115200);
  usart_set_databits(USART2, 8);
  usart_set_stopbits(USART2, USART_STOPBITS_1);
  usart_set_mode(USART2, USART_MODE_TX_RX);
  usart_set_parity(USART2, USART_PARITY_NONE);
  usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

	usart_enable(USART2);
}

int main() {
  init_clocks();
  init_io();
  init_usart();
  
  uint32_t i;
  uint8_t count = 0;

  while (true) {
    gpio_toggle(GPIOC, GPIO13);
    usart_send_blocking(USART2, 'Y');
    usart_send_blocking(USART2, 'u');
    usart_send_blocking(USART2, 'p');
    usart_send_blocking(USART2, ' ');
    usart_send_blocking(USART2, '0' + count%10);
    usart_send_blocking(USART2, '\r');
    usart_send_blocking(USART2, '\n');

    ++count;
    for (i=0; i<1000000; i++) { __asm__("nop"); }
  }
  return 0;
}
