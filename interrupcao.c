#include <stdio.h>
#include "pico/stdlib.h"
#include <hardware/pio.h>           
#include "hardware/clocks.h"        
#include "hardware/gpio.h"

#include "animacao_matriz.pio.h" // Biblioteca PIO para controle de LEDs WS2818B

#define LED_PIN_RED 13
#define LED_COUNT 25            // Número de LEDs na matriz
#define MATRIZ_PIN 7               // Pino GPIO conectado aos LEDs WS2818B
#define BUTTON_PIN_A 5               // Pino GPIO conectado ao botão A
#define BUTTON_PIN_B 6               // Pino GPIO conectado ao botão B

volatile int num = 0; // Variável para contar o número de interrupções

//rotina para definição da intensidade de cores do led
uint32_t matrix_rgb(double b, double r, double g)
{
  unsigned char R, G, B;
  R = r * 255;
  G = g * 255;
  B = b * 255;
  return (G << 24) | (R << 16) | (B << 8);
}

int main()
{
    PIO pio = pio0; 
    bool ok;
    
    //coloca a frequência de clock para 128 MHz, facilitando a divisão pelo clock
    ok = set_sys_clock_khz(128000, false);

    stdio_init_all(); // Inicializa a comunicação serial

    printf("iniciando a transmissão PIO");
    if (ok) printf("clock set to %ld\n", clock_get_hz(clk_sys));

    //configurações da PIO
    uint offset = pio_add_program(pio, &animacao_matriz_program);
    uint sm = pio_claim_unused_sm(pio, true);
    animacao_matriz_program_init(pio, sm, offset, MATRIZ_PIN);


    // Configuração do LED RGB
    gpio_init(LED_PIN_RED);
    gpio_set_dir(LED_PIN_RED, GPIO_OUT);


    // Configura os botões
    gpio_init(BUTTON_PIN_A);
    gpio_set_dir(BUTTON_PIN_A, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_A);

    gpio_init(BUTTON_PIN_B);
    gpio_set_dir(BUTTON_PIN_B, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_B);


    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
