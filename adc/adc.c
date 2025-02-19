#include <stdio.h>
#include <stdlib.h>
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "ssd1306.h"
#include "font.h"
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define JOYSTICK_X_PIN 26  // GPIO para eixo X
#define JOYSTICK_Y_PIN 27  // GPIO para eixo Y
#define JOYSTICK_PB 22 // GPIO para botão do Joystick
#define BOTAOA 5 
#define BOTAOB 6
#define LEDG 11
#define LEDB 12
#define LEDR 13

//variáveis globais
static volatile uint32_t lastEventButton = 0; // Armazena o tempo do último evento (em microssegundos)
ssd1306_t ssd; // Inicializa a estrutura do display
const float clock = 255.0;
const uint16_t wrap = 4095;
uint16_t sliceBlue;
uint16_t sliceRed;
bool pwmonoff = true;  //controle do pwm


// Função de interrupção com debouncing
void gpio_irq_handler_BOTAO(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - lastEventButton > 200000) // 200 ms de debouncing
    {
            lastEventButton = current_time;
            if(gpio == BOTAOA)
            {
                //habilita ou desabilita os leds pwm
                pwm_set_enabled(sliceBlue, pwmonoff = !pwmonoff);
                pwm_set_enabled(sliceRed, pwmonoff);
                
            }

            if (gpio == BOTAOB)
            {
                reset_usb_boot(0 , 0);
            }

            if(gpio == JOYSTICK_PB)
            {
                //liga/desliga o led verde
                gpio_put(LEDG, !gpio_get(LEDG));
                
                //adição de uma borda a mais no display, "efeito 3d" na borda
                ssd1306_rect(&ssd, 6, 6, 115, 55, gpio_get(LEDG), 0); 
            }
                                            
    }
}

int main()
{
  //inicialização botoes e leds
  gpio_init(BOTAOB);
  gpio_set_dir(BOTAOB, GPIO_IN);
  gpio_pull_up(BOTAOB);
  gpio_set_irq_enabled_with_callback(BOTAOB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler_BOTAO);

  gpio_init(JOYSTICK_PB);
  gpio_set_dir(JOYSTICK_PB, GPIO_IN);
  gpio_pull_up(JOYSTICK_PB);
  gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler_BOTAO);

  gpio_init(BOTAOA);
  gpio_set_dir(BOTAOA, GPIO_IN);
  gpio_pull_up(BOTAOA);
  gpio_set_irq_enabled_with_callback(BOTAOA, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler_BOTAO);

  gpio_init(LEDG);
  gpio_set_dir(LEDG, GPIO_OUT);
  gpio_pull_up(LEDG);

  //led azul iniciado como pwm
  gpio_set_function(LEDB, GPIO_FUNC_PWM);
  sliceBlue = pwm_gpio_to_slice_num(LEDB);
  pwm_set_clkdiv(sliceBlue, clock);
  pwm_set_wrap(sliceBlue, wrap);
  pwm_set_gpio_level(LEDB, 0);
  pwm_set_enabled(sliceBlue, true);
  
  //led vermelho iniciado como pwm
  gpio_set_function(LEDR, GPIO_FUNC_PWM);
  sliceRed = pwm_gpio_to_slice_num(LEDR);
  pwm_set_clkdiv(sliceRed, clock);
  pwm_set_wrap(sliceRed, wrap);
  pwm_set_gpio_level(LEDR, 0);
  pwm_set_enabled(sliceRed, true); 



  //inicialização do display 
  i2c_init(I2C_PORT, 400 * 1000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  ssd1306_rect(&ssd, 3, 3, 122, 60, 1, 0); // Desenha um retângulo

  //inicialização do adc
  adc_init();
  adc_gpio_init(JOYSTICK_X_PIN);
  adc_gpio_init(JOYSTICK_Y_PIN);
  

  //variaveis para auxiliar na ilusão de "movimento" do quadrado
  uint16_t adc_value_x;
  uint16_t adc_value_y;

  uint16_t adc_value_x_antes;
  uint16_t adc_value_y_antes;

  uint16_t x_correcao;
  uint16_t y_correcao; 
  
  //leitura dos valores do adc
  adc_select_input(1);
  adc_value_x_antes = adc_read();
  x_correcao = adc_read();

  adc_select_input(0);
  y_correcao = adc_read();
  adc_value_y_antes = adc_read();

  while (true)
  {
    adc_select_input(1); // Seleciona o ADC para eixo X. O pino 26 como entrada analógica
    adc_value_x = adc_read();
    pwm_set_gpio_level(LEDR, abs(adc_value_x - x_correcao)); //O joystick não se mantém em um valor fixo então uma correção é necessária


    adc_select_input(0); // Seleciona o ADC para eixo Y. O pino 27 como entrada analógica
    adc_value_y = adc_read();
    pwm_set_gpio_level(LEDB, abs(adc_value_y - y_correcao)); //O joystick não se mantém em um valor fixo então uma correção é necessária

    //correção dos limites até onde o quadrado pode ir, necessário para ele não atravessar e "comer" os pixels da borda
    adc_value_x = adc_value_x/39 + 8;           
    adc_value_y = (4096 - adc_value_y)/91 + 7;  

    //apaga o retangulo na posicao anterior e desenha na proxima, dando a de movimento
    ssd1306_rect(&ssd, adc_value_y_antes, adc_value_x_antes, 8, 8, 0, 1);
    ssd1306_rect(&ssd, adc_value_y, adc_value_x, 8, 8, 1, 1);

    //atualização dos valores para o próximo loop
    adc_value_x_antes = adc_value_x;
    adc_value_y_antes = adc_value_y;

    

    ssd1306_send_data(&ssd); // Atualiza o display


    sleep_ms(100);
  }
}