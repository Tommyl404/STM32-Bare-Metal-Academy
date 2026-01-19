/**
 ******************************************************************************
 * @file           : exti_tutorial.c
 * @brief          : Learning EXTI without HAL - Bare Metal
 ******************************************************************************
 * 
 *  ███████╗██╗  ██╗████████╗██╗    ████████╗██╗   ██╗████████╗ ██████╗ ██████╗ 
 *  ██╔════╝╚██╗██╔╝╚══██╔══╝██║    ╚══██╔══╝██║   ██║╚══██╔══╝██╔═══██╗██╔══██╗
 *  █████╗   ╚███╔╝    ██║   ██║       ██║   ██║   ██║   ██║   ██║   ██║██████╔╝
 *  ██╔══╝   ██╔██╗    ██║   ██║       ██║   ██║   ██║   ██║   ██║   ██║██╔══██╗
 *  ███████╗██╔╝ ██╗   ██║   ██║       ██║   ╚██████╔╝   ██║   ╚██████╔╝██║  ██║
 *  ╚══════╝╚═╝  ╚═╝   ╚═╝   ╚═╝       ╚═╝    ╚═════╝    ╚═╝    ╚═════╝ ╚═╝  ╚═╝
 * 
 *  INTERACTIVE LEARNING: EXTI (External Interrupt/Event Controller)
 * 
 *  WHAT YOU'LL LEARN:
 *  1. How external interrupts work
 *  2. How to configure GPIO as interrupt source
 *  3. How to configure edge detection (rising/falling)
 *  4. How to handle the interrupt
 * 
 *  HARDWARE:
 *  - Nucleo-H753ZI has a USER button on PC13
 *  - We'll use it to trigger an interrupt
 * 
 *  DIFFICULTY: ⭐⭐⭐ (Intermediate)
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 * 
 *  LESSON 0: WHAT IS EXTI?
 *  ========================
 * 
 *  EXTI = External Interrupt/Event Controller
 *  
 *  It detects external events (like button presses) and:
 *  - Triggers an interrupt (CPU handles it)
 *  - Generates an event (wakes up peripherals)
 *  
 *  EDGE DETECTION:
 *  
 *  Signal: ────┐     ┌─────┐     ┌────
 *              │     │     │     │
 *              └─────┘     └─────┘
 *              ↑     ↑     ↑     ↑
 *         Falling Rising Falling Rising
 *           Edge   Edge    Edge   Edge
 *  
 *  You can trigger on:
 *  - Rising edge (0→1)
 *  - Falling edge (1→0)
 *  - Both edges
 *  
 *  EXTI LINES:
 *  - Lines 0-15: GPIO pins (Px0-Px15)
 *  - Lines 16+: Internal events (PVD, RTC, USB, etc.)
 * 
 * ============================================================================ */

/* ============================================================================
 *  PERIPHERAL BASE ADDRESSES
 * ============================================================================ */
#define RCC_BASE        0x58024400UL
#define GPIOB_BASE      0x58020400UL
#define GPIOC_BASE      0x58020800UL
#define EXTI_BASE       0x58000000UL
#define SYSCFG_BASE     0x58000400UL

/* ============================================================================
 *  RCC REGISTERS
 * ============================================================================ */
typedef struct {
    volatile uint32_t CR;
    volatile uint32_t HSICFGR;
    volatile uint32_t CRRCR;
    volatile uint32_t CSICFGR;
    volatile uint32_t CFGR;
    volatile uint32_t RESERVED1;
    volatile uint32_t D1CFGR;
    volatile uint32_t D2CFGR;
    volatile uint32_t D3CFGR;
    volatile uint32_t RESERVED2;
    volatile uint32_t PLLCKSELR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t PLL1DIVR;
    volatile uint32_t PLL1FRACR;
    volatile uint32_t PLL2DIVR;
    volatile uint32_t PLL2FRACR;
    volatile uint32_t PLL3DIVR;
    volatile uint32_t PLL3FRACR;
    volatile uint32_t RESERVED3;
    volatile uint32_t D1CCIPR;
    volatile uint32_t D2CCIP1R;
    volatile uint32_t D2CCIP2R;
    volatile uint32_t D3CCIPR;
    volatile uint32_t RESERVED4;
    volatile uint32_t CIER;
    volatile uint32_t CIFR;
    volatile uint32_t CICR;
    volatile uint32_t RESERVED5;
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
    volatile uint32_t RESERVED6;
    volatile uint32_t AHB3RSTR;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    volatile uint32_t AHB4RSTR;
    volatile uint32_t APB3RSTR;
    volatile uint32_t APB1LRSTR;
    volatile uint32_t APB1HRSTR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t APB4RSTR;
    volatile uint32_t GCR;
    volatile uint32_t RESERVED7;
    volatile uint32_t D3AMR;
    volatile uint32_t RESERVED8[9];
    volatile uint32_t RSR;
    volatile uint32_t AHB3ENR;
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    volatile uint32_t AHB4ENR;
    volatile uint32_t APB3ENR;
    volatile uint32_t APB1LENR;
    volatile uint32_t APB1HENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t APB4ENR;
} RCC_TypeDef;

