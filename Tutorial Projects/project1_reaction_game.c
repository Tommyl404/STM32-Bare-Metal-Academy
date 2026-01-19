/**
 ******************************************************************************
 * @file           : project1_reaction_game.c
 * @brief          : Project Tutorial 1 - Reaction Time Game
 ******************************************************************************
 * 
 *  ██████╗ ███████╗ █████╗  ██████╗████████╗██╗ ██████╗ ███╗   ██╗
 *  ██╔══██╗██╔════╝██╔══██╗██╔════╝╚══██╔══╝██║██╔═══██╗████╗  ██║
 *  ██████╔╝█████╗  ███████║██║        ██║   ██║██║   ██║██╔██╗ ██║
 *  ██╔══██╗██╔══╝  ██╔══██║██║        ██║   ██║██║   ██║██║╚██╗██║
 *  ██║  ██║███████╗██║  ██║╚██████╗   ██║   ██║╚██████╔╝██║ ╚████║
 *  ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝
 *                                                                 
 *   ████████╗██╗███╗   ███╗███████╗     ██████╗  █████╗ ███╗   ███╗███████╗
 *   ╚══██╔══╝██║████╗ ████║██╔════╝    ██╔════╝ ██╔══██╗████╗ ████║██╔════╝
 *      ██║   ██║██╔████╔██║█████╗      ██║  ███╗███████║██╔████╔██║█████╗  
 *      ██║   ██║██║╚██╔╝██║██╔══╝      ██║   ██║██╔══██║██║╚██╔╝██║██╔══╝  
 *      ██║   ██║██║ ╚═╝ ██║███████╗    ╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗
 *      ╚═╝   ╚═╝╚═╝     ╚═╝╚══════╝     ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝
 * 
 *  PROJECT TUTORIAL 1: REACTION TIME GAME
 * 
 *  ════════════════════════════════════════════════════════════════════════
 *  THE GAME:
 *  ════════════════════════════════════════════════════════════════════════
 *  
 *  1. All LEDs are OFF
 *  2. After a RANDOM delay (1-5 seconds), GREEN LED turns ON
 *  3. Player presses the USER button as FAST as possible
 *  4. Reaction time is measured and displayed via LED pattern:
 *     
 *     ┌────────────────┬────────────────────────────────────────────┐
 *     │ Reaction Time  │ Result                                     │
 *     ├────────────────┼────────────────────────────────────────────┤
 *     │ < 200 ms       │ GREEN blinks fast (Excellent!)             │
 *     │ 200-400 ms     │ YELLOW blinks (Good)                       │
 *     │ > 400 ms       │ RED blinks slow (Try again!)               │
 *     │ Too early!     │ RED stays ON (Cheater detected!)           │
 *     └────────────────┴────────────────────────────────────────────┘
 *  
 *  5. Press button again to restart
 *  
 *  
 *  CONCEPTS COMBINED IN THIS PROJECT:
 *  ════════════════════════════════════════════════════════════════════════
 *  
 *  ┌─────────────────┬──────────────────────────────────────────────────┐
 *  │ Concept         │ How it's used                                    │
 *  ├─────────────────┼──────────────────────────────────────────────────┤
 *  │ RCC             │ Enable clocks for GPIO, TIM, EXTI, SYSCFG        │
 *  │ GPIO            │ LEDs (output) and Button (input)                 │
 *  │ TIM             │ Measure reaction time in microseconds            │
 *  │ EXTI            │ Detect button press via interrupt                │
 *  │ NVIC            │ Configure and handle interrupts                  │
 *  │ Bit Manipulation│ All register configurations                      │
 *  └─────────────────┴──────────────────────────────────────────────────┘
 *  
 *  
 *  HARDWARE (Nucleo-H753ZI - NO EXTERNAL COMPONENTS NEEDED):
 *  ════════════════════════════════════════════════════════════════════════
 *  
 *  LEDs:
 *  • PB0  = Green LED (LD1)
 *  • PE1  = Yellow LED (LD2)  
 *  • PB14 = Red LED (LD3)
 *  
 *  Button:
 *  • PC13 = User Button (active LOW with external pull-up)
 * 
 *  DIFFICULTY: ⭐⭐⭐ (Intermediate)
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 *  QUICK REMINDER: PERIPHERAL BASE ADDRESSES
 *  ==========================================
 *  
 *  Every peripheral lives at a specific address in memory.
 *  We need: RCC, GPIO (B, C, E), SYSCFG, EXTI, TIM2, NVIC
 * ============================================================================ */

