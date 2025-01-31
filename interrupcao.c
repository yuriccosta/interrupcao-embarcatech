#include <stdio.h>
#include "pico/stdlib.h"
#include <hardware/pio.h>           
#include "hardware/clocks.h"        
#include "hardware/gpio.h"

#include "animacao_matriz.pio.h" // Biblioteca PIO para controle de LEDs WS2818B

// Definição de constantes
#define LED_PIN_RED 13
#define LED_COUNT 25            // Número de LEDs na matriz
#define MATRIZ_PIN 7            // Pino GPIO conectado aos LEDs WS2818B
#define BUTTON_PIN_A 5          // Pino GPIO conectado ao botão A
#define BUTTON_PIN_B 6          // Pino GPIO conectado ao botão B

// Declaração de variáveis globais
PIO pio;
uint sm;
volatile uint num = 0; // Variável para contar o número de interrupções
volatile uint32_t last_time = 0; // Variável para armazenar o tempo do último evento

double padrao[10][LED_COUNT] = {
        {0, 1, 1, 1, 0,
         0, 1, 0, 1, 0,
         0, 1, 0, 1, 0,
         0, 1, 0, 1, 0,
         0, 1, 1, 1, 0,
        }, // 0
        {0, 0, 1, 0, 0,
         0, 1, 1, 0, 0,
         1, 0, 1, 0, 0,
         0, 0, 1, 0, 0,
         0, 0, 1, 0, 0,
        }, // 1
        {0, 1, 1, 1, 0,
         0, 0, 0, 1, 0,
         0, 0, 1, 0, 0,
         0, 1, 0, 0, 0,
         0, 1, 1, 1, 0,
        },  // 2
        {0, 1, 1, 1, 0,
         0, 0, 0, 1, 0,
         0, 1, 1, 1, 0,
         0, 0, 0, 1, 0,
         0, 1, 1, 1, 0,
        }, // 3
        {0, 1, 0, 1, 0,
         0, 1, 0, 1, 0,
         0, 1, 1, 1, 0,
         0, 0, 0, 1, 0,
         0, 0, 0, 1, 0,
        }, // 4
        {0, 1, 1, 1, 0,
         0, 1, 0, 0, 0,
         0, 1, 1, 1, 0,
         0, 0, 0, 1, 0,
         0, 1, 1, 1, 0,
        }, // 5
        {0, 1, 1, 1, 0,
         0, 1, 0, 0, 0,
         0, 1, 1, 1, 0,
         0, 1, 0, 1, 0,
         0, 1, 1, 1, 0,
        },  // 6
        {0, 1, 1, 1, 0,
         0, 0, 0, 1, 0,
         0, 0, 1, 0, 0,
         0, 0, 1, 0, 0,
         0, 0, 1, 0, 0,
        }, // 7
        {0, 1, 1, 1, 0,
         0, 1, 0, 1, 0,
         0, 1, 1, 1, 0,
         0, 1, 0, 1, 0,
         0, 1, 1, 1, 0,
        }, // 8
        {0, 1, 1, 1, 0,
         0, 1, 0, 1, 0,
         0, 1, 1, 1, 0,
         0, 0, 0, 1, 0,
         0, 1, 1, 1, 0,
        }  // 9
    };

// Ordem da matriz de LEDS
int ordem[] = {0, 1, 2, 3, 4, 9, 8, 7, 6, 5, 10, 11, 12, 13, 14, 19, 18, 17, 16, 15, 20, 21, 22, 23, 24};  


//rotina para definição da intensidade de cores do led
uint32_t matrix_rgb(double b, double r, double g)
{
  // Para não ficar forte demais, a intensidade de cada cor é multiplicada por 50
  unsigned char R, G, B;
  R = r * 50; 
  G = g * 50;
  B = b * 50;
  return (G << 24) | (R << 16) | (B << 8);
}

void display_num(int number){
    uint32_t valor_led;

    for (int i = 0; i < LED_COUNT; i++){
        valor_led = matrix_rgb(0, padrao[number][ordem[24 - i]], 0);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

void gpio_irq_handler(uint gpio, uint32_t events) {
     // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    printf("num = %u\n", num);
    // Verificação de tempo para debounce
    if (current_time - last_time > 200){
        if(gpio == BUTTON_PIN_A){
            printf("Botão A pressionado\n");
            num++;
        } else if(gpio == BUTTON_PIN_B){
            printf("Botão B pressionado\n");
            num--;
        }
        printf("num = %u\n", num);

        display_num(num % 10);

        last_time = current_time; // Atualiza o tempo do último evento
    }
}

int main()
{
    pio = pio0; 
    bool ok;
    
    //coloca a frequência de clock para 128 MHz, facilitando a divisão pelo clock
    ok = set_sys_clock_khz(128000, false);

    stdio_init_all(); // Inicializa a comunicação serial

    printf("iniciando a transmissão PIO");
    if (ok) printf("clock set to %ld\n", clock_get_hz(clk_sys));

    //configurações da PIO
    uint offset = pio_add_program(pio, &animacao_matriz_program);
    sm = pio_claim_unused_sm(pio, true);
    animacao_matriz_program_init(pio, sm, offset, MATRIZ_PIN);


    // Configuração do LED RGB
    gpio_init(LED_PIN_RED);
    gpio_set_dir(LED_PIN_RED, GPIO_OUT);
    gpio_put(LED_PIN_RED, 0);


    // Configura os botões
    gpio_init(BUTTON_PIN_A);
    gpio_set_dir(BUTTON_PIN_A, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_A);

    gpio_init(BUTTON_PIN_B);
    gpio_set_dir(BUTTON_PIN_B, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_B);


    // Configuração da interrupção
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Começa a matriz com o número 0
    display_num(0);

    while (true) {
        // Pisca o led 5 vezes por segundo
        gpio_put(LED_PIN_RED, !gpio_get(LED_PIN_RED));
        sleep_ms(200);
    }
}
