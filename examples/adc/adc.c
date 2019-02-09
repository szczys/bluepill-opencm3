#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/adc.h>

void init_clocks(void);
void init_io(void);
void init_usart(void);
void init_adc(void);
static void my_usart_print_int(uint32_t usart, int value);
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

void init_adc(void) {
  //Boilerplate: http://libopencm3.org/docs/latest/stm32f1/html/group__adc__file.html
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_ADC1EN);
	adc_power_off(ADC1);
	rcc_peripheral_reset(&RCC_APB2RSTR, RCC_APB2RSTR_ADC1RST);
	rcc_peripheral_clear_reset(&RCC_APB2RSTR, RCC_APB2RSTR_ADC1RST);
	rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV2);
	adc_set_dual_mode(ADC_CR1_DUALMOD_IND);
	adc_disable_scan_mode(ADC1);
	adc_set_single_conversion_mode(ADC1);
	adc_set_sample_time(ADC1, ADC_CHANNEL0, ADC_SMPR_SMP_1DOT5CYC);
	adc_enable_external_trigger_regular(ADC1, ADC_CR2_EXTSEL_SWSTART);
	adc_power_on(ADC1);
	adc_reset_calibration(ADC1);
	adc_calibrate(ADC1);
}

static void my_usart_print_int(uint32_t usart, int value)
{
	int8_t i;
	uint8_t nr_digits = 0;
	char buffer[25];

	if (value < 0) {
		usart_send(usart, '-');
		value = value * -1;
	}

	while (value > 0) {
		buffer[nr_digits++] = "0123456789"[value % 10];
		value /= 10;
	}

	for (i = nr_digits; i >= 0; i--)
		usart_send_blocking(usart, buffer[i]);
}

int main() {
  init_clocks();
  init_io();
  init_usart();
  init_adc();
  
  uint32_t i;
  uint8_t count = 0;

  uint16_t temp;

  while (true) {
    //gpio_toggle(GPIOC, GPIO13);
    
    usart_send_blocking(USART2, 'Y');
    usart_send_blocking(USART2, 'u');
    usart_send_blocking(USART2, 'p');
    usart_send_blocking(USART2, ' ');
    usart_send_blocking(USART2, '0' + count%10);
    usart_send_blocking(USART2, '\r');
    usart_send_blocking(USART2, '\n');
    
    ++count;
    for (i=0; i<1000000; i++) { __asm__("nop"); }

    adc_start_conversion_regular(ADC1);
    while (! adc_eoc(ADC1));
    temp = adc_read_regular(ADC1); 
  
    gpio_toggle(GPIOC, GPIO13);
    my_usart_print_int(USART2, temp);
    usart_send_blocking(USART2, '\r');
    usart_send_blocking(USART2, '\n');
  }
  return 0;
}
