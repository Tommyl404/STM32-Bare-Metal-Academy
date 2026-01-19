/**
 ******************************************************************************
 * @file           : tim_tutorial.c
 * @brief          : Learning TIM (Timer) without HAL - Bare Metal
 ******************************************************************************
 * 
 *  ████████╗██╗███╗   ███╗███████╗██████╗     ████████╗██╗   ██╗████████╗ ██████╗ ██████╗ 
 *  ╚══██╔══╝██║████╗ ████║██╔════╝██╔══██╗    ╚══██╔══╝██║   ██║╚══██╔══╝██╔═══██╗██╔══██╗
 *     ██║   ██║██╔████╔██║█████╗  ██████╔╝       ██║   ██║   ██║   ██║   ██║   ██║██████╔╝
 *     ██║   ██║██║╚██╔╝██║██╔══╝  ██╔══██╗       ██║   ██║   ██║   ██║   ██║   ██║██╔══██╗
 *     ██║   ██║██║ ╚═╝ ██║███████╗██║  ██║       ██║   ╚██████╔╝   ██║   ╚██████╔╝██║  ██║
 *     ╚═╝   ╚═╝╚═╝     ╚═╝╚══════╝╚═╝  ╚═╝       ╚═╝    ╚═════╝    ╚═╝    ╚═════╝ ╚═╝  ╚═╝
 * 
 *  INTERACTIVE LEARNING: TIMERS
 * 
 *  WHAT YOU'LL LEARN:
 *  1. What timers are and why they're essential
 *  2. How to configure basic timer counting
 *  3. How to generate delays using timers
 *  4. How to generate PWM for LED dimming/motor control
 * 
 *  HARDWARE:
 *  - TIM2 for basic timing (general purpose, 32-bit)
 *  - TIM3 for PWM output on PB0 (LED1 on Nucleo)
 * 
 *  DIFFICULTY: ⭐⭐⭐ (Medium)
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 * 
 *  LESSON 0: WHAT IS A TIMER?
 *  ===========================
 * 
 *  A timer is a counter that counts up (or down) at a known rate.
 *  
 *          ┌──────────────────┐
 *  Clock ──┤   PRESCALER      ├──► Slower Clock
 *          │   (PSC)          │
 *          └──────────────────┘
 *                   │
 *                   ▼
 *          ┌──────────────────┐
 *          │   COUNTER (CNT)  │──► 0, 1, 2, 3 ... ARR
 *          │                  │
 *          └──────────────────┘
 *                   │
 *                   ▼
 *          When CNT = ARR → EVENT! (then restart from 0)
 *  
 *  KEY FORMULAS:
 *  
 *  Timer Frequency = APB Clock / (PSC + 1)
 *  
 *  Overflow Time = (ARR + 1) × (PSC + 1) / APB Clock
 *  
 *  Example with 64 MHz clock:
 *  PSC = 63, ARR = 999
 *  Timer Freq = 64 MHz / 64 = 1 MHz (1 µs per tick)
 *  Overflow = 1000 × 1 µs = 1 ms
 * 
 * ============================================================================ */

/* ============================================================================
 *  PERIPHERAL BASE ADDRESSES
 * ============================================================================ */
#define RCC_BASE        0x58024400UL
#define GPIOB_BASE      0x58020400UL
#define TIM2_BASE       0x40000000UL
#define TIM3_BASE       0x40000400UL

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

/* ============================================================================
 *  TIMER REGISTERS (General Purpose Timer - TIM2/TIM3)
 * ============================================================================ */
typedef struct {
    volatile uint32_t CR1;      /* 0x00 - Control register 1 */
    volatile uint32_t CR2;      /* 0x04 - Control register 2 */
    volatile uint32_t SMCR;     /* 0x08 - Slave mode control register */
    volatile uint32_t DIER;     /* 0x0C - DMA/Interrupt enable register */
    volatile uint32_t SR;       /* 0x10 - Status register */
    volatile uint32_t EGR;      /* 0x14 - Event generation register */
    volatile uint32_t CCMR1;    /* 0x18 - Capture/compare mode register 1 */
    volatile uint32_t CCMR2;    /* 0x1C - Capture/compare mode register 2 */
    volatile uint32_t CCER;     /* 0x20 - Capture/compare enable register */
    volatile uint32_t CNT;      /* 0x24 - Counter */
    volatile uint32_t PSC;      /* 0x28 - Prescaler */
    volatile uint32_t ARR;      /* 0x2C - Auto-reload register */
    volatile uint32_t RCR;      /* 0x30 - Repetition counter (advanced timers) */
    volatile uint32_t CCR1;     /* 0x34 - Capture/compare register 1 */
    volatile uint32_t CCR2;     /* 0x38 - Capture/compare register 2 */
    volatile uint32_t CCR3;     /* 0x3C - Capture/compare register 3 */
    volatile uint32_t CCR4;     /* 0x40 - Capture/compare register 4 */
    volatile uint32_t BDTR;     /* 0x44 - Break and dead-time register */
    volatile uint32_t DCR;      /* 0x48 - DMA control register */
    volatile uint32_t DMAR;     /* 0x4C - DMA address for full transfer */
} TIM_TypeDef;

#define TIM2    ((TIM_TypeDef *) TIM2_BASE)
#define TIM3    ((TIM_TypeDef *) TIM3_BASE)

/* ============================================================================
 * 
 *  LESSON 1: IMPORTANT BIT DEFINITIONS
 *  ====================================
 * 
 * ============================================================================ */

/* RCC Clock Enable */
#define RCC_AHB4ENR_GPIOBEN     (1U << 1)
#define RCC_APB1LENR_TIM2EN     (1U << 0)
#define RCC_APB1LENR_TIM3EN     (1U << 1)

/* TIM_CR1 Bits */
#define TIM_CR1_CEN             (1U << 0)   /* Counter enable */
#define TIM_CR1_ARPE            (1U << 7)   /* Auto-reload preload enable */

/* TIM_SR Bits */
#define TIM_SR_UIF              (1U << 0)   /* Update interrupt flag */

/* TIM_EGR Bits */
#define TIM_EGR_UG              (1U << 0)   /* Update generation */

/* TIM_CCMR1 Bits (Output Compare Mode) */
#define TIM_CCMR1_OC1M_PWM1     (6U << 4)   /* PWM mode 1 on channel 1 */
#define TIM_CCMR1_OC1PE         (1U << 3)   /* Output compare 1 preload enable */
#define TIM_CCMR1_OC2M_PWM1     (6U << 12)  /* PWM mode 1 on channel 2 */
#define TIM_CCMR1_OC2PE         (1U << 11)  /* Output compare 2 preload enable */

/* TIM_CCMR2 Bits */
#define TIM_CCMR2_OC3M_PWM1     (6U << 4)   /* PWM mode 1 on channel 3 */
#define TIM_CCMR2_OC3PE         (1U << 3)   /* Output compare 3 preload enable */

/* TIM_CCER Bits */
#define TIM_CCER_CC1E           (1U << 0)   /* Capture/Compare 1 output enable */
#define TIM_CCER_CC3E           (1U << 8)   /* Capture/Compare 3 output enable */

/* ============================================================================
 * 
 *  LESSON 2: HOW TO CALCULATE PSC (PRESCALER) VALUES
 *  ===================================================
 * 
 *  Q: How do I know what PSC value to use?
 *  A: Work backwards from what you want!
 * 
 * 
 *  THE FORMULA:
 *  ───────────────────────────────────────────────────────────────────────────
 *  
 *                      APB Clock Frequency
 *  Timer Frequency = ─────────────────────────
 *                         (PSC + 1)
 *  
 *  Rearranged:
 *  
 *                 APB Clock Frequency
 *  PSC + 1  =  ───────────────────────────
 *              Desired Timer Frequency
 *  
 *  Therefore:
 *  
 *             APB Clock Frequency
 *  PSC  =  ─────────────────────────── - 1
 *          Desired Timer Frequency
 * 
 * 
 *  PSC REGISTER SIZE (What values can it be?):
 *  ───────────────────────────────────────────────────────────────────────────
 *  
 *  ┌──────────┬───────────────┬─────────────────┬─────────────────────────┐
 *  │ Timer    │ PSC Size      │ Max PSC Value   │ Min Timer Freq (64 MHz) │
 *  ├──────────┼───────────────┼─────────────────┼─────────────────────────┤
 *  │ TIM2     │ 16-bit        │ 65535           │ 64MHz/65536 = 976 Hz    │
 *  │ TIM3     │ 16-bit        │ 65535           │ 976 Hz                  │
 *  │ TIM4     │ 16-bit        │ 65535           │ 976 Hz                  │
 *  │ TIM5     │ 16-bit        │ 65535           │ 976 Hz                  │
 *  │ Advanced │ 16-bit        │ 65535           │ 976 Hz                  │
 *  └──────────┴───────────────┴─────────────────┴─────────────────────────┘
 *  
 *  PSC can be ANY value from 0 to 65535!
 *  • PSC = 0 → No division (Timer runs at full APB clock)
 *  • PSC = 1 → Divide by 2
 *  • PSC = 65535 → Divide by 65536
 * 
 * 
 *  STEP-BY-STEP CALCULATION EXAMPLES:
 *  ───────────────────────────────────────────────────────────────────────────
 *  
 *  Given: APB Clock = 64 MHz
 *  
 *  
 *  Example 1: I want 1 MHz timer frequency (1 µs per tick)
 *  ──────────────────────────────────────────────────────────
 *  
 *  Step 1: What division do I need?
 *          64 MHz / 1 MHz = 64
 *  
 *  Step 2: Calculate PSC:
 *          PSC + 1 = 64
 *          PSC = 64 - 1 = 63  ✓
 *  
 *  Result: TIM->PSC = 63
 *  
 *  
 *  Example 2: I want 1 kHz timer frequency (1 ms per tick)
 *  ────────────────────────────────────────────────────────
 *  
 *  Step 1: What division do I need?
 *          64 MHz / 1 kHz = 64,000
 *  
 *  Step 2: Calculate PSC:
 *          PSC + 1 = 64,000
 *          PSC = 64,000 - 1 = 63,999  ✓
 *  
 *  Result: TIM->PSC = 63999
 *  
 *  
 *  Example 3: I want 100 Hz timer frequency (10 ms per tick)
 *  ──────────────────────────────────────────────────────────
 *  
 *  Step 1: What division do I need?
 *          64 MHz / 100 Hz = 640,000
 *  
 *  Step 2: Calculate PSC:
 *          PSC + 1 = 640,000
 *          PSC = 640,000 - 1 = 639,999
 *  
 *  PROBLEM! 639,999 > 65,535 (too big for 16-bit PSC!)
 *  
 *  Solution: Use BOTH PSC and ARR together!
 *  
 *  
 *  Example 4: When PSC alone isn't enough (100 Hz with PSC+ARR)
 *  ─────────────────────────────────────────────────────────────
 *  
 *  Step 1: Pick a convenient PSC (e.g., 63 → 1 MHz)
 *          Timer runs at: 64 MHz / 64 = 1 MHz
 *  
 *  Step 2: Calculate ARR for 100 Hz:
 *          1 MHz / 100 Hz = 10,000
 *          ARR = 10,000 - 1 = 9,999  ✓
 *  
 *  Result: TIM->PSC = 63
 *          TIM->ARR = 9999
 *  
 *  Check: 64 MHz / 64 / 10,000 = 100 Hz ✓
 * 
 * 
 *  CHOOSING "NICE" PSC VALUES:
 *  ───────────────────────────────────────────────────────────────────────────
 *  
 *  Common convenient timer frequencies:
 *  
 *  ┌─────────────────┬────────────┬──────────────────────────────────────┐
 *  │ Desired Freq    │ PSC Value  │ Why it's nice                        │
 *  ├─────────────────┼────────────┼──────────────────────────────────────┤
 *  │ 1 MHz (1 µs)    │ 63         │ Easy math for µs delays              │
 *  │ 100 kHz (10 µs) │ 639        │ Good for precise delays              │
 *  │ 10 kHz (100 µs) │ 6399       │ Nice for servo control (20 ms)       │
 *  │ 1 kHz (1 ms)    │ 63999      │ Perfect for millisecond timing       │
 *  └─────────────────┴────────────┴──────────────────────────────────────┘
 *  
 *  Pro tip: Choose PSC to make ARR calculations easy!
 * 
 * 
 *  IMPORTANT NOTES:
 *  ───────────────────────────────────────────────────────────────────────────
 *  
 *  1. PSC and ARR are (N - 1) values!
 *     • PSC = 63 means divide by 64
 *     • ARR = 999 means count 1000 times (0 to 999)
 *  
 *  2. The timer counts: 0, 1, 2, ..., ARR, then back to 0
 *     Total count = ARR + 1
 *  
 *  3. Final period formula:
 *  
 *              (PSC + 1) × (ARR + 1)
 *     Period = ─────────────────────
 *                  APB Clock
 *  
 *  4. Find your APB clock:
 *     • Check RCC configuration
 *     • Default HSI on H7 = 64 MHz
 *     • APB1/APB2 might have dividers!
 * 
 * 
 *  QUICK REFERENCE TABLE (64 MHz Clock):
 *  ───────────────────────────────────────────────────────────────────────────
 *  
 *  ┌──────────────┬──────────┬──────────┬─────────────────────────────────┐
 *  │ Goal         │ PSC      │ ARR      │ Result                          │
 *  ├──────────────┼──────────┼──────────┼─────────────────────────────────┤
 *  │ 1 ms tick    │ 63       │ 999      │ 64MHz/64/1000 = 1 kHz           │
 *  │ 100 µs tick  │ 63       │ 99       │ 64MHz/64/100 = 10 kHz           │
 *  │ 10 µs tick   │ 63       │ 9        │ 64MHz/64/10 = 100 kHz           │
 *  │ 1 µs tick    │ 63       │ 0        │ 64MHz/64/1 = 1 MHz              │
 *  │ 20 ms period │ 639      │ 1999     │ 64MHz/640/2000 = 50 Hz (servo!) │
 *  │ 1 sec period │ 63999    │ 999      │ 64MHz/64000/1000 = 1 Hz         │
 *  └──────────────┴──────────┴──────────┴─────────────────────────────────┘
 * 
 * 
 *  CODE TEMPLATE FOR CALCULATING PSC:
 *  ───────────────────────────────────────────────────────────────────────────
 *  
 *  uint32_t apb_clock = 64000000;              // 64 MHz
 *  uint32_t desired_timer_freq = 1000000;      // 1 MHz
 *  
 *  uint32_t psc = (apb_clock / desired_timer_freq) - 1;
 *  
 *  // Check if it fits in 16 bits
 *  if (psc > 65535) {
 *      // Need to use ARR too!
 *  }
 *  
 *  TIM2->PSC = psc;
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 1: ENABLE CLOCKS
 *  ==============================
 * 
 * ============================================================================ */

void TIM_EnableClocks(void) {
    /* ✏️ YOUR TURN: Enable GPIOB clock (for PWM output) */
    RCC->AHB4ENR |= ???;        /* HINT: Which AHB4 bit enables GPIO port B? */
    
    /* ✏️ YOUR TURN: Enable TIM2 clock */
    RCC->APB1LENR |= ???;       /* HINT: TIM2 is on APB1. Find the timer 2 enable bit. */
    
    /* Enable TIM3 clock (for PWM) */
    RCC->APB1LENR |= RCC_APB1LENR_TIM3EN;
    
    (void)RCC->APB1LENR;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void TIM_EnableClocks(void) {
 *     RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
 *     RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;
 *     RCC->APB1LENR |= RCC_APB1LENR_TIM3EN;
 *     (void)RCC->APB1LENR;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 2: CONFIGURE TIMER FOR 1ms TICK
 *  =============================================
 * 
 *  With 64 MHz clock:
 *  - PSC = 63 → Timer counts at 64MHz/64 = 1 MHz (1 µs per tick)
 *  - ARR = 999 → Counts 0-999 = 1000 ticks = 1 ms
 * 
 * ============================================================================ */

void TIM2_Init(void) {
    /* ✏️ YOUR TURN: Set prescaler (divide by 64) */
    TIM2->PSC = ???;            /* HINT: To divide 64 MHz by 64, what value? Remember: divides by (PSC+1) */
    
    /* ✏️ YOUR TURN: Set auto-reload value (1000 ticks = 1ms) */
    TIM2->ARR = ???;            /* HINT: Counter counts from 0 to ARR, so for 1000 ticks... */
    
    /* Generate update event to load prescaler */
    TIM2->EGR = TIM_EGR_UG;
    
    /* Clear update flag */
    TIM2->SR = 0;
    
    /* ✏️ YOUR TURN: Enable the counter */
    TIM2->CR1 |= ???;           /* HINT: Which CR1 bit starts the counter? */
}
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void TIM2_Init(void) {
 *     TIM2->PSC = 63;
 *     TIM2->ARR = 999;
 *     TIM2->EGR = TIM_EGR_UG;
 *     TIM2->SR = 0;
 *     TIM2->CR1 |= TIM_CR1_CEN;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 3: DELAY USING TIMER
 *  ==================================
 * 
 *  Wait for 'ms' milliseconds by counting update events.
 * 
 * ============================================================================ */

void TIM2_DelayMs(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        /* ✏️ YOUR TURN: Wait for update flag (overflow) */
        while (!(TIM2->SR & ???));  /* HINT: Which SR flag indicates the counter overflowed? */
        
        /* ✏️ YOUR TURN: Clear the flag */
        TIM2->SR &= ~???;           /* HINT: Clear the same flag you just checked */
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void TIM2_DelayMs(uint32_t ms) {
 *     for (uint32_t i = 0; i < ms; i++) {
 *         while (!(TIM2->SR & TIM_SR_UIF));
 *         TIM2->SR &= ~TIM_SR_UIF;
 *     }
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 3: PWM (Pulse Width Modulation)
 *  =======================================
 * 
 *  PWM creates a square wave with adjustable duty cycle.
 * 
 *       ┌────┐    ┌────┐    ┌────┐         25% Duty Cycle
 *  ─────┘    └────┘    └────┘    └─────
 * 
 *       ┌────────┐    ┌────────┐          50% Duty Cycle
 *  ─────┘        └────┘        └─────
 * 
 *       ┌────────────┐    ┌──────────    75% Duty Cycle
 *  ─────┘            └────┘
 * 
 *  Duty Cycle = CCR / ARR × 100%
 * 
 *  TIM3_CH3 is on PB0 (LED1 on Nucleo-H753ZI!)
 *  GPIO Alternate Function 2 (AF2) for TIM3
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 4: CONFIGURE GPIO FOR PWM
 *  =======================================
 * 
 * ============================================================================ */

void PWM_ConfigureGPIO(void) {
    /* ✏️ YOUR TURN: Set PB0 to alternate function mode (MODER = 10) */
    GPIOB->MODER &= ~(3U << (0 * 2));   /* Clear bits */
    GPIOB->MODER |= ???;                /* HINT: AF mode = 10 binary = 2. Shift to pin 0's position. */
    
    /* ✏️ YOUR TURN: Set alternate function to AF2 (4 bit size, value is 0010) (TIM3_CH3) */
    GPIOB->AFR[0] &= ~(0xFU << (0 * 4)); /* Clear AFR bits for PB0 */
    GPIOB->AFR[0] |= ???;               /* HINT: AF2 = value 2. AFR uses 4 bits per pin. */
}
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void PWM_ConfigureGPIO(void) {
 *     GPIOB->MODER &= ~(3U << (0 * 2));
 *     GPIOB->MODER |= (2U << (0 * 2));
 *     
 *     GPIOB->AFR[0] &= ~(0xFU << (0 * 4));
 *     GPIOB->AFR[0] |= (2U << (0 * 4));
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 5: CONFIGURE TIMER FOR PWM
 *  ========================================
 * 
 * ============================================================================ */

void TIM3_PWM_Init(void) {
    /* Set prescaler (64 MHz / 64 = 1 MHz) */
    TIM3->PSC = 63;
    
    /* Set period for 1 kHz PWM (1000 µs) */
    TIM3->ARR = 999;
    
    /* ✏️ YOUR TURN: Configure Channel 3 for PWM mode 1 WITH preload */
    TIM3->CCMR2 = ???;          /* HINT: Combine PWM mode 1 bits with output compare preload enable for CH3 */
    
    /* ✏️ YOUR TURN: Enable Channel 3 output */
    TIM3->CCER |= ???;          /* HINT: Which CCER bit enables capture/compare channel 3? */
    
    /* Initial duty cycle = 0 */
    TIM3->CCR3 = 0;
    
    /* Enable auto-reload preload */
    TIM3->CR1 |= TIM_CR1_ARPE;
    
    /* Generate update event */
    TIM3->EGR = TIM_EGR_UG;
    
    /* Start the timer */
    TIM3->CR1 |= TIM_CR1_CEN;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void TIM3_PWM_Init(void) {
 *     TIM3->PSC = 63;
 *     TIM3->ARR = 999;
 *     TIM3->CCMR2 = TIM_CCMR2_OC3M_PWM1 | TIM_CCMR2_OC3PE;
 *     TIM3->CCER |= TIM_CCER_CC3E;
 *     TIM3->CCR3 = 0;
 *     TIM3->CR1 |= TIM_CR1_ARPE;
 *     TIM3->EGR = TIM_EGR_UG;
 *     TIM3->CR1 |= TIM_CR1_CEN;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 6: SET PWM DUTY CYCLE
 *  ===================================
 * 
 * ============================================================================ */

void PWM_SetDutyCycle(uint8_t percent) {
    if (percent > 100) percent = 100;
    
    /* ✏️ YOUR TURN: Calculate CCR3 value from percentage */
    TIM3->CCR3 = ???;           /* HINT: Duty = CCR/ARR. Solve for CCR given percent. */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void PWM_SetDutyCycle(uint8_t percent) {
 *     if (percent > 100) percent = 100;
 *     TIM3->CCR3 = (TIM3->ARR * percent) / 100;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ██████╗ ██████╗  █████╗  ██████╗████████╗██╗ ██████╗███████╗
 *  ██╔══██╗██╔══██╗██╔══██╗██╔════╝╚══██╔══╝██║██╔════╝██╔════╝
 *  ██████╔╝██████╔╝███████║██║        ██║   ██║██║     █████╗  
 *  ██╔═══╝ ██╔══██╗██╔══██║██║        ██║   ██║██║     ██╔══╝  
 *  ██║     ██║  ██║██║  ██║╚██████╗   ██║   ██║╚██████╗███████╗
 *  ╚═╝     ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝╚══════╝
 *  
 *  MAIN PROGRAM - LED breathing effect with PWM
 * 
 * ============================================================================ */

int main(void)
{
    TIM_EnableClocks();
    
    /* Initialize TIM2 for delay */
    TIM2_Init();
    
    /* Initialize PWM on PB0 */
    PWM_ConfigureGPIO();
    TIM3_PWM_Init();
    
    /* Breathing LED effect */
    for(;;) {
        /* Fade in */
        for (uint8_t i = 0; i <= 100; i++) {
            PWM_SetDutyCycle(i);
            TIM2_DelayMs(10);
        }
        
        /* Fade out */
        for (uint8_t i = 100; i > 0; i--) {
            PWM_SetDutyCycle(i);
            TIM2_DelayMs(10);
        }
    }
}

/* ============================================================================
 * 
 *  🎉 CONGRATULATIONS!
 *  
 *  You've learned TIMERS without HAL:
 *  
 *  ✅ What timers are and how they count
 *  ✅ Prescaler and auto-reload configuration
 *  ✅ Timer-based delays
 *  ✅ PWM output configuration
 *  ✅ GPIO alternate function for timer
 *  ✅ Duty cycle control
 *  
 *  KEY FORMULAS TO REMEMBER:
 *  • Timer Freq = APB Clock / (PSC + 1)
 *  • Period = (ARR + 1) × (PSC + 1) / APB Clock
 *  • Duty % = CCR / ARR × 100%
 *  
 *  NEXT STEPS:
 *  • Timer interrupts for periodic tasks
 *  • Input capture for measuring pulses
 *  • Encoder mode for rotary encoders
 *  • One-pulse mode for precise timing
 * 
 * ============================================================================ */
