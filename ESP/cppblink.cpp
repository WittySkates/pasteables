/* Blink Example
 *
 * mcu: esp32
 * os: freertos
 * language: cpp
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

extern "C"{
void app_main();
}


#define BLINK_GPIO_1 GPIO_NUM_16
#define BLINK_GPIO_2 GPIO_NUM_15


void blink_task_1(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(BLINK_GPIO_1);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction((gpio_num_t)BLINK_GPIO_1, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        gpio_set_level((gpio_num_t)BLINK_GPIO_1, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level((gpio_num_t)BLINK_GPIO_1, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void blink_task_2(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(BLINK_GPIO_2);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction((gpio_num_t)BLINK_GPIO_2, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        gpio_set_level((gpio_num_t)BLINK_GPIO_2, 0);
        vTaskDelay(750 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level((gpio_num_t)BLINK_GPIO_2, 1);
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
}



void app_main(void)
{
    nvs_flash_init();
    xTaskCreate(&blink_task_1, "blink_task_1", 512, NULL, 5, NULL);
    xTaskCreate(&blink_task_2, "blink_task_2", 512, NULL, 5, NULL);
}
