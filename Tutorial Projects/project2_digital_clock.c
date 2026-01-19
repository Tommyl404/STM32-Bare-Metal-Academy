/**
 ******************************************************************************
 * @file           : project2_digital_clock.c
 * @brief          : Project Tutorial 2 - Digital Clock with Alarm
 ******************************************************************************
 * 
 *  ██████╗ ██╗ ██████╗ ██╗████████╗ █████╗ ██╗      
 *  ██╔══██╗██║██╔════╝ ██║╚══██╔══╝██╔══██╗██║      
 *  ██║  ██║██║██║  ███╗██║   ██║   ███████║██║      
 *  ██║  ██║██║██║   ██║██║   ██║   ██╔══██║██║      
 *  ██████╔╝██║╚██████╔╝██║   ██║   ██║  ██║███████╗ 
 *  ╚═════╝ ╚═╝ ╚═════╝ ╚═╝   ╚═╝   ╚═╝  ╚═╝╚══════╝ 
 *                                                   
 *   ██████╗██╗      ██████╗  ██████╗██╗  ██╗
 *  ██╔════╝██║     ██╔═══██╗██╔════╝██║ ██╔╝
 *  ██║     ██║     ██║   ██║██║     █████╔╝ 
 *  ██║     ██║     ██║   ██║██║     ██╔═██╗ 
 *  ╚██████╗███████╗╚██████╔╝╚██████╗██║  ██╗
 *   ╚═════╝╚══════╝ ╚═════╝  ╚═════╝╚═╝  ╚═╝
 * 
 *  PROJECT TUTORIAL 2: DIGITAL CLOCK WITH LED DISPLAY
 * 
 *  ════════════════════════════════════════════════════════════════════════
 *  THE PROJECT:
 *  ════════════════════════════════════════════════════════════════════════
 *  
 *  A real-time clock that displays time using LED blink patterns:
 *  
 *  • GREEN LED blinks = Hours (in 12-hour format)
 *  • YELLOW LED blinks = Minutes (in tens, 0-5)
 *  • RED LED blinks = Seconds heartbeat (once per second)
 *  
 *  Example: 3:20 PM
 *  → Green blinks 3 times (3 hours)
 *  → Yellow blinks 2 times (20 minutes = 2 tens)
 *  → Red pulses every second
 *  
 *  FEATURES:
 *  • Button short press: Show current time
 *  • Button long press (2s): Set alarm (current time + 10 seconds)
 *  • When alarm triggers: All LEDs flash rapidly!
 *  
 *  
 *  CONCEPTS COMBINED IN THIS PROJECT:
 *  ════════════════════════════════════════════════════════════════════════
 *  
 *  ┌─────────────────┬──────────────────────────────────────────────────┐
 *  │ Concept         │ How it's used                                    │
 *  ├─────────────────┼──────────────────────────────────────────────────┤
 *  │ RCC             │ Enable clocks for RTC, GPIO, PWR, EXTI           │
 *  │ RTC             │ Keep real time, even during low power            │
 *  │ GPIO            │ LED outputs, button input                        │
 *  │ TIM             │ Debouncing, LED timing                           │
 *  │ PWR             │ Backup domain access for RTC                     │
 *  │ EXTI            │ Button interrupt, RTC alarm interrupt            │
 *  │ NVIC            │ Interrupt priorities and handling                │
 *  └─────────────────┴──────────────────────────────────────────────────┘
 *  
 *  
 *  HARDWARE (NO EXTERNAL COMPONENTS NEEDED):
 *  ════════════════════════════════════════════════════════════════════════
 *  
 *  LEDs:
 *  • PB0  = Green LED (Hours)
 *  • PE1  = Yellow LED (Minutes)  
 *  • PB14 = Red LED (Seconds)
 *  
 *  Button:
 *  • PC13 = User Button
 * 
 *  DIFFICULTY: ⭐⭐⭐⭐ (Intermediate-Advanced)
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 *  PERIPHERAL BASE ADDRESSES
 * ============================================================================ */

