#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

volatile int timer_flag_red;
volatile int timer_flag_green;

void btn_callback(uint gpio, uint32_t events)
{
    if (events == 0x4)
    {
        if (gpio == BTN_PIN_R)
        {
            flag_r = !flag_r;
        }
        if (gpio == BTN_PIN_G)
        {
            flag_g = !flag_g;
        }
    }
}

// Função de callback do timer
bool timer_callback_red(repeating_timer_t *t)
{
    timer_flag_red = 1; // seta a flag do timer
    return true;        // retorna true para continuar chamando a função
}

bool timer_callback_green(repeating_timer_t *t)
{
    timer_flag_green = 1; // seta a flag do timer
    return true;          // retorna true para continuar chamando a função
}

int main()
{
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);

    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    // declara variavel
    repeating_timer_t timer_red;
    repeating_timer_t timer_green;

    gpio_put(LED_PIN_R, 0);
    gpio_put(LED_PIN_G, 0);

    bool timer_init_red = false;
    bool timer_init_green = false;

    while (true)
    {

        if (flag_r)
        {
            if (!timer_init_red)
            {
                timer_init_red = true;
                if (!add_repeating_timer_ms(500, timer_callback_red, NULL, &timer_red))
                {
                    printf("Erro ao inicializar o timer\n");
                };
            }
            if (timer_flag_red)
            {
                gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
                timer_flag_red = 0;
            }
        }

        if (flag_g)
        {
            if (!timer_init_green)
            {
                timer_init_green = true;
                if (!add_repeating_timer_ms(250, timer_callback_green, NULL, &timer_green))
                {
                    printf("Erro ao inicializar o timer\n");
                };
            }
            if (timer_flag_green)
            {
                gpio_put(LED_PIN_G, !gpio_get(LED_PIN_G));
                timer_flag_green = 0;
            }
        }

        if (!flag_r && timer_init_red)
        {
            cancel_repeating_timer(&timer_red);
            timer_init_red = false;
            gpio_put(LED_PIN_R, 0);
        }
        if (!flag_g && timer_init_green)
        {
            cancel_repeating_timer(&timer_green);
            timer_init_green = false;
            gpio_put(LED_PIN_G, 0);
        };
    };
}
