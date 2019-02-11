#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>

#define OLED_I2C_ADDR   0b0111100
#define OLED_iic_cmd    0x80
#define OLED_I2C_DATA   0x40

void init_clocks(void);
void init_io(void);
void init_i2c(void);
void iic_cmd(uint8_t cmd);
void init_oled(void);
int main(void);

void init_clocks(void) {
  // internal oscillator (HSI) at 48MHz
  rcc_clock_setup_in_hsi_out_48mhz();
  // set ABP1 divider to 2 to achieve 24MHz (must be <36MHz)
  rcc_set_ppre1(RCC_CFGR_PPRE1_HCLK_DIV2);
}

void init_io(void) {
  //Enable GPIO C for LED on PC13
  rcc_periph_clock_enable(RCC_GPIOC);
  //Set PC13 as output
  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
}

void init_i2c(void) {
  //Based on: https://github.com/libopencm3/libopencm3-examples/blob/master/examples/stm32/f1/other/i2c_stts75_sensor/i2c_stts75_sensor.c
  //enable i2c clock and alternate function IO clock
  rcc_periph_clock_enable(RCC_I2C1);
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_AFIO);

  //Setup SDA and SCL pins
  gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, (GPIO_I2C1_SCL | GPIO_I2C1_SDA));

  i2c_peripheral_disable(I2C1);
  // This will set the FREQ bits used in CCR and TRISE calculations
  // This value is the APB1 clock and cannot be more than 36 MHz
  // That clock prescaler is set with rcc_set_ppre1()
  i2c_set_clock_frequency(I2C1, I2C_CR2_FREQ_24MHZ);
  // Use 100 kHz mode
  i2c_set_standard_mode(I2C1);
  // 48 MHz system clock / ABP1 Prescale of 2 = 24MHz
  // Reference manual section 26.6.8 CCR formula for 100 kHz i2c
  // 1000ns / 24(MHz) = 42
  // 5000ns / 42 = 199 = 0x77
  i2c_set_ccr(I2C1, 0x77); 
  // Reference manual section 26.6.9
  // 1000ns / 24(MHz) = 42
  // 1000ns / 42ns = 24 + 1 = 25
  i2c_set_trise(I2C1, 0x19);
  i2c_peripheral_enable(I2C1);
}

void iic_cmd(uint8_t cmd) {
  uint8_t data[2] = { 0x80, cmd };
  i2c_transfer7(I2C1, OLED_I2C_ADDR, data, 2, NULL, 0);
}

void init_oled(void) {
  //Initialization for a 128x32 OLED (SSD1306)

  //Set MUX Ratio
  iic_cmd(0xA8);
  iic_cmd(0x1F);  //32rows-1 = 0x1F

  //Set Display Offset
  iic_cmd(0xD3);
  iic_cmd(0x00);

  //Set Display Start Line
  iic_cmd(0x40);  //start line 0

  //Set Segment re-map
  iic_cmd(0xA0);

  //Set COM Output Scan Direction
  iic_cmd(0xC0);

  //Set COM Pins hardware configuration
  iic_cmd(0xDA);
  iic_cmd(0x02); 

  //Set Contrast Control
  iic_cmd(0x81);
  iic_cmd(0x7F);

  //Disable Entire Display On
  iic_cmd(0xA4);

  //Set Normal Display
  iic_cmd(0xA6);

  //Set horizontal addressing mode
  iic_cmd(0x20);
  iic_cmd(0x00);

  //Set column start and end address
  iic_cmd(0x21);
  iic_cmd(0x00);
  iic_cmd(0xFF);

  //Set page start and end address
  iic_cmd(0x22);
  iic_cmd(0x00);
  iic_cmd(0x03);

  //Set Osc Frequency
  iic_cmd(0xD5);
  iic_cmd(0x80); 

  //Enable charge pump regulator
  iic_cmd(0x8D);
  iic_cmd(0x14); 

  //Display On
  iic_cmd(0xAF); 
}

int main() {
  init_clocks();
  init_io();
  init_i2c();
  init_oled();

  /* //Seems like this block doesn't work
  i2c_send_start(I2C1);
  i2c_send_7bit_address (I2C1, OLED_I2C_ADDR, I2C_WRITE);
  i2c_send_data(I2C1,0x40);
  i2c_send_data(I2C1,0x00);
  i2c_send_data(I2C1,0x00);
  i2c_send_data(I2C1,0x00);
  i2c_send_data(I2C1,0x00);
  i2c_send_data(I2C1,0x00);
  i2c_send_data(I2C1,0x00);
  i2c_send_data(I2C1,0x00);
  i2c_send_stop(I2C1);
  */
  

  uint32_t i;
  uint8_t data[8] = { 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  i2c_transfer7(I2C1, OLED_I2C_ADDR, data, 8, NULL, 0);

  while (true) {
    gpio_toggle(GPIOC, GPIO13);

    for (i=0; i<1000000; i++) { __asm__("nop"); }
  }
  return 0;
}