#define RCC     ((RCC_TypeDef *) RCC_BASE)

/* ============================================================================
 *  GPIO REGISTERS
 * ============================================================================ */
typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
} GPIO_TypeDef;

#define GPIOB   ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC   ((GPIO_TypeDef *) GPIOC_BASE)

/* ============================================================================
 *  EXTI REGISTERS (STM32H7 specific)
 * ============================================================================ */
typedef struct {
    volatile uint32_t RTSR1;        /* 0x00 - Rising trigger selection */
    volatile uint32_t FTSR1;        /* 0x04 - Falling trigger selection */
    volatile uint32_t SWIER1;       /* 0x08 - Software interrupt event */
    volatile uint32_t D3PMR1;       /* 0x0C - D3 pending mask */
    volatile uint32_t D3PCR1L;      /* 0x10 - D3 pending clear low */
    volatile uint32_t D3PCR1H;      /* 0x14 - D3 pending clear high */
    volatile uint32_t RESERVED0[2];
    volatile uint32_t RTSR2;        /* 0x20 */
    volatile uint32_t FTSR2;        /* 0x24 */
    volatile uint32_t SWIER2;       /* 0x28 */
    volatile uint32_t D3PMR2;       /* 0x2C */
    volatile uint32_t D3PCR2L;      /* 0x30 */
    volatile uint32_t D3PCR2H;      /* 0x34 */
    volatile uint32_t RESERVED1[2];
    volatile uint32_t RTSR3;        /* 0x40 */
    volatile uint32_t FTSR3;        /* 0x44 */
    volatile uint32_t SWIER3;       /* 0x48 */
    volatile uint32_t D3PMR3;       /* 0x4C */
    volatile uint32_t D3PCR3L;      /* 0x50 */
    volatile uint32_t D3PCR3H;      /* 0x54 */
    volatile uint32_t RESERVED2[10];
    volatile uint32_t IMR1;         /* 0x80 - CPU1 interrupt mask */
    volatile uint32_t EMR1;         /* 0x84 - CPU1 event mask */
    volatile uint32_t PR1;          /* 0x88 - CPU1 pending register */
    volatile uint32_t RESERVED3;
    volatile uint32_t IMR2;         /* 0x90 */
    volatile uint32_t EMR2;         /* 0x94 */
    volatile uint32_t PR2;          /* 0x98 */
    volatile uint32_t RESERVED4;
    volatile uint32_t IMR3;         /* 0xA0 */
    volatile uint32_t EMR3;         /* 0xA4 */
    volatile uint32_t PR3;          /* 0xA8 */
} EXTI_TypeDef;

#define EXTI    ((EXTI_TypeDef *) EXTI_BASE)

/* ============================================================================
 *  SYSCFG REGISTERS (for EXTI line selection)
 * ============================================================================ */
typedef struct {
    volatile uint32_t RESERVED0;    /* 0x00 */
    volatile uint32_t PMCR;         /* 0x04 */
    volatile uint32_t EXTICR[4];    /* 0x08-0x14 - External interrupt config */
    volatile uint32_t CFGR;         /* 0x18 */
    volatile uint32_t RESERVED1;    /* 0x1C */
    volatile uint32_t CCCSR;        /* 0x20 */
    volatile uint32_t CCVR;         /* 0x24 */
    volatile uint32_t CCCR;         /* 0x28 */
    volatile uint32_t PWRCR;        /* 0x2C */
    volatile uint32_t RESERVED2[61];
    volatile uint32_t PKGR;         /* 0x124 */
    volatile uint32_t RESERVED3[118];
    volatile uint32_t UR0;          /* 0x300 */
    /* ... more UR registers */
} SYSCFG_TypeDef;

#define SYSCFG  ((SYSCFG_TypeDef *) SYSCFG_BASE)

/* ============================================================================
 * 
 *  LESSON 1: IMPORTANT DEFINITIONS
 *  =================================
 * 
 * ============================================================================ */

/* RCC Clock Enable */
#define RCC_AHB4ENR_GPIOBEN     (1U << 1)
#define RCC_AHB4ENR_GPIOCEN     (1U << 2)
#define RCC_APB4ENR_SYSCFGEN    (1U << 1)

/* EXTI Line 13 (for PC13 button) */
#define EXTI_LINE_13            (1U << 13)

/* SYSCFG EXTICR - Port C selection */
#define SYSCFG_EXTICR_PC        2U      /* Port C = 2 */