#define RCC_BASE        0x58024400UL
#define GPIOB_BASE      0x58020400UL
#define GPIOC_BASE      0x58020800UL
#define GPIOE_BASE      0x58021000UL
#define SYSCFG_BASE     0x58000400UL
#define EXTI_BASE       0x58000000UL
#define TIM2_BASE       0x40000000UL

/* NVIC registers are in the Cortex-M7 core, not the STM32 peripheral space */
#define NVIC_ISER_BASE  0xE000E100UL    /* Interrupt Set Enable Registers */
#define NVIC_ICER_BASE  0xE000E180UL    /* Interrupt Clear Enable Registers */
#define NVIC_ISPR_BASE  0xE000E200UL    /* Interrupt Set Pending Registers */
#define NVIC_ICPR_BASE  0xE000E280UL    /* Interrupt Clear Pending Registers */
#define NVIC_IPR_BASE   0xE000E400UL    /* Interrupt Priority Registers */

/* ============================================================================
 *  REGISTER STRUCTURES
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

typedef struct {
    volatile uint32_t RESERVED1;
    volatile uint32_t PMCR;
    volatile uint32_t EXTICR[4];
} SYSCFG_TypeDef;

typedef struct {
    volatile uint32_t RTSR1;    /* 0x00 - Rising trigger selection */
    volatile uint32_t FTSR1;    /* 0x04 - Falling trigger selection */
    volatile uint32_t SWIER1;   /* 0x08 - Software interrupt event */
    volatile uint32_t D3PMR1;   /* 0x0C - D3 pending mask */
    volatile uint32_t D3PCR1L;  /* 0x10 - D3 pending clear low */
    volatile uint32_t D3PCR1H;  /* 0x14 - D3 pending clear high */
    volatile uint32_t RESERVED1[2];
    volatile uint32_t RTSR2;    /* 0x20 */
    volatile uint32_t FTSR2;    /* 0x24 */
    volatile uint32_t SWIER2;   /* 0x28 */
    volatile uint32_t D3PMR2;   /* 0x2C */
    volatile uint32_t D3PCR2L;  /* 0x30 */
    volatile uint32_t D3PCR2H;  /* 0x34 */
    volatile uint32_t RESERVED2[2];
    volatile uint32_t RTSR3;    /* 0x40 */
    volatile uint32_t FTSR3;    /* 0x44 */
    volatile uint32_t SWIER3;   /* 0x48 */
    volatile uint32_t D3PMR3;   /* 0x4C */
    volatile uint32_t D3PCR3L;  /* 0x50 */
    volatile uint32_t D3PCR3H;  /* 0x54 */
    volatile uint32_t RESERVED3[10];
    volatile uint32_t IMR1;     /* 0x80 - CPU interrupt mask */
    volatile uint32_t EMR1;     /* 0x84 - CPU event mask */
    volatile uint32_t PR1;      /* 0x88 - Pending register */
    volatile uint32_t RESERVED4;
    volatile uint32_t IMR2;     /* 0x90 */
    volatile uint32_t EMR2;     /* 0x94 */
    volatile uint32_t PR2;      /* 0x98 */
    volatile uint32_t RESERVED5;
    volatile uint32_t IMR3;     /* 0xA0 */
    volatile uint32_t EMR3;     /* 0xA4 */
    volatile uint32_t PR3;      /* 0xA8 */
} EXTI_TypeDef;

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RCR;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
} TIM_TypeDef;

/* Peripheral pointers */
#define RCC     ((RCC_TypeDef *) RCC_BASE)
#define GPIOB   ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC   ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOE   ((GPIO_TypeDef *) GPIOE_BASE)
#define SYSCFG  ((SYSCFG_TypeDef *) SYSCFG_BASE)
#define EXTI    ((EXTI_TypeDef *) EXTI_BASE)
#define TIM2    ((TIM_TypeDef *) TIM2_BASE)