#define RCC_BASE        0x58024400UL
#define PWR_BASE        0x58024800UL
#define GPIOB_BASE      0x58020400UL
#define GPIOC_BASE      0x58020800UL
#define GPIOE_BASE      0x58021000UL
#define RTC_BASE        0x58004000UL
#define EXTI_BASE       0x58000000UL
#define SYSCFG_BASE     0x58000400UL
#define TIM2_BASE       0x40000000UL

#define NVIC_ISER_BASE  0xE000E100UL

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
    volatile uint32_t CR1;
    volatile uint32_t CSR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t CPUCR;
    volatile uint32_t RESERVED1;
    volatile uint32_t D3CR;
} PWR_TypeDef;

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
    volatile uint32_t TR;
    volatile uint32_t DR;
    volatile uint32_t SSR;
    volatile uint32_t ICSR;
    volatile uint32_t PRER;
    volatile uint32_t WUTR;
    volatile uint32_t CR;
    volatile uint32_t RESERVED1[2];
    volatile uint32_t WPR;
    volatile uint32_t CALR;
    volatile uint32_t SHIFTR;
    volatile uint32_t TSTR;
    volatile uint32_t TSDR;
    volatile uint32_t TSSSR;
    volatile uint32_t RESERVED2;
    volatile uint32_t ALRMAR;
    volatile uint32_t ALRMASSR;
    volatile uint32_t ALRMBR;
    volatile uint32_t ALRMBSSR;
    volatile uint32_t SR;
    volatile uint32_t MISR;
    volatile uint32_t RESERVED3;
    volatile uint32_t SCR;
} RTC_TypeDef;

typedef struct {
    volatile uint32_t RTSR1;
    volatile uint32_t FTSR1;
    volatile uint32_t SWIER1;
    volatile uint32_t D3PMR1;
    volatile uint32_t D3PCR1L;
    volatile uint32_t D3PCR1H;
    volatile uint32_t RESERVED1[2];
    volatile uint32_t RTSR2;
    volatile uint32_t FTSR2;
    volatile uint32_t SWIER2;
    volatile uint32_t D3PMR2;
    volatile uint32_t D3PCR2L;
    volatile uint32_t D3PCR2H;
    volatile uint32_t RESERVED2[2];
    volatile uint32_t RTSR3;
    volatile uint32_t FTSR3;
    volatile uint32_t SWIER3;
    volatile uint32_t D3PMR3;
    volatile uint32_t D3PCR3L;
    volatile uint32_t D3PCR3H;
    volatile uint32_t RESERVED3[10];
    volatile uint32_t IMR1;
    volatile uint32_t EMR1;
    volatile uint32_t PR1;
    volatile uint32_t RESERVED4;
    volatile uint32_t IMR2;
    volatile uint32_t EMR2;
    volatile uint32_t PR2;
} EXTI_TypeDef;

typedef struct {
    volatile uint32_t RESERVED1;
    volatile uint32_t PMCR;
    volatile uint32_t EXTICR[4];
} SYSCFG_TypeDef;

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
} TIM_TypeDef;

/* Peripheral Pointers */
#define RCC     ((RCC_TypeDef *) RCC_BASE)
#define PWR     ((PWR_TypeDef *) PWR_BASE)
#define GPIOB   ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC   ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOE   ((GPIO_TypeDef *) GPIOE_BASE)
#define RTC     ((RTC_TypeDef *) RTC_BASE)
#define EXTI    ((EXTI_TypeDef *) EXTI_BASE)
#define SYSCFG  ((SYSCFG_TypeDef *) SYSCFG_BASE)
#define TIM2    ((TIM_TypeDef *) TIM2_BASE)

#define NVIC_ISER   ((volatile uint32_t *) NVIC_ISER_BASE)

/* ============================================================================
 *  BIT DEFINITIONS
 * ============================================================================ */

/* RCC */
#define RCC_AHB4ENR_GPIOBEN     (1U << 1)
#define RCC_AHB4ENR_GPIOCEN     (1U << 2)
#define RCC_AHB4ENR_GPIOEEN     (1U << 4)
#define RCC_APB4ENR_SYSCFGEN    (1U << 1)
#define RCC_APB4ENR_RTCAPBEN    (1U << 16)
#define RCC_APB1LENR_TIM2EN     (1U << 0)

