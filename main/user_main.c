

#include <stdio.h>
#include "esp_system.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#define BUF_SIZE (1024) // UART buffer size (in words?)
//
//      Pin definitions
//
#define R_PIN 14 // NodeMCU D5
#define G_PIN  5 // NodeMCU D2
#define B_PIN  4 // NodeMCU D1
#define OUTPUT_PIN_MASK ((1ULL << B_PIN) | (1ULL << G_PIN) | (1ULL << R_PIN)) // All output pins
//
//      Type definitions
//
typedef enum {BAD, OK, GOOD} mood_t;
//
//      Init functions
//
static void uart_init(){
    uart_config_t uc = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uc);
    uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL);
}

static void gpio_init(){
    gpio_config_t gpioc;                    // Create config variable
    gpioc.intr_type = GPIO_INTR_DISABLE;    // Disable interrupt
    gpioc.mode = GPIO_MODE_OUTPUT;          // Set pin mode
    gpioc.pin_bit_mask = OUTPUT_PIN_MASK;   // Pin mask of pins that config affects
    gpioc.pull_down_en = 0;                 // Disable pulldown
    gpioc.pull_up_en = 0;                   // Disable pullup
    gpio_config(&gpioc);                    // Configure the pins
}
//
//      Tasks
//
static void fsm(){
    mood_t mood = OK;
    while(1){
        switch(mood){
            case BAD:
                gpio_set_level(R_PIN, 1);
                break;
            case OK:
                gpio_set_level(B_PIN, 1);
                break;
            case GOOD:
                gpio_set_level(G_PIN, 1);
                break;
        }

    }
}

static void spamTask(){
    const TickType_t d = 500;
    int var = 1;

    while(1){
        gpio_set_level(R_PIN, var);
        gpio_set_level(G_PIN, var);
        gpio_set_level(B_PIN, var);
        if(var == 0){
            var = 1;
        }
        else{
            var = 0;
        }
        uart_write_bytes(UART_NUM_0, "HELLO", 5);

        vTaskDelay(d);
    }
}
//
//      Main:
//
void app_main(void){
    uart_init();
    gpio_init();

    xTaskCreate(spamTask, "spam_task", 1000, NULL, 10, NULL);
}