/* NVIC IRQ number for EXTI15_10 */
#define EXTI15_10_IRQn          40

/* LED on PB0 */
#define LED_PIN                 0

/* Button on PC13 */
#define BUTTON_PIN              13

/* ============================================================================
 * 
 *  ✏️  EXERCISE 1: ENABLE CLOCKS
 *  ==============================
 * 
 * ============================================================================ */

void EXTI_EnableClocks(void) {
    /* ✏️ YOUR TURN: Enable GPIOB clock (for LED) */
    RCC->AHB4ENR |= ???;        /* HINT: Which bit enables GPIOB? */
    
    /* ✏️ YOUR TURN: Enable GPIOC clock (for button) */
    RCC->AHB4ENR |= ???;        /* HINT: Which bit enables GPIOC? */
    
    /* ✏️ YOUR TURN: Enable SYSCFG clock (for EXTI) */
    RCC->APB4ENR |= ???;        /* HINT: SYSCFG is on APB4 bus. Find its enable bit. */
    
    (void)RCC->APB4ENR;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void EXTI_EnableClocks(void) {
 *     RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
 *     RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;
 *     RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
 *     (void)RCC->APB4ENR;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 2: CONFIGURE GPIO
 *  ===============================
 * 
 *  Before EXTI can work, we need to configure the GPIO pins:
 *  - PB0 as OUTPUT (for the LED indicator)
 *  - PC13 as INPUT (for the button that triggers interrupt)
 *  
 *  Input mode = 00 in MODER register (default after reset)
 * 
 * ============================================================================ */

void GPIO_Configure(void) {
    /* Configure PB0 as output (LED) */
    GPIOB->MODER &= ~(3U << (LED_PIN * 2));
    GPIOB->MODER |= (1U << (LED_PIN * 2));   /* Output mode */
    
    /* ✏️ YOUR TURN: Configure PC13 as input (button) */
    /* Clear mode bits (input = 00) */
    GPIOC->MODER &= ???;  /* HINT: Clear 2 bits for BUTTON_PIN using inverted mask */
    
    /* Note: PC13 already has external pull-up on Nucleo board */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * GPIOC->MODER &= ~(3U << (BUTTON_PIN * 2));
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 3: CONFIGURE EXTI LINE SOURCE
 *  ============================================
 * 
 *  EXTI line 13 can come from PA13, PB13, PC13, etc.
 *  We need to select PC13 using SYSCFG.
 *  
 *  EXTICR[0] = Lines 0-3
 *  EXTICR[1] = Lines 4-7
 *  EXTICR[2] = Lines 8-11
 *  EXTICR[3] = Lines 12-15  ← Pin 13 is here!
 *  
 *  Each line uses 4 bits: (line % 4) * 4
 * 
 * ============================================================================ */

void EXTI_ConfigureSource(void) {
    /* Pin 13 is in EXTICR[3] at position (13 % 4) * 4 = 1 * 4 = 4 */
    
    /* Clear the bits for line 13 */
    SYSCFG->EXTICR[3] &= ~(0xFU << ((BUTTON_PIN % 4) * 4));
    
    /* ✏️ YOUR TURN: Set Port C as source for EXTI line 13 */
    SYSCFG->EXTICR[3] |= ???;   /* HINT: Shift Port C code to the 4-bit position for line 13 */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR_PC << ((BUTTON_PIN % 4) * 4));
 * 
 * WHY THIS FORMULA?
 *   - Each EXTICR register holds 4 EXTI lines (4 lines × 4 bits = 16 bits used)
 *   - Pin 13: EXTICR[13/4] = EXTICR[3] (which register)
 *   - Position: (13 % 4) * 4 = 1 * 4 = 4 (which bit position)
 *   - SYSCFG_EXTICR_PC = 2 (Port C code)
 *   - Result: Write value 2 at bits [7:4] of EXTICR[3]
 *   
 *   This tells EXTI: "Line 13 should listen to Port C pin 13 (PC13)"
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 4: CONFIGURE EDGE DETECTION
 *  =========================================
 * 
 *  The button on Nucleo is active LOW (pressed = 0).
 *  So we want to detect FALLING edge (1→0).
 * 
 * ============================================================================ */

void EXTI_ConfigureEdge(void) {
    /* ✏️ YOUR TURN: Enable falling edge trigger for line 13 */
    EXTI->FTSR1 |= ???;         /* HINT: Set the bit for EXTI line 13 */
    
    /* Optional: Disable rising edge (just to be explicit) */
    EXTI->RTSR1 &= ~EXTI_LINE_13;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void EXTI_ConfigureEdge(void) {
 *     EXTI->FTSR1 |= EXTI_LINE_13;
 *     EXTI->RTSR1 &= ~EXTI_LINE_13;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 5: ENABLE EXTI INTERRUPT
 *  ======================================
 * 
 * ============================================================================ */

void EXTI_EnableInterrupt(void) {
    /* ✏️ YOUR TURN: Unmask (enable) interrupt for line 13 */
    EXTI->IMR1 |= ???;          /* HINT: Set the interrupt mask bit for line 13 */
    
    /* Clear any pending interrupt */
    EXTI->PR1 = EXTI_LINE_13;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void EXTI_EnableInterrupt(void) {
 *     EXTI->IMR1 |= EXTI_LINE_13;
 *     EXTI->PR1 = EXTI_LINE_13;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 2: THE INTERRUPT HANDLER
 *  ================================
 * 
 *  When the button is pressed, the CPU jumps to this function.
 *  The function name MUST match the vector table entry!
 *  
 *  EXTI lines 10-15 share one interrupt: EXTI15_10_IRQHandler
 * 
 * ============================================================================ */

/* Global variable to track button presses */
volatile uint32_t button_press_count = 0;

/* ============================================================================
 * 
 *  ✏️  EXERCISE 6: INTERRUPT HANDLER
 *  ==================================
 * 
 *  This is where the magic happens! When the button is pressed:
 *  1. EXTI detects the falling edge
 *  2. NVIC triggers this interrupt handler
 *  3. CPU jumps here automatically
 *  
 *  CRITICAL: Always check which line triggered (multiple lines share this IRQ)
 *  CRITICAL: Always clear the pending bit, or interrupt fires forever!
 * 
 * ============================================================================ */

void EXTI15_10_IRQHandler(void) {
    /* Check if it's line 13 that triggered */
    if (EXTI->PR1 & EXTI_LINE_13) {
        /* ✏️ YOUR TURN: Clear the pending bit (IMPORTANT!) */
        EXTI->PR1 = ???;        /* HINT: Write 1 to the line's bit to clear it (W1C) */
        
        /* Toggle LED */
        GPIOB->ODR ^= (1U << LED_PIN);
        
        /* Increment counter */
        button_press_count++;
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * EXTI->PR1 = EXTI_LINE_13;
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  NOTE: NVIC Configuration
 *  =========================
 * 
 *  To actually receive the interrupt, you also need to enable it in NVIC.
 *  See the NVIC tutorial for details!
 *  
 *  Quick version:
 *  NVIC->ISER[EXTI15_10_IRQn / 32] = (1U << (EXTI15_10_IRQn % 32));
 * 
 * ============================================================================ */

/* NVIC registers */
#define NVIC_ISER_BASE  0xE000E100UL
#define NVIC_ISER       ((volatile uint32_t *) NVIC_ISER_BASE)

void NVIC_EnableIRQ_Simple(uint8_t irq) {
    NVIC_ISER[irq / 32] = (1U << (irq % 32));
}

/* ============================================================================
 * 
 *  ██████╗ ██████╗  █████╗  ██████╗████████╗██╗ ██████╗███████╗
 *  ██╔══██╗██╔══██╗██╔══██╗██╔════╝╚══██╔══╝██║██╔════╝██╔════╝
 *  ██████╔╝██████╔╝███████║██║        ██║   ██║██║     █████╗  
 *  ██╔═══╝ ██╔══██╗██╔══██║██║        ██║   ██║██║     ██╔══╝  
 *  ██║     ██║  ██║██║  ██║╚██████╗   ██║   ██║╚██████╗███████╗
 *  ╚═╝     ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝╚══════╝
 *  
 *  MAIN PROGRAM - Button interrupt toggles LED
 * 
 * ============================================================================ */

int main(void)
{
    /* Initialize */
    EXTI_EnableClocks();
    GPIO_Configure();
    EXTI_ConfigureSource();
    EXTI_ConfigureEdge();
    EXTI_EnableInterrupt();
    
    /* Enable interrupt in NVIC */
    NVIC_EnableIRQ_Simple(EXTI15_10_IRQn);
    
    /* Main loop - CPU is free to do other things! */
    for(;;) {
        /* The LED will toggle automatically when button is pressed */
        /* button_press_count will increment each press */
        
        /* You could do other work here... */
        __asm("nop");
    }
}

/* ============================================================================
 * 
 *  🎉 CONGRATULATIONS!
 *  
 *  You've learned EXTI without HAL:
 *  
 *  ✅ What EXTI is and how it works
 *  ✅ Configuring GPIO as interrupt source
 *  ✅ Selecting edge detection (rising/falling)
 *  ✅ Enabling the interrupt
 *  ✅ Writing an interrupt handler
 *  ✅ Clearing the pending bit
 *  
 *  COMMON USE CASES:
 *  • Button debouncing with timers
 *  • Encoder inputs
 *  • Sensor data ready signals
 *  • Wake from sleep on external event
 * 
 * ============================================================================ */
