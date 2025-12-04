#include <stdint.h>

#define PERIPH_BASE        0x40000000U
#define APB1PERIPH_BASE    PERIPH_BASE
#define AHB1PERIPH_BASE    (PERIPH_BASE + 0x00020000U)

#define GPIOA_BASE         (AHB1PERIPH_BASE + 0x0000)
#define USART2_BASE        (APB1PERIPH_BASE + 0x4400)
#define RCC_BASE           (AHB1PERIPH_BASE + 0x3800)

#define RCC_AHB1ENR        (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define RCC_APB1ENR        (*(volatile uint32_t *)(RCC_BASE + 0x40))

#define GPIOA_MODER        (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_AFRL         (*(volatile uint32_t *)(GPIOA_BASE + 0x20))

#define USART2_SR          (*(volatile uint32_t *)(USART2_BASE + 0x00))
#define USART2_DR          (*(volatile uint32_t *)(USART2_BASE + 0x04))
#define USART2_BRR         (*(volatile uint32_t *)(USART2_BASE + 0x08))
#define USART2_CR1         (*(volatile uint32_t *)(USART2_BASE + 0x0C))

static void uart2_init(void)
{
    /* Enable clocks */
    RCC_AHB1ENR |= (1 << 0);    // GPIOA clock enable
    RCC_APB1ENR |= (1 << 17);   // USART2 clock enable

    /* PA2 = USART2_TX (AF7), PA3 = USART2_RX (AF7) */
    GPIOA_MODER &= ~(0xF << 4);
    GPIOA_MODER |=  (0xA << 4);

    GPIOA_AFRL |= (7 << 8) | (7 << 12);

    /* Baud rate = 115200 (assuming 16 MHz HSI) */
    USART2_BRR = 0x8B;        // 16MHz/115200

    USART2_CR1 = (1 << 3) | (1 << 13);  // TE=1, UE=1
}

static void uart2_write(char c)
{
    while (!(USART2_SR & (1 << 7)));  // Wait until TXE=1
    USART2_DR = c;
}

static void uart2_print(char *s)
{
    while (*s) uart2_write(*s++);
}

int main(void)
{
    uart2_init();

    while (1)
    {
        uart2_print("Hello from STM32 bare-metal!\r\n");
        for(int i = 0; i < 500000; i++);  // Delay
    }
}