/* NVIC registers (arrays) */
#define NVIC_ISER   ((volatile uint32_t *) NVIC_ISER_BASE)
#define NVIC_ICER   ((volatile uint32_t *) NVIC_ICER_BASE)
#define NVIC_ICPR   ((volatile uint32_t *) NVIC_ICPR_BASE)

/* ============================================================================
 *  BIT DEFINITIONS
 * ============================================================================ */

/* RCC */
#define RCC_AHB4ENR_GPIOBEN     (1U << 1)
#define RCC_AHB4ENR_GPIOCEN     (1U << 2)
#define RCC_AHB4ENR_GPIOEEN     (1U << 4)
#define RCC_APB4ENR_SYSCFGEN    (1U << 1)
#define RCC_APB1LENR_TIM2EN     (1U << 0)

/* TIM */
#define TIM_CR1_CEN             (1U << 0)
#define TIM_SR_UIF              (1U << 0)
#define TIM_EGR_UG              (1U << 0)

/* EXTI line 13 (for PC13) */
#define EXTI_LINE13             (1U << 13)

/* NVIC - EXTI15_10 is IRQ number 40 */
#define EXTI15_10_IRQn          40

/* LED Pins */
#define LED_GREEN_PIN           0       /* PB0 */
#define LED_YELLOW_PIN          1       /* PE1 */
#define LED_RED_PIN             14      /* PB14 */

/* Button Pin */
#define BUTTON_PIN              13      /* PC13 */

/* ============================================================================
 * 
 *  STEP 1: ENABLE ALL CLOCKS
 *  ==========================
 * 
 *  📚 REMINDER: THE GOLDEN RULE
 *  ─────────────────────────────────────────────────────────────────────────
 *  Before using ANY peripheral, enable its clock!
 *  
 *  For this project we need:
 *  • GPIOB, GPIOC, GPIOE → AHB4ENR (for LEDs and button)
 *  • SYSCFG → APB4ENR (for EXTI line selection)
 *  • TIM2 → APB1LENR (for timing)
 * 
 * ============================================================================ */