#define RCC_BDCR_LSION          (1U << 0)   /* We'll use LSI for simplicity */
#define RCC_BDCR_LSIRDY         (1U << 1)
#define RCC_BDCR_RTCSEL_LSI     (2U << 8)
#define RCC_BDCR_RTCEN          (1U << 15)

/* PWR */
#define PWR_CR1_DBP             (1U << 8)

/* RTC */
#define RTC_WPR_KEY1            0xCA
#define RTC_WPR_KEY2            0x53
#define RTC_ICSR_INIT           (1U << 7)
#define RTC_ICSR_INITF          (1U << 6)
#define RTC_ICSR_RSF            (1U << 5)
#define RTC_CR_ALRAE            (1U << 8)   /* Alarm A enable */
#define RTC_CR_ALRAIE           (1U << 12)  /* Alarm A interrupt enable */
#define RTC_SR_ALRAF            (1U << 0)   /* Alarm A flag */
#define RTC_SCR_CALRAF          (1U << 0)   /* Clear alarm A flag */
#define RTC_ALRMAR_MSK4         (1U << 31)  /* Mask day */
#define RTC_ALRMAR_MSK3         (1U << 23)  /* Mask hours */
#define RTC_ALRMAR_MSK2         (1U << 15)  /* Mask minutes */
#define RTC_ALRMAR_MSK1         (1U << 7)   /* Mask seconds */

/* TIM */
#define TIM_CR1_CEN             (1U << 0)
#define TIM_EGR_UG              (1U << 0)

/* EXTI */
#define EXTI_LINE13             (1U << 13)  /* Button */
#define EXTI_LINE17             (1U << 17)  /* RTC Alarm */

/* IRQ Numbers */
#define EXTI15_10_IRQn          40
#define RTC_Alarm_IRQn          41

/* Pins */
#define LED_GREEN_PIN           0
#define LED_YELLOW_PIN          1
#define LED_RED_PIN             14
#define BUTTON_PIN              13

/* ============================================================================
 * 
 *  📚 QUICK LESSON: BCD (Binary Coded Decimal)
 *  ============================================
 *  
 *  The RTC stores time in BCD format, not regular binary!
 *  
 *  Regular binary: 23 = 0x17 = 0001 0111
 *  BCD:            23 = 0x23 = 0010 0011
 *                              ─┬── ─┬──
 *                               │    └── units (3)
 *                               └─────── tens (2)
 *  
 *  Each decimal digit is stored in 4 bits.
 *  Makes it easier to display on 7-segment displays!
 * 
 * ============================================================================ */

uint8_t DecToBcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

uint8_t BcdToDec(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}

/* ============================================================================
 * 
 *  STEP 1: ENABLE ALL CLOCKS
 *  ==========================
 * 
 * ============================================================================ */

