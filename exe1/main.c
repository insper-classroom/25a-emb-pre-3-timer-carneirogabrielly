#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;
volatile int g_timer_flag; // flag para o timer

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        flag_f_r = !flag_f_r;
    } else if (events == 0x8) { // rise edge
    }
}

// Função de callback do timer
bool timer_callback(repeating_timer_t *t) {
    g_timer_flag = 1;       // seta a flag do timer
    return true;        // retorna true para continuar chamando a função
}


int main() {
    stdio_init_all();
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);

    // declara variavel
    repeating_timer_t timer;
    // os parametros sao, nessa ordem, tempo que repete a função de callback, a funcao que repete, argumentos, ponteiro para o timer
    

    gpio_put(LED_PIN_R, 0);

    bool timer_init = false;

    while (true) {
        if (flag_f_r) {

            if (!timer_init) {
                timer_init = true;
                // inicializa o timer   
                if (!add_repeating_timer_ms(500, timer_callback, NULL, &timer)) {
                    printf("Erro ao inicializar o timer\n");
                }
            }

            // cada vez que chama a timer callback muda o status do led
            if (g_timer_flag) {
                int led_status = !gpio_get(LED_PIN_R);
                gpio_put(LED_PIN_R, led_status);
                g_timer_flag = 0;   
            };
            
        } else if (timer_init && !flag_f_r) {
            // desliga o timer
            cancel_repeating_timer(&timer);
            timer_init = false;
            gpio_put(LED_PIN_R, 0);
        }
    }
}