void EnableAllClocks(void) {
    /* ✏️ YOUR TURN: Enable GPIOB clock */
    RCC->AHB4ENR |= ???;        /* HINT: RCC_AHB4ENR_GPIOBEN */
    
    /* ✏️ YOUR TURN: Enable GPIOC clock (for button) */
    RCC->AHB4ENR |= ???;        /* HINT: RCC_AHB4ENR_GPIOCEN */
    
    /* ✏️ YOUR TURN: Enable GPIOE clock (for yellow LED) */
    RCC->AHB4ENR |= ???;        /* HINT: RCC_AHB4ENR_GPIOEEN */
    
    /* Enable SYSCFG clock (for EXTI) */
    RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
    
    /* ✏️ YOUR TURN: Enable TIM2 clock */
    RCC->APB1LENR |= ???;       /* HINT: RCC_APB1LENR_TIM2EN */
    
    /* Dummy read for synchronization */
    (void)RCC->AHB4ENR;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void EnableAllClocks(void) {
 *     RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
 *     RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;
 *     RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;
 *     RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
 *     RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;
 *     (void)RCC->AHB4ENR;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  STEP 2: CONFIGURE GPIO - LEDs AND BUTTON
 *  ==========================================
 * 
 *  📚 REMINDER: GPIO MODER
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *  ┌───────────┬─────────────────────────────┐
 *  │ Value     │ Mode                        │
 *  ├───────────┼─────────────────────────────┤
 *  │ 00        │ Input                       │
 *  │ 01        │ Output                      │
 *  │ 10        │ Alternate function          │
 *  │ 11        │ Analog                      │
 *  └───────────┴─────────────────────────────┘
 *  
 *  Position formula: pin × 2
 * 
 * ============================================================================ */

void ConfigureGPIO(void) {
    /* ═══════════════════════════════════════════════════════════════════════
     * GREEN LED - PB0 as output
     * ═══════════════════════════════════════════════════════════════════════ */
    GPIOB->MODER &= ~(3U << (LED_GREEN_PIN * 2));   /* Clear */
    
    /* ✏️ YOUR TURN: Set PB0 to output mode (01) */
    GPIOB->MODER |= ???;        /* HINT: (1U << (LED_GREEN_PIN * 2)) */
    
    /* ═══════════════════════════════════════════════════════════════════════
     * YELLOW LED - PE1 as output
     * ═══════════════════════════════════════════════════════════════════════ */
    GPIOE->MODER &= ~(3U << (LED_YELLOW_PIN * 2));
    GPIOE->MODER |= (1U << (LED_YELLOW_PIN * 2));
    
    /* ═══════════════════════════════════════════════════════════════════════
     * RED LED - PB14 as output
     * ═══════════════════════════════════════════════════════════════════════ */
    GPIOB->MODER &= ~(3U << (LED_RED_PIN * 2));
    
    /* ✏️ YOUR TURN: Set PB14 to output mode */
    GPIOB->MODER |= ???;        /* HINT: (1U << (LED_RED_PIN * 2)) */
    
    /* ═══════════════════════════════════════════════════════════════════════
     * USER BUTTON - PC13 as input (default is 00, but let's be explicit)
     * ═══════════════════════════════════════════════════════════════════════ */
    
    /* ✏️ YOUR TURN: Set PC13 to input mode (clear both bits) */
    GPIOC->MODER &= ???;        /* HINT: ~(3U << (BUTTON_PIN * 2)) */
    
    /* Button has external pull-up on Nucleo, no need for internal pull */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void ConfigureGPIO(void) {
 *     GPIOB->MODER &= ~(3U << (LED_GREEN_PIN * 2));
 *     GPIOB->MODER |= (1U << (LED_GREEN_PIN * 2));
 *     
 *     GPIOE->MODER &= ~(3U << (LED_YELLOW_PIN * 2));
 *     GPIOE->MODER |= (1U << (LED_YELLOW_PIN * 2));
 *     
 *     GPIOB->MODER &= ~(3U << (LED_RED_PIN * 2));
 *     GPIOB->MODER |= (1U << (LED_RED_PIN * 2));
 *     
 *     GPIOC->MODER &= ~(3U << (BUTTON_PIN * 2));
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  STEP 3: CONFIGURE TIMER FOR MICROSECOND COUNTING
 *  ==================================================
 * 
 *  📚 REMINDER: TIMER FORMULA
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *  Timer Frequency = APB Clock / (PSC + 1)
 *  
 *  With 64 MHz clock and PSC = 63:
 *  Timer runs at 64 MHz / 64 = 1 MHz = 1 tick per microsecond!
 *  
 *  TIM2 is 32-bit, so it can count up to 4,294,967,295 µs (~71 minutes)
 *  Perfect for measuring reaction times!
 * 
 * ============================================================================ */

void ConfigureTimer(void) {
    /* ✏️ YOUR TURN: Set prescaler for 1 MHz (1 µs per tick) */
    TIM2->PSC = ???;            /* HINT: 63 (divide 64 MHz by 64) */
    
    /* Set ARR to maximum (free-running counter) */
    TIM2->ARR = 0xFFFFFFFF;     /* TIM2 is 32-bit */
    
    /* Generate update event to load prescaler */
    TIM2->EGR = TIM_EGR_UG;
    
    /* Clear any pending flags */
    TIM2->SR = 0;
    
    /* ✏️ YOUR TURN: Start the timer */
    TIM2->CR1 |= ???;           /* HINT: TIM_CR1_CEN */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void ConfigureTimer(void) {
 *     TIM2->PSC = 63;
 *     TIM2->ARR = 0xFFFFFFFF;
 *     TIM2->EGR = TIM_EGR_UG;
 *     TIM2->SR = 0;
 *     TIM2->CR1 |= TIM_CR1_CEN;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  STEP 4: CONFIGURE EXTI FOR BUTTON INTERRUPT
 *  =============================================
 * 
 *  📚 REMINDER: EXTI CONFIGURATION STEPS
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *  1. Select which GPIO port triggers the EXTI line (SYSCFG->EXTICR)
 *  2. Configure edge detection (rising/falling)
 *  3. Unmask the interrupt line
 *  4. Enable the interrupt in NVIC
 *  
 *  PC13 → EXTI line 13 → EXTICR[3] (lines 12-15) → bits [7:4] for line 13
 *  Port C = 0x02
 *  
 *  Button on Nucleo: Active LOW (pressed = 0)
 *  So we want FALLING edge detection!
 * 
 * ============================================================================ */

void ConfigureEXTI(void) {
    /* Step 1: Select PC13 as EXTI13 source
     * EXTICR[3] handles lines 12-15
     * Line 13 → bits [7:4]
     * Port C = 0x02 */
    SYSCFG->EXTICR[3] &= ~(0xFU << 4);      /* Clear bits for line 13 */
    
    /* ✏️ YOUR TURN: Select port C (0x02) for EXTI13 */
    SYSCFG->EXTICR[3] |= ???;   /* HINT: (0x02U << 4) */
    
    /* Step 2: Configure falling edge trigger (button press) */
    /* ✏️ YOUR TURN: Enable falling edge on line 13 */
    EXTI->FTSR1 |= ???;         /* HINT: EXTI_LINE13 */
    
    /* Step 3: Unmask interrupt for line 13 */
    /* ✏️ YOUR TURN: Enable interrupt mask */
    EXTI->IMR1 |= ???;          /* HINT: EXTI_LINE13 */
    
    /* Step 4: Enable in NVIC
     * EXTI15_10 is IRQ 40
     * ISER[1] handles IRQs 32-63
     * Bit position = 40 - 32 = 8 */
    NVIC_ISER[1] = (1U << (EXTI15_10_IRQn - 32));
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void ConfigureEXTI(void) {
 *     SYSCFG->EXTICR[3] &= ~(0xFU << 4);
 *     SYSCFG->EXTICR[3] |= (0x02U << 4);
 *     EXTI->FTSR1 |= EXTI_LINE13;
 *     EXTI->IMR1 |= EXTI_LINE13;
 *     NVIC_ISER[1] = (1U << (EXTI15_10_IRQn - 32));
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 *  LED CONTROL FUNCTIONS
 * ============================================================================ */

void LED_AllOff(void) {
    GPIOB->BSRR = (1U << (LED_GREEN_PIN + 16));     /* Green OFF */
    GPIOE->BSRR = (1U << (LED_YELLOW_PIN + 16));    /* Yellow OFF */
    GPIOB->BSRR = (1U << (LED_RED_PIN + 16));       /* Red OFF */
}

void LED_GreenOn(void)  { GPIOB->BSRR = (1U << LED_GREEN_PIN); }
void LED_GreenOff(void) { GPIOB->BSRR = (1U << (LED_GREEN_PIN + 16)); }
void LED_YellowOn(void) { GPIOE->BSRR = (1U << LED_YELLOW_PIN); }
void LED_YellowOff(void){ GPIOE->BSRR = (1U << (LED_YELLOW_PIN + 16)); }
void LED_RedOn(void)    { GPIOB->BSRR = (1U << LED_RED_PIN); }
void LED_RedOff(void)   { GPIOB->BSRR = (1U << (LED_RED_PIN + 16)); }

/* ============================================================================
 *  SIMPLE DELAY FUNCTIONS
 * ============================================================================ */

void delay_ms(uint32_t ms) {
    /* Using TIM2 which runs at 1 MHz */
    uint32_t start = TIM2->CNT;
    uint32_t target = ms * 1000;    /* Convert to microseconds */
    while ((TIM2->CNT - start) < target);
}

/* ============================================================================
 *  PSEUDO-RANDOM NUMBER GENERATOR
 *  ================================
 *  
 *  We use a Linear Congruential Generator (LCG) for simplicity.
 *  Seeded by timer value for randomness.
 * ============================================================================ */

static uint32_t random_seed = 12345;

void random_init(void) {
    /* Seed with current timer value */
    random_seed = TIM2->CNT;
    if (random_seed == 0) random_seed = 12345;
}

uint32_t random_range(uint32_t min, uint32_t max) {
    /* LCG: seed = (a * seed + c) mod m */
    random_seed = random_seed * 1103515245 + 12345;
    return min + (random_seed % (max - min + 1));
}

/* ============================================================================
 * 
 *  STEP 5: GAME STATE MACHINE
 *  ============================
 * 
 *  📚 NEW CONCEPT: STATE MACHINES
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *  A state machine keeps track of what the program is doing.
 *  Each state has its own behavior.
 *  
 *        ┌──────────────┐
 *        │  WAITING     │ All LEDs off, waiting to start
 *        └──────┬───────┘
 *               │ (random delay complete)
 *               ▼
 *        ┌──────────────┐
 *        │  READY       │ Green LED ON, waiting for button
 *        └──────┬───────┘
 *               │ (button pressed)
 *               ▼
 *        ┌──────────────┐
 *        │  RESULT      │ Show result LED pattern
 *        └──────┬───────┘
 *               │ (button pressed)
 *               ▼
 *           (back to WAITING)
 *  
 *  Special case: If button pressed during WAITING → CHEATED!
 * 
 * ============================================================================ */

/* Game states */
typedef enum {
    STATE_WAITING,      /* Waiting before green light */
    STATE_READY,        /* Green light ON, waiting for reaction */
    STATE_RESULT,       /* Showing result */
    STATE_CHEATED       /* Pressed too early! */
} GameState;

/* Global game variables (volatile because modified in interrupt) */
volatile GameState game_state = STATE_WAITING;
volatile uint32_t start_time = 0;
volatile uint32_t reaction_time = 0;
volatile uint8_t button_pressed = 0;

/* ============================================================================
 * 
 *  STEP 6: INTERRUPT HANDLER
 *  ===========================
 * 
 *  📚 REMINDER: INTERRUPT HANDLER RULES
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *  1. Keep it SHORT - do minimal work
 *  2. Clear the pending flag or it fires forever!
 *  3. Use volatile for shared variables
 *  4. The handler name must match the vector table
 * 
 * ============================================================================ */

void EXTI15_10_IRQHandler(void) {
    /* Check if it's our line (13) */
    if (EXTI->PR1 & EXTI_LINE13) {
        /* ✏️ YOUR TURN: Clear the pending bit */
        EXTI->PR1 = ???;        /* HINT: EXTI_LINE13 (write 1 to clear!) */
        
        /* Record button press */
        button_pressed = 1;
        
        /* Capture reaction time if in READY state */
        if (game_state == STATE_READY) {
            reaction_time = TIM2->CNT - start_time;
        }
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void EXTI15_10_IRQHandler(void) {
 *     if (EXTI->PR1 & EXTI_LINE13) {
 *         EXTI->PR1 = EXTI_LINE13;
 *         button_pressed = 1;
 *         if (game_state == STATE_READY) {
 *             reaction_time = TIM2->CNT - start_time;
 *         }
 *     }
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 *  RESULT DISPLAY FUNCTIONS
 * ============================================================================ */

void ShowExcellent(void) {
    /* Green blinks fast (5 times) */
    for (int i = 0; i < 5; i++) {
        LED_GreenOn();
        delay_ms(100);
        LED_GreenOff();
        delay_ms(100);
    }
}

void ShowGood(void) {
    /* Yellow blinks (3 times) */
    for (int i = 0; i < 3; i++) {
        LED_YellowOn();
        delay_ms(200);
        LED_YellowOff();
        delay_ms(200);
    }
}

void ShowSlow(void) {
    /* Red blinks slow (2 times) */
    for (int i = 0; i < 2; i++) {
        LED_RedOn();
        delay_ms(400);
        LED_RedOff();
        delay_ms(400);
    }
}

void ShowCheated(void) {
    /* Red stays on for 2 seconds */
    LED_RedOn();
    delay_ms(2000);
    LED_RedOff();
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
 *  MAIN GAME LOOP
 * 
 * ============================================================================ */

int main(void) {
    uint32_t random_delay;
    uint32_t wait_start;
    
    /* Initialize everything */
    EnableAllClocks();
    ConfigureGPIO();
    ConfigureTimer();
    ConfigureEXTI();
    
    /* Seed random number generator */
    random_init();
    
    /* Start with all LEDs off */
    LED_AllOff();
    
    /* Main game loop */
    for (;;) {
        switch (game_state) {
            
            case STATE_WAITING:
                /* ═══════════════════════════════════════════════════════════
                 * WAITING STATE: Random delay before showing green
                 * ═══════════════════════════════════════════════════════════ */
                LED_AllOff();
                button_pressed = 0;
                
                /* Generate random delay: 1000-5000 ms */
                random_delay = random_range(1000, 5000);
                wait_start = TIM2->CNT;
                
                /* Wait for random time, checking for cheating */
                while ((TIM2->CNT - wait_start) < (random_delay * 1000)) {
                    if (button_pressed) {
                        /* Cheater! Pressed before green light */
                        game_state = STATE_CHEATED;
                        break;
                    }
                }
                
                /* If didn't cheat, move to READY */
                if (game_state == STATE_WAITING) {
                    game_state = STATE_READY;
                }
                break;
                
            case STATE_READY:
                /* ═══════════════════════════════════════════════════════════
                 * READY STATE: Green ON, measure reaction time
                 * ═══════════════════════════════════════════════════════════ */
                button_pressed = 0;
                reaction_time = 0;
                
                /* Turn on green LED and record start time */
                LED_GreenOn();
                start_time = TIM2->CNT;
                
                /* Wait for button press (interrupt will capture time) */
                while (!button_pressed);
                
                LED_GreenOff();
                game_state = STATE_RESULT;
                break;
                
            case STATE_RESULT:
                /* ═══════════════════════════════════════════════════════════
                 * RESULT STATE: Show how fast they were
                 * ═══════════════════════════════════════════════════════════ */
                
                /* Convert to milliseconds */
                reaction_time = reaction_time / 1000;
                
                /* Show result based on reaction time */
                if (reaction_time < 200) {
                    ShowExcellent();        /* < 200 ms = Excellent! */
                } else if (reaction_time < 400) {
                    ShowGood();             /* 200-400 ms = Good */
                } else {
                    ShowSlow();             /* > 400 ms = Slow */
                }
                
                /* Wait for button to restart */
                button_pressed = 0;
                delay_ms(500);      /* Debounce delay */
                while (!button_pressed);
                
                game_state = STATE_WAITING;
                break;
                
            case STATE_CHEATED:
                /* ═══════════════════════════════════════════════════════════
                 * CHEATED STATE: Pressed too early!
                 * ═══════════════════════════════════════════════════════════ */
                ShowCheated();
                
                /* Wait for button to restart */
                button_pressed = 0;
                delay_ms(500);      /* Debounce delay */
                while (!button_pressed);
                
                game_state = STATE_WAITING;
                break;
        }
    }
}

/* ============================================================================
 * 
 *  🎮 HOW TO PLAY:
 *  
 *  1. Flash this code to your Nucleo board
 *  2. Wait for a random time (1-5 seconds)
 *  3. When GREEN LED lights up, press the button AS FAST AS YOU CAN!
 *  4. Watch the result:
 *     • Green blinks fast = Excellent (< 200 ms)
 *     • Yellow blinks = Good (200-400 ms)
 *     • Red blinks slow = Too slow (> 400 ms)
 *     • Red stays on = You cheated (pressed too early!)
 *  5. Press button again to play again
 *  
 *  
 *  🎓 WHAT YOU LEARNED:
 *  
 *  ✅ RCC: Enabling clocks for multiple peripherals
 *  ✅ GPIO: Configuring both inputs and outputs
 *  ✅ TIM: Using timer for precise microsecond timing
 *  ✅ EXTI: External interrupt configuration
 *  ✅ NVIC: Enabling interrupts and handler implementation
 *  ✅ State Machines: Managing complex program flow
 *  ✅ Volatile: Proper use with interrupt-modified variables
 *  ✅ Bit Manipulation: All the |=, &=~, << operations
 *  
 *  
 *  🔧 EXPERIMENT IDEAS:
 *  
 *  • Change the reaction time thresholds
 *  • Add a "best time" tracker
 *  • Make it harder by using shorter green light durations
 *  • Add a countdown (3-2-1) before the game starts
 *  • Make it multiplayer (first to press wins)
 * 
 * ============================================================================ */