void EnableClocks(void) {
    /* Enable GPIO clocks */
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;
    
    /* Enable SYSCFG for EXTI */
    RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
    
    /* ✏️ YOUR TURN: Enable RTC APB clock */
    RCC->APB4ENR |= ???;        /* HINT: RCC_APB4ENR_RTCAPBEN */
    
    /* Enable TIM2 for delays */
    RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;
    
    (void)RCC->APB4ENR;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * RCC->APB4ENR |= RCC_APB4ENR_RTCAPBEN;
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  STEP 2: CONFIGURE GPIO
 *  ========================
 * 
 * ============================================================================ */

void ConfigureGPIO(void) {
    /* Configure LEDs as outputs */
    GPIOB->MODER &= ~(3U << (LED_GREEN_PIN * 2));
    GPIOB->MODER |= (1U << (LED_GREEN_PIN * 2));
    
    GPIOE->MODER &= ~(3U << (LED_YELLOW_PIN * 2));
    GPIOE->MODER |= (1U << (LED_YELLOW_PIN * 2));
    
    GPIOB->MODER &= ~(3U << (LED_RED_PIN * 2));
    GPIOB->MODER |= (1U << (LED_RED_PIN * 2));
    
    /* Configure button as input */
    GPIOC->MODER &= ~(3U << (BUTTON_PIN * 2));
}

/* ============================================================================
 * 
 *  STEP 3: CONFIGURE RTC
 *  =======================
 * 
 *  📚 REMINDER: RTC HAS 3 PROTECTION LAYERS
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *  Layer 1: PWR->CR1 |= PWR_CR1_DBP (Backup domain access)
 *  Layer 2: RTC->WPR = 0xCA, 0x53 (Write protection keys)
 *  Layer 3: RTC->ICSR |= INIT (Initialization mode)
 *  
 *  All three must be unlocked before changing time!
 * 
 * ============================================================================ */

void ConfigureRTC(void) {
    /* ═══════════════════════════════════════════════════════════════════════
     * LAYER 1: Enable backup domain access
     * ═══════════════════════════════════════════════════════════════════════ */
    
    /* ✏️ YOUR TURN: Enable backup domain access */
    PWR->CR1 |= ???;            /* HINT: PWR_CR1_DBP */
    while (!(PWR->CR1 & PWR_CR1_DBP));
    
    /* ═══════════════════════════════════════════════════════════════════════
     * Enable LSI and select as RTC clock (simpler than LSE)
     * ═══════════════════════════════════════════════════════════════════════ */
    RCC->CSR |= (1U << 0);      /* LSION */
    while (!(RCC->CSR & (1U << 1)));  /* Wait for LSIRDY */
    
    /* Select LSI as RTC clock source */
    RCC->BDCR &= ~(3U << 8);    /* Clear RTCSEL */
    RCC->BDCR |= RCC_BDCR_RTCSEL_LSI;
    
    /* Enable RTC */
    RCC->BDCR |= RCC_BDCR_RTCEN;
    
    /* ═══════════════════════════════════════════════════════════════════════
     * LAYER 2: Disable write protection
     * ═══════════════════════════════════════════════════════════════════════ */
    
    /* ✏️ YOUR TURN: Write the magic keys */
    RTC->WPR = ???;             /* HINT: RTC_WPR_KEY1 (0xCA) */
    RTC->WPR = ???;             /* HINT: RTC_WPR_KEY2 (0x53) */
    
    /* ═══════════════════════════════════════════════════════════════════════
     * LAYER 3: Enter initialization mode
     * ═══════════════════════════════════════════════════════════════════════ */
    
    /* ✏️ YOUR TURN: Enter init mode */
    RTC->ICSR |= ???;           /* HINT: RTC_ICSR_INIT */
    while (!(RTC->ICSR & RTC_ICSR_INITF));
    
    /* Set prescaler for ~1 Hz from 32 kHz LSI
     * Async = 127 (divide by 128)
     * Sync = 249 (divide by 250)
     * 32000 / 128 / 250 ≈ 1 Hz */
    RTC->PRER = (127U << 16) | 249U;
    
    /* Set initial time: 12:00:00 */
    RTC->TR = (DecToBcd(12) << 16) |    /* Hours */
              (DecToBcd(0) << 8) |       /* Minutes */
              (DecToBcd(0));             /* Seconds */
    
    /* Set date: 2025-01-01, Wednesday */
    RTC->DR = (DecToBcd(25) << 16) |    /* Year */
              (3U << 13) |               /* Weekday (Wed) */
              (DecToBcd(1) << 8) |       /* Month */
              (DecToBcd(1));             /* Day */
    
    /* Exit initialization mode */
    RTC->ICSR &= ~RTC_ICSR_INIT;
    
    /* Enable write protection */
    RTC->WPR = 0xFF;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void ConfigureRTC(void) {
 *     PWR->CR1 |= PWR_CR1_DBP;
 *     while (!(PWR->CR1 & PWR_CR1_DBP));
 *     
 *     RCC->CSR |= (1U << 0);
 *     while (!(RCC->CSR & (1U << 1)));
 *     RCC->BDCR &= ~(3U << 8);
 *     RCC->BDCR |= RCC_BDCR_RTCSEL_LSI;
 *     RCC->BDCR |= RCC_BDCR_RTCEN;
 *     
 *     RTC->WPR = RTC_WPR_KEY1;
 *     RTC->WPR = RTC_WPR_KEY2;
 *     
 *     RTC->ICSR |= RTC_ICSR_INIT;
 *     while (!(RTC->ICSR & RTC_ICSR_INITF));
 *     
 *     RTC->PRER = (127U << 16) | 249U;
 *     RTC->TR = (DecToBcd(12) << 16) | (DecToBcd(0) << 8) | DecToBcd(0);
 *     RTC->DR = (DecToBcd(25) << 16) | (3U << 13) | (DecToBcd(1) << 8) | DecToBcd(1);
 *     
 *     RTC->ICSR &= ~RTC_ICSR_INIT;
 *     RTC->WPR = 0xFF;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  STEP 4: CONFIGURE TIMER FOR DELAYS
 *  ====================================
 * 
 * ============================================================================ */

void ConfigureTimer(void) {
    TIM2->PSC = 63;             /* 64 MHz / 64 = 1 MHz */
    TIM2->ARR = 0xFFFFFFFF;
    TIM2->EGR = TIM_EGR_UG;
    TIM2->CR1 |= TIM_CR1_CEN;
}

void delay_ms(uint32_t ms) {
    uint32_t start = TIM2->CNT;
    while ((TIM2->CNT - start) < (ms * 1000));
}

/* ============================================================================
 * 
 *  STEP 5: CONFIGURE BUTTON INTERRUPT
 *  ====================================
 * 
 * ============================================================================ */

void ConfigureButtonEXTI(void) {
    /* Select PC13 for EXTI13 */
    SYSCFG->EXTICR[3] &= ~(0xFU << 4);
    SYSCFG->EXTICR[3] |= (0x02U << 4);      /* Port C */
    
    /* Falling edge (button press) */
    EXTI->FTSR1 |= EXTI_LINE13;
    
    /* Unmask */
    EXTI->IMR1 |= EXTI_LINE13;
    
    /* Enable in NVIC */
    NVIC_ISER[1] = (1U << (EXTI15_10_IRQn - 32));
}

/* ============================================================================
 *  LED CONTROL
 * ============================================================================ */

void LED_AllOff(void) {
    GPIOB->BSRR = (1U << (LED_GREEN_PIN + 16));
    GPIOE->BSRR = (1U << (LED_YELLOW_PIN + 16));
    GPIOB->BSRR = (1U << (LED_RED_PIN + 16));
}

void LED_AllOn(void) {
    GPIOB->BSRR = (1U << LED_GREEN_PIN);
    GPIOE->BSRR = (1U << LED_YELLOW_PIN);
    GPIOB->BSRR = (1U << LED_RED_PIN);
}

void LED_GreenBlink(uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        GPIOB->BSRR = (1U << LED_GREEN_PIN);
        delay_ms(200);
        GPIOB->BSRR = (1U << (LED_GREEN_PIN + 16));
        delay_ms(200);
    }
}

void LED_YellowBlink(uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        GPIOE->BSRR = (1U << LED_YELLOW_PIN);
        delay_ms(200);
        GPIOE->BSRR = (1U << (LED_YELLOW_PIN + 16));
        delay_ms(200);
    }
}

void LED_RedPulse(void) {
    GPIOB->BSRR = (1U << LED_RED_PIN);
    delay_ms(50);
    GPIOB->BSRR = (1U << (LED_RED_PIN + 16));
}

/* ============================================================================
 * 
 *  STEP 6: READ CURRENT TIME
 *  ===========================
 * 
 *  📚 REMINDER: SHADOW REGISTERS
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *  RTC runs on a slow clock (32 kHz). Reading TR directly might give
 *  inconsistent values. Clear RSF and wait for it to be set again
 *  to ensure we read consistent shadow registers.
 * 
 * ============================================================================ */

typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} Time_t;

