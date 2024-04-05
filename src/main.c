#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/__assert.h>
 
 
#define PRIORITY 7
#define STACKSIZE 1024
 
volatile uint8_t received = 0;
volatile uint8_t send = 0;
 
static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
    switch (evt->type) {
    
    case UART_TX_DONE:
        send = 1;
        break;
 
    case UART_TX_ABORTED:
        // do something
        break;
        
    case UART_RX_RDY:
        // do something
        break;
 
    case UART_RX_BUF_REQUEST:
        // do something
        break;
 
    case UART_RX_BUF_RELEASED:
        // do something
        break;
        
    case UART_RX_DISABLED:
        received = 1;
        break;
 
    case UART_RX_STOPPED:
        // do something
        break;
        
    default:
        break;
    }
}
 
 
void transmiting(void){
    static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(blue), gpios);
    
    if(!gpio_is_ready_dt(&led)){
        while(1){}
    }
 
    if(gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE) < 0){
        while(1){}
    }
 
    const struct device *const uart = DEVICE_DT_GET(DT_NODELABEL(dut));
    if(!device_is_ready(uart)){
        while(1){}
    }
    if(uart_callback_set(uart, uart_cb, NULL) < 0){
        while(1){}
    }
    uint8_t tx_buff[100] = {};
    for(int i = 0; i < sizeof(tx_buff);i++)
        tx_buff[i] = i + 1;
    uart_tx(uart, tx_buff, sizeof(tx_buff), SYS_FOREVER_US);
 
 
    while(1){
        
        if(send){
            send = 0;
            gpio_pin_toggle_dt(&led);
            k_msleep(250);
            gpio_pin_toggle_dt(&led);
            k_msleep(250);
            for(int i = 0; i < sizeof(tx_buff); i++){
                tx_buff[i] ++;
            }
            uart_tx(uart, tx_buff, sizeof(tx_buff), SYS_FOREVER_US);
        }
        k_yield();
    }
 
}
 
void receiving(){
 
    static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(red), gpios);
    
    if(!gpio_is_ready_dt(&led)){
        while(1){}
    }
 
    if(gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE) < 0){
        while(1){}
    }
 
    const struct device *const uart = DEVICE_DT_GET(DT_NODELABEL(tud));
    if(!device_is_ready(uart)) {
        while(1){}
    }
    if(uart_callback_set(uart, uart_cb, NULL) < 0){
        while(1){}
    }
    uint8_t rx_buff[100] = {};
    uart_rx_enable(uart, rx_buff, sizeof(rx_buff),  SYS_FOREVER_US);
 
 
    while(1){
        
 
        if(received){
            for(int i = 0; i < sizeof(rx_buff); i++)
                printk("rx[%d] = %d\n", i + 1, rx_buff[i]);
            gpio_pin_toggle_dt(&led);
            k_msleep(100);
            gpio_pin_toggle_dt(&led);
            uart_rx_enable(uart, rx_buff, sizeof(rx_buff),  SYS_FOREVER_US);
            received = 0;
        }
        k_yield();
    }
}
 
K_THREAD_DEFINE(transmit_id, STACKSIZE, transmiting, NULL, NULL, NULL,
        2, 0, 0);
K_THREAD_DEFINE(receive_id, STACKSIZE, receiving, NULL, NULL, NULL,
        2, 0, 0);