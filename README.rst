.. zephyr:code-sample:: UART
   :name: Blinky
   :relevant-api: gpio_interface, uart_interface

Adding board support
********************

To add support for your board, add something like this to your devicetree:

.. code-block:: DTS

/ {
    aliases{
        red = &red_led_10;
        blue = &blue_led_9;
    };
};
 
dut: &usart2 {
    dmas = <&dma1 7 STM32_DMA_PERIPH_TX>,
        <&dma1 6 STM32_DMA_PERIPH_RX>;
    dma-names = "tx", "rx";
};

tud: &usart3 {
	pinctrl-0 = <&usart3_tx_pb10 &usart3_rx_pb11>;
	pinctrl-names = "default";
	current-speed = <115200>;
	status = "okay";

    dmas = <&dma1 2 STM32_DMA_PERIPH_TX>,
        <&dma1 3 STM32_DMA_PERIPH_RX>;
    dma-names = "tx", "rx";
};

To build and flash the application into your board, type the following commands from a west workspace:
 - west build -b <reel_board>
 - west flash