void GetTime(Time_t *time) {
    uint32_t tr;
    
    /* Wait for shadow registers to sync */
    RTC->ICSR &= ~RTC_ICSR_RSF;
    while (!(RTC->ICSR & RTC_ICSR_RSF));
    
    /* ✏️ YOUR TURN: Read the time register */
    tr = ???;                   /* HINT: RTC->TR */
    
    /* Extract BCD values and convert */
    time->hours = BcdToDec((tr >> 16) & 0x3F);
    time->minutes = BcdToDec((tr >> 8) & 0x7F);
    time->seconds = BcdToDec(tr & 0x7F);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * tr = RTC->TR;
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  STEP 7: SET ALARM
 *  ===================
 * 
 *  📚 NEW CONCEPT: RTC ALARM MASKS
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *  Each field (seconds, minutes, hours, day) can be masked.
 *  If masked, that field is ignored when comparing.
 *  
 *  Example: To trigger every minute at second 30:
 *  - Set seconds = 30
 *  - Mask minutes, hours, day (MSK2, MSK3, MSK4)
 *  
 *  For our alarm, we want to match exact seconds, so we mask everything else.
 * 
 * ============================================================================ */

void SetAlarm(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    /* Disable write protection */
    RTC->WPR = RTC_WPR_KEY1;
    RTC->WPR = RTC_WPR_KEY2;
    
    /* ✏️ YOUR TURN: Disable Alarm A before configuring */
    RTC->CR &= ???;             /* HINT: ~RTC_CR_ALRAE */
    
    /* Wait for alarm write access (can take up to 2 RTCCLK cycles) */
    delay_ms(1);
    
    /* Set alarm time
     * We mask day and match hours, minutes, seconds */
    RTC->ALRMAR = RTC_ALRMAR_MSK4 |                     /* Don't care about day */
                  (DecToBcd(hours) << 16) |
                  (DecToBcd(minutes) << 8) |
                  DecToBcd(seconds);
    
    /* Enable Alarm A and its interrupt */
    RTC->CR |= RTC_CR_ALRAE | RTC_CR_ALRAIE;
    
    /* Enable write protection */
    RTC->WPR = 0xFF;
    
    /* Configure EXTI line 17 for RTC Alarm */
    EXTI->RTSR1 |= EXTI_LINE17;     /* Rising edge */
    EXTI->IMR1 |= EXTI_LINE17;      /* Unmask */
    
    /* Enable in NVIC */
    NVIC_ISER[1] = (1U << (RTC_Alarm_IRQn - 32));
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * RTC->CR &= ~RTC_CR_ALRAE;
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 *  DISPLAY TIME ON LEDs
 * ============================================================================ */

void DisplayTime(Time_t *time) {
    uint8_t hours_12;
    uint8_t minutes_tens;
    
    /* Convert to 12-hour format */
    hours_12 = time->hours % 12;
    if (hours_12 == 0) hours_12 = 12;
    
    /* Get tens of minutes */
    minutes_tens = time->minutes / 10;
    
    /* Display hours (Green LED) */
    LED_GreenBlink(hours_12);
    
    delay_ms(500);      /* Pause between */
    
    /* Display minutes tens (Yellow LED) */
    if (minutes_tens > 0) {
        LED_YellowBlink(minutes_tens);
    } else {
        /* Blink once quickly to show 0 */
        GPIOE->BSRR = (1U << LED_YELLOW_PIN);
        delay_ms(50);
        GPIOE->BSRR = (1U << (LED_YELLOW_PIN + 16));
    }
}

/* ============================================================================
 *  GLOBAL VARIABLES
 * ============================================================================ */

volatile uint8_t button_pressed = 0;
volatile uint32_t button_press_time = 0;
volatile uint8_t alarm_triggered = 0;

/* ============================================================================
 *  INTERRUPT HANDLERS
 * ============================================================================ */

void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR1 & EXTI_LINE13) {
        EXTI->PR1 = EXTI_LINE13;        /* Clear pending */
        button_pressed = 1;
        button_press_time = TIM2->CNT;
    }
}

void RTC_Alarm_IRQHandler(void) {
    if (EXTI->PR1 & EXTI_LINE17) {
        EXTI->PR1 = EXTI_LINE17;        /* Clear EXTI pending */
    }
    
    if (RTC->SR & RTC_SR_ALRAF) {
        /* Clear alarm flag */
        RTC->SCR = RTC_SCR_CALRAF;
        alarm_triggered = 1;
    }
}

/* ============================================================================
 *  ALARM ANIMATION
 * ============================================================================ */

void AlarmAnimation(void) {
    /* Flash all LEDs rapidly 10 times */
    for (int i = 0; i < 10; i++) {
        LED_AllOn();
        delay_ms(100);
        LED_AllOff();
        delay_ms(100);
    }
    
    /* Disable alarm */
    RTC->WPR = RTC_WPR_KEY1;
    RTC->WPR = RTC_WPR_KEY2;
    RTC->CR &= ~(RTC_CR_ALRAE | RTC_CR_ALRAIE);
    RTC->WPR = 0xFF;
    
    alarm_triggered = 0;
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
 *  MAIN PROGRAM
 * 
 * ============================================================================ */

int main(void) {
    Time_t current_time;
    uint32_t last_second = 0;
    uint32_t button_hold_time;
    
    /* Initialize */
    EnableClocks();
    ConfigureGPIO();
    ConfigureTimer();
    ConfigureRTC();
    ConfigureButtonEXTI();
    
    LED_AllOff();
    
    for (;;) {
        /* Get current time */
        GetTime(&current_time);
        
        /* ═══════════════════════════════════════════════════════════════════
         * HEARTBEAT: Red LED pulses every second
         * ═══════════════════════════════════════════════════════════════════ */
        if (current_time.seconds != last_second) {
            last_second = current_time.seconds;
            LED_RedPulse();
        }
        
        /* ═══════════════════════════════════════════════════════════════════
         * ALARM CHECK
         * ═══════════════════════════════════════════════════════════════════ */
        if (alarm_triggered) {
            AlarmAnimation();
        }
        
        /* ═══════════════════════════════════════════════════════════════════
         * BUTTON HANDLING
         * Short press (< 2s) = Show time
         * Long press (>= 2s) = Set alarm for 10 seconds from now
         * ═══════════════════════════════════════════════════════════════════ */
        if (button_pressed) {
            button_pressed = 0;
            
            /* Wait for button release and measure hold time */
            while (!(GPIOC->IDR & (1U << BUTTON_PIN)));     /* Wait for release */
            button_hold_time = (TIM2->CNT - button_press_time) / 1000;  /* ms */
            
            delay_ms(50);       /* Debounce */
            
            if (button_hold_time >= 2000) {
                /* Long press: Set alarm for 10 seconds from now */
                GetTime(&current_time);
                
                uint8_t alarm_sec = (current_time.seconds + 10) % 60;
                uint8_t alarm_min = current_time.minutes + ((current_time.seconds + 10) / 60);
                uint8_t alarm_hr = current_time.hours + (alarm_min / 60);
                alarm_min %= 60;
                alarm_hr %= 24;
                
                SetAlarm(alarm_hr, alarm_min, alarm_sec);
                
                /* Confirm with 3 quick flashes */
                for (int i = 0; i < 3; i++) {
                    LED_AllOn();
                    delay_ms(100);
                    LED_AllOff();
                    delay_ms(100);
                }
            } else {
                /* Short press: Display current time */
                GetTime(&current_time);
                DisplayTime(&current_time);
            }
        }
    }
}

/* ============================================================================
 * 
 *  ⏰ HOW TO USE:
 *  
 *  1. Flash this code to your Nucleo board
 *  2. Watch the RED LED pulse every second (heartbeat)
 *  3. Short press button: Time is displayed
 *     • Green blinks = Hours (1-12)
 *     • Yellow blinks = Tens of minutes (0-5)
 *  4. Long press button (2+ seconds): Alarm set for 10 seconds from now
 *     • All LEDs flash 3 times to confirm
 *     • After 10 seconds, all LEDs flash rapidly!
 *  
 *  
 *  🎓 WHAT YOU LEARNED:
 *  
 *  ✅ RCC: Multiple clock domains (RTC uses LSI, CPU uses HSI)
 *  ✅ PWR: Backup domain access
 *  ✅ RTC: Write protection, initialization, time format (BCD)
 *  ✅ RTC Alarms: Configuration and interrupt handling
 *  ✅ GPIO: Both input and output configuration
 *  ✅ EXTI: Multiple interrupt sources (button + RTC alarm)
 *  ✅ NVIC: Multiple interrupt handlers
 *  ✅ TIM: Using timer for delays and measuring press duration
 *  ✅ State Management: Using volatile with interrupts
 *  
 *  
 *  🔧 EXPERIMENT IDEAS:
 *  
 *  • Add minutes display (single digits as well as tens)
 *  • Use PWM for LED breathing effect on heartbeat
 *  • Add snooze functionality (button during alarm delays it)
 *  • Make time adjustable with button sequences
 *  • Display AM/PM differently (e.g., blink pattern)
 * 
 * ============================================================================ */
