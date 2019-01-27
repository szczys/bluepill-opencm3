#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>


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
  rcc_periph_clock_enable(RCC_AFIO);
  rcc_periph_clock_enable(RCC_USART2);

  //Enable USART2 RX Int
  nvic_enable_irq(NVIC_USART2_IRQ);

  //Configure USART2 RX on PA3
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART2_RX);

  usart_set_baudrate(USART2, 115200);
  usart_set_databits(USART2, 8);
  usart_set_stopbits(USART2, USART_STOPBITS_1);
  usart_set_mode(USART2, USART_MODE_TX_RX);
  usart_set_parity(USART2, USART_PARITY_NONE);
  usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

  // Enable USART2 RX Int
  USART_CR1(USART2) |= USART_CR1_RXNEIE;

  usart_enable(USART2);
}


int main() {
  init_clocks();
  init_io();
  init_usart();

  while (true) { ;; }
  return 0;
}

void usart2_isr(void) {
  static uint8_t data = 'A';

  //This could be an RX or a TX interrupt. This verifies that it' an RX Int
  if (((USART_CR1(USART2) & USART_CR1_RXNEIE) != 0) && ((USART_SR(USART2) & USART_SR_RXNE) != 0)) {

    //Check if character recevied was an L (toggle LED if so)
    data = usart_recv(USART2);
    if (data == 'L') { gpio_toggle(GPIOC, GPIO13); }
  }
}
