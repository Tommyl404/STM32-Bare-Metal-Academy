/**
 ******************************************************************************
 * @file           : project3_led_metronome.c
 * @brief          : Project Tutorial 3 - LED Metronome with Tempo Memory
 ******************************************************************************
 * 
 *  ███╗   ███╗███████╗████████╗██████╗  ██████╗ ███╗   ██╗ ██████╗ ███╗   ███╗███████╗
 *  ████╗ ████║██╔════╝╚══██╔══╝██╔══██╗██╔═══██╗████╗  ██║██╔═══██╗████╗ ████║██╔════╝
 *  ██╔████╔██║█████╗     ██║   ██████╔╝██║   ██║██╔██╗ ██║██║   ██║██╔████╔██║█████╗  
 *  ██║╚██╔╝██║██╔══╝     ██║   ██╔══██╗██║   ██║██║╚██╗██║██║   ██║██║╚██╔╝██║██╔══╝  
 *  ██║ ╚═╝ ██║███████╗   ██║   ██║  ██║╚██████╔╝██║ ╚████║╚██████╔╝██║ ╚═╝ ██║███████╗
 *  ╚═╝     ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝
 *
 *  PROJECT TUTORIAL 3: LED METRONOME WITH FLASH MEMORY
 * 
 *  ════════════════════════════════════════════════════════════════════════
 *  THE PROJECT:
 *  ════════════════════════════════════════════════════════════════════════
 *  
 *  A visual metronome that:
 *  • Blinks LEDs at a steady beat (tempo)
 *  • Button cycles through 4 preset tempos
 *  • Current tempo is SAVED to flash memory
 *  • After power cycle, it resumes at the last saved tempo!
 *  
 *  TEMPO PRESETS:
 *  ┌──────────┬──────────┬─────────────────┐
 *  │ Preset   │ BPM      │ LED Indicator   │
 *  ├──────────┼──────────┼─────────────────┤
 *  │ Andante  │ 60 BPM   │ Green only      │
 *  │ Moderato │ 90 BPM   │ Yellow only     │
 *  │ Allegro  │ 120 BPM  │ Red only        │
 *  │ Presto   │ 180 BPM  │ All 3 LEDs      │
 *  └──────────┴──────────┴─────────────────┘
 *  
 *  
 *  CONCEPTS COMBINED IN THIS PROJECT:
 *  ════════════════════════════════════════════════════════════════════════
 *  
 *  ┌─────────────────┬──────────────────────────────────────────────────┐
 *  │ Concept         │ How it's used                                    │
 *  ├─────────────────┼──────────────────────────────────────────────────┤
 *  │ RCC             │ Enable clocks for peripherals                    │
 *  │ GPIO            │ LED outputs, button input                        │
 *  │ TIM             │ Generate precise beat intervals                  │
 *  │ EXTI            │ Button interrupt to change tempo                 │
 *  │ NVIC            │ Timer and button interrupts                      │
 *  │ FLASH           │ Save/load tempo setting to persist power cycles │
 *  │ State Machine   │ Manage metronome state                           │
 *  └─────────────────┴──────────────────────────────────────────────────┘
 *  
 *  
 *  HARDWARE (NO EXTERNAL COMPONENTS NEEDED):
 *  ════════════════════════════════════════════════════════════════════════
 *  
 *  LEDs:
 *  • PB0  = Green LED 
 *  • PE1  = Yellow LED
 *  • PB14 = Red LED
 *  
 *  Button:
 *  • PC13 = User Button
 * 
 *  DIFFICULTY: ⭐⭐⭐⭐⭐ (Advanced - involves Flash programming)
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 *  PERIPHERAL BASE ADDRESSES
 * ============================================================================ */

#define RCC_BASE        0x58024400UL
#define GPIOB_BASE      0x58020400UL
#define GPIOC_BASE      0x58020800UL
#define GPIOE_BASE      0x58021000UL
#define FLASH_BASE      0x52002000UL
#define TIM2_BASE       0x40000000UL
#define TIM3_BASE       0x40000400UL
#define EXTI_BASE       0x58000000UL
#define SYSCFG_BASE     0x58000400UL

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
    volatile uint32_t ACR;
    volatile uint32_t KEYR1;
    volatile uint32_t OPTKEYR;
    volatile uint32_t CR1;
    volatile uint32_t SR1;
    volatile uint32_t CCR1;
    volatile uint32_t OPTCR;
    volatile uint32_t OPTSR_CUR;
    volatile uint32_t OPTSR_PRG;
    volatile uint32_t OPTCCR;
    volatile uint32_t RESERVED1[6];
    volatile uint32_t KEYR2;
    volatile uint32_t RESERVED2;
    volatile uint32_t CR2;
    volatile uint32_t SR2;
    volatile uint32_t CCR2;
} FLASH_TypeDef;

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

/* Peripheral Pointers */
#define RCC     ((RCC_TypeDef *) RCC_BASE)
#define GPIOB   ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC   ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOE   ((GPIO_TypeDef *) GPIOE_BASE)
#define FLASH   ((FLASH_TypeDef *) FLASH_BASE)
#define TIM2    ((TIM_TypeDef *) TIM2_BASE)
#define TIM3    ((TIM_TypeDef *) TIM3_BASE)
#define EXTI    ((EXTI_TypeDef *) EXTI_BASE)
#define SYSCFG  ((SYSCFG_TypeDef *) SYSCFG_BASE)

#define NVIC_ISER   ((volatile uint32_t *) NVIC_ISER_BASE)

/* ============================================================================
 *  BIT DEFINITIONS
 * ============================================================================ */

/* RCC */
#define RCC_AHB4ENR_GPIOBEN     (1U << 1)
#define RCC_AHB4ENR_GPIOCEN     (1U << 2)
#define RCC_AHB4ENR_GPIOEEN     (1U << 4)
#define RCC_APB4ENR_SYSCFGEN    (1U << 1)
#define RCC_APB1LENR_TIM2EN     (1U << 0)
#define RCC_APB1LENR_TIM3EN     (1U << 1)

/* TIM */
#define TIM_CR1_CEN             (1U << 0)
#define TIM_DIER_UIE            (1U << 0)
#define TIM_SR_UIF              (1U << 0)
#define TIM_EGR_UG              (1U << 0)

/* FLASH */
#define FLASH_SR_BSY            (1U << 0)
#define FLASH_SR_QW             (1U << 2)
#define FLASH_SR_EOP            (1U << 16)
#define FLASH_CR_LOCK           (1U << 0)
#define FLASH_CR_PG             (1U << 1)
#define FLASH_CR_SER            (1U << 2)
#define FLASH_CR_START          (1U << 7)
#define FLASH_CR_FW             (1U << 6)
#define FLASH_KEY1              0x45670123UL
#define FLASH_KEY2              0xCDEF89ABUL

/* EXTI */
#define EXTI_LINE13             (1U << 13)

/* IRQ Numbers */
#define TIM3_IRQn               29
#define EXTI15_10_IRQn          40

/* GPIO Pins */
#define LED_GREEN_PIN           0
#define LED_YELLOW_PIN          1
#define LED_RED_PIN             14
#define BUTTON_PIN              13

/* ============================================================================
 * 
 *  📚 QUICK LESSON: FLASH MEMORY LAYOUT (STM32H7)
 *  ════════════════════════════════════════════════════════════════════════
 *  
 *  The H753 has 2 MB of flash organized in 2 banks:
 *  
 *  Bank 1: 0x08000000 - 0x080FFFFF (1 MB, 8 sectors of 128 KB)
 *  Bank 2: 0x08100000 - 0x081FFFFF (1 MB, 8 sectors of 128 KB)
 *  
 *  ┌────────────────────────────────────────────────────────────────────┐
 *  │ Sector   │ Address Range                 │ Size     │ Usage       │
 *  ├──────────┼───────────────────────────────┼──────────┼─────────────┤
 *  │ Sector 0 │ 0x08000000 - 0x0801FFFF       │ 128 KB   │ Code        │
 *  │ Sector 1 │ 0x08020000 - 0x0803FFFF       │ 128 KB   │ Code        │
 *  │ ...      │ ...                           │ ...      │ ...         │
 *  │ Sector 7 │ 0x080E0000 - 0x080FFFFF       │ 128 KB   │ ← OUR DATA! │
 *  └──────────┴───────────────────────────────┴──────────┴─────────────┘
 *  
 *  We'll use the LAST sector (Sector 7 of Bank 1) to store settings.
 *  This keeps our data safe from code updates.
 *  
 *  ⚠️ IMPORTANT: Flash can only be written 32 bytes at a time!
 *  
 * ============================================================================ */

/* We'll use the last sector of Bank 1 for our data */
#define FLASH_SETTINGS_ADDR     0x080E0000UL
#define FLASH_SETTINGS_SECTOR   7           /* Sector 7 of Bank 1 */

/* Settings signature to detect valid data */
#define SETTINGS_MAGIC          0xDEADBEEF

/* ============================================================================
 *  TEMPO DEFINITIONS
 * ============================================================================ */

typedef enum {
    TEMPO_ANDANTE = 0,      /* 60 BPM */
    TEMPO_MODERATO = 1,     /* 90 BPM */
    TEMPO_ALLEGRO = 2,      /* 120 BPM */
    TEMPO_PRESTO = 3,       /* 180 BPM */
    TEMPO_COUNT = 4
} Tempo_t;

/* BPM values for each tempo */
const uint16_t tempo_bpm[TEMPO_COUNT] = {60, 90, 120, 180};

/* Settings structure (must be 32-byte aligned for flash writes) */
typedef struct __attribute__((aligned(32))) {
    uint32_t magic;         /* 0xDEADBEEF if valid */
    uint32_t tempo_index;   /* 0-3 */
    uint8_t padding[24];    /* Pad to 32 bytes */
} Settings_t;

/* ============================================================================
 *  GLOBAL VARIABLES
 * ============================================================================ */

volatile uint8_t beat_tick = 0;
volatile uint8_t button_pressed = 0;
volatile Tempo_t current_tempo = TEMPO_ANDANTE;

/* ============================================================================
 * 
 *  STEP 1: ENABLE CLOCKS
 *  =======================
 * 
 * ============================================================================ */

void EnableClocks(void) {
    /* ✏️ YOUR TURN: Enable GPIO clocks for ports B, C, E */
    RCC->AHB4ENR |= ???;        /* HINT: RCC_AHB4ENR_GPIOBEN */
    RCC->AHB4ENR |= ???;        /* HINT: RCC_AHB4ENR_GPIOCEN */
    RCC->AHB4ENR |= ???;        /* HINT: RCC_AHB4ENR_GPIOEEN */
    
    /* Enable SYSCFG for EXTI */
    RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
    
    /* Enable TIM2 for delays, TIM3 for metronome beat */
    RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;
    RCC->APB1LENR |= RCC_APB1LENR_TIM3EN;
    
    (void)RCC->APB4ENR;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
 * RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;
 * RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  STEP 2: CONFIGURE GPIO
 *  ========================
 * 
 * ============================================================================ */

void ConfigureGPIO(void) {
    /* Configure Green LED (PB0) as output */
    GPIOB->MODER &= ~(3U << (LED_GREEN_PIN * 2));
    GPIOB->MODER |= (1U << (LED_GREEN_PIN * 2));
    
    /* ✏️ YOUR TURN: Configure Yellow LED (PE1) as output */
    GPIOE->MODER &= ~(???);     /* HINT: (3U << (LED_YELLOW_PIN * 2)) */
    GPIOE->MODER |= (???);      /* HINT: (1U << (LED_YELLOW_PIN * 2)) */
    
    /* Configure Red LED (PB14) as output */
    GPIOB->MODER &= ~(3U << (LED_RED_PIN * 2));
    GPIOB->MODER |= (1U << (LED_RED_PIN * 2));
    
    /* Button as input (default) */
    GPIOC->MODER &= ~(3U << (BUTTON_PIN * 2));
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * GPIOE->MODER &= ~(3U << (LED_YELLOW_PIN * 2));
 * GPIOE->MODER |= (1U << (LED_YELLOW_PIN * 2));
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  STEP 3: CONFIGURE DELAY TIMER (TIM2)
 *  ======================================
 * 
 * ============================================================================ */

void ConfigureDelayTimer(void) {
    TIM2->PSC = 63;             /* 64 MHz / 64 = 1 MHz (1 µs per tick) */
    TIM2->ARR = 0xFFFFFFFF;     /* Max count */
    TIM2->EGR = TIM_EGR_UG;
    TIM2->CR1 |= TIM_CR1_CEN;
}

void delay_ms(uint32_t ms) {
    uint32_t start = TIM2->CNT;
    while ((TIM2->CNT - start) < (ms * 1000));
}

/* ============================================================================
 * 
 *  STEP 4: CONFIGURE METRONOME TIMER (TIM3)
 *  =========================================
 * 
 *  📚 REMINDER: CALCULATING TIMER PERIOD FOR BPM
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *  BPM = Beats Per Minute
 *  
 *  Period (ms) = 60000 / BPM
 *  
 *  Example:
 *  60 BPM → 60000 / 60 = 1000 ms = 1 second per beat
 *  120 BPM → 60000 / 120 = 500 ms per beat
 *  
 *  Timer formula:
 *  ARR = (Timer_Clock / (PSC + 1)) × Period_seconds - 1
 *  
 *  With PSC = 63999 (divide by 64000):
 *  Timer runs at 64 MHz / 64000 = 1 kHz (1 ms per tick)
 *  ARR = Period_ms - 1
 * 
 * ============================================================================ */

void ConfigureMetronomeTimer(uint16_t bpm) {
    uint32_t period_ms;
    
    /* Disable timer during configuration */
    TIM3->CR1 &= ~TIM_CR1_CEN;
    
    /* Calculate period in milliseconds */
    period_ms = 60000 / bpm;
    
    /* ✏️ YOUR TURN: Set prescaler for 1 kHz (1 ms per tick) */
    TIM3->PSC = ???;            /* HINT: 64000 - 1 = 63999 */
    
    /* Set auto-reload for beat period */
    TIM3->ARR = period_ms - 1;
    
    /* Enable update interrupt */
    TIM3->DIER |= TIM_DIER_UIE;
    
    /* Generate update to load values */
    TIM3->EGR = TIM_EGR_UG;
    
    /* Clear pending interrupt */
    TIM3->SR &= ~TIM_SR_UIF;
    
    /* Enable in NVIC */
    NVIC_ISER[0] = (1U << TIM3_IRQn);
    
    /* Start timer */
    TIM3->CR1 |= TIM_CR1_CEN;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * TIM3->PSC = 63999;
 * ───────────────────────────────────────────────────────────────────────────── */

void UpdateMetronomeTempo(uint16_t bpm) {
    uint32_t period_ms = 60000 / bpm;
    TIM3->ARR = period_ms - 1;
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
    SYSCFG->EXTICR[3] |= (0x02U << 4);
    
    /* Falling edge trigger */
    EXTI->FTSR1 |= EXTI_LINE13;
    
    /* Unmask */
    EXTI->IMR1 |= EXTI_LINE13;
    
    /* Enable in NVIC */
    NVIC_ISER[1] = (1U << (EXTI15_10_IRQn - 32));
}

/* ============================================================================
 * 
 *  STEP 6: FLASH OPERATIONS
 *  =========================
 * 
 *  📚 REMINDER: FLASH WRITE SEQUENCE
 *  ─────────────────────────────────────────────────────────────────────────
 *  
 *  1. Unlock flash (write KEY1, then KEY2)
 *  2. Wait for BSY flag to clear
 *  3. Erase sector (if needed)
 *  4. Set PG (program) bit
 *  5. Write data in 32-byte chunks (256-bit flash words)
 *  6. Wait for EOP (End Of Programming)
 *  7. Lock flash
 *  
 *  ⚠️ CAUTION: Erasing takes ~1 second! LED will freeze briefly.
 * 
 * ============================================================================ */

void Flash_Unlock(void) {
    if (FLASH->CR1 & FLASH_CR_LOCK) {
        /* ✏️ YOUR TURN: Write the unlock keys */
        FLASH->KEYR1 = ???;     /* HINT: FLASH_KEY1 */
        FLASH->KEYR1 = ???;     /* HINT: FLASH_KEY2 */
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * FLASH->KEYR1 = FLASH_KEY1;
 * FLASH->KEYR1 = FLASH_KEY2;
 * ───────────────────────────────────────────────────────────────────────────── */

void Flash_Lock(void) {
    FLASH->CR1 |= FLASH_CR_LOCK;
}

void Flash_WaitBusy(void) {
    while (FLASH->SR1 & (FLASH_SR_BSY | FLASH_SR_QW));
}

void Flash_EraseSector(uint32_t sector) {
    Flash_Unlock();
    Flash_WaitBusy();
    
    /* Clear any previous errors */
    FLASH->CCR1 = 0x1EFF0000;
    
    /* Set sector erase and sector number */
    FLASH->CR1 &= ~(7U << 8);           /* Clear SNB bits */
    FLASH->CR1 |= FLASH_CR_SER;         /* Sector erase */
    FLASH->CR1 |= (sector << 8);        /* Sector number */
    
    /* Start erase */
    FLASH->CR1 |= FLASH_CR_START;
    
    /* Wait for completion */
    Flash_WaitBusy();
    
    /* Clear SER bit */
    FLASH->CR1 &= ~FLASH_CR_SER;
    
    Flash_Lock();
}

void Flash_Program256Bits(uint32_t address, uint32_t *data) {
    Flash_Unlock();
    Flash_WaitBusy();
    
    /* Clear any previous errors */
    FLASH->CCR1 = 0x1EFF0000;
    
    /* Enable programming */
    FLASH->CR1 |= FLASH_CR_PG;
    
    /* Write 8 words (32 bytes = 256 bits) */
    volatile uint32_t *dest = (volatile uint32_t *)address;
    for (int i = 0; i < 8; i++) {
        dest[i] = data[i];
    }
    
    /* Force write */
    __asm("DSB");
    
    /* Wait for completion */
    Flash_WaitBusy();
    
    /* Clear PG bit */
    FLASH->CR1 &= ~FLASH_CR_PG;
    
    Flash_Lock();
}

/* ============================================================================
 * 
 *  STEP 7: SAVE AND LOAD SETTINGS
 *  ================================
 * 
 * ============================================================================ */

void SaveSettings(Tempo_t tempo) {
    Settings_t settings;
    
    /* Prepare settings structure */
    settings.magic = SETTINGS_MAGIC;
    settings.tempo_index = tempo;
    
    /* Fill padding with zeros */
    for (int i = 0; i < 24; i++) {
        settings.padding[i] = 0;
    }
    
    /* Erase sector first (required before writing) */
    Flash_EraseSector(FLASH_SETTINGS_SECTOR);
    
    /* Write settings */
    Flash_Program256Bits(FLASH_SETTINGS_ADDR, (uint32_t *)&settings);
}

Tempo_t LoadSettings(void) {
    volatile Settings_t *stored = (volatile Settings_t *)FLASH_SETTINGS_ADDR;
    
    /* ✏️ YOUR TURN: Check if settings are valid */
    if (stored->magic == ???) {     /* HINT: SETTINGS_MAGIC */
        if (stored->tempo_index < TEMPO_COUNT) {
            return (Tempo_t)stored->tempo_index;
        }
    }
    
    /* Default to Andante if no valid settings */
    return TEMPO_ANDANTE;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * if (stored->magic == SETTINGS_MAGIC) {
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 *  LED CONTROL
 * ============================================================================ */

void LED_AllOff(void) {
    GPIOB->BSRR = (1U << (LED_GREEN_PIN + 16));
    GPIOE->BSRR = (1U << (LED_YELLOW_PIN + 16));
    GPIOB->BSRR = (1U << (LED_RED_PIN + 16));
}

void LED_ShowTempo(Tempo_t tempo) {
    LED_AllOff();
    
    switch (tempo) {
        case TEMPO_ANDANTE:     /* Green only */
            GPIOB->BSRR = (1U << LED_GREEN_PIN);
            break;
            
        case TEMPO_MODERATO:    /* Yellow only */
            GPIOE->BSRR = (1U << LED_YELLOW_PIN);
            break;
            
        case TEMPO_ALLEGRO:     /* Red only */
            GPIOB->BSRR = (1U << LED_RED_PIN);
            break;
            
        case TEMPO_PRESTO:      /* All three */
            GPIOB->BSRR = (1U << LED_GREEN_PIN);
            GPIOE->BSRR = (1U << LED_YELLOW_PIN);
            GPIOB->BSRR = (1U << LED_RED_PIN);
            break;
            
        default:
            break;
    }
}

/* ============================================================================
 *  INTERRUPT HANDLERS
 * ============================================================================ */

void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF;    /* Clear flag */
        beat_tick = 1;
    }
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR1 & EXTI_LINE13) {
        EXTI->PR1 = EXTI_LINE13;    /* Clear pending */
        button_pressed = 1;
    }
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
    uint8_t led_on = 0;
    
    /* Initialize peripherals */
    EnableClocks();
    ConfigureGPIO();
    ConfigureDelayTimer();
    ConfigureButtonEXTI();
    
    /* ═══════════════════════════════════════════════════════════════════════
     * LOAD SAVED TEMPO (persists across power cycles!)
     * ═══════════════════════════════════════════════════════════════════════ */
    current_tempo = LoadSettings();
    
    /* Show startup animation */
    for (int i = 0; i < TEMPO_COUNT; i++) {
        LED_ShowTempo((Tempo_t)i);
        delay_ms(200);
    }
    LED_AllOff();
    delay_ms(200);
    
    /* Start metronome at loaded tempo */
    ConfigureMetronomeTimer(tempo_bpm[current_tempo]);
    
    for (;;) {
        /* ═══════════════════════════════════════════════════════════════════
         * HANDLE BEAT
         * ═══════════════════════════════════════════════════════════════════ */
        if (beat_tick) {
            beat_tick = 0;
            
            if (led_on) {
                LED_AllOff();
                led_on = 0;
            } else {
                LED_ShowTempo(current_tempo);
                led_on = 1;
            }
        }
        
        /* ═══════════════════════════════════════════════════════════════════
         * HANDLE BUTTON: CYCLE TEMPO
         * ═══════════════════════════════════════════════════════════════════ */
        if (button_pressed) {
            button_pressed = 0;
            
            /* Simple debounce */
            delay_ms(50);
            
            /* Cycle to next tempo */
            current_tempo = (Tempo_t)((current_tempo + 1) % TEMPO_COUNT);
            
            /* Update timer for new BPM */
            UpdateMetronomeTempo(tempo_bpm[current_tempo]);
            
            /* ═══════════════════════════════════════════════════════════════
             * SAVE TO FLASH
             * This takes ~1 second but tempo survives power loss!
             * ═══════════════════════════════════════════════════════════════ */
            SaveSettings(current_tempo);
            
            /* Show new tempo briefly */
            LED_ShowTempo(current_tempo);
            delay_ms(300);
        }
    }
}

/* ============================================================================
 * 
 *  🎵 HOW TO USE:
 *  
 *  1. Flash this code to your Nucleo board
 *  2. LEDs will cycle through all tempos on startup (animation)
 *  3. Metronome starts at last saved tempo (or 60 BPM if first run)
 *  4. Press button to cycle through tempos:
 *     • Green = 60 BPM (Andante - walking pace)
 *     • Yellow = 90 BPM (Moderato - moderate)
 *     • Red = 120 BPM (Allegro - fast)
 *     • All = 180 BPM (Presto - very fast)
 *  5. Turn off power, wait, turn back on - TEMPO IS PRESERVED!
 *  
 *  
 *  🎓 WHAT YOU LEARNED:
 *  
 *  ✅ FLASH: Sector erase, 256-bit programming, data persistence
 *  ✅ TIM: Generating precise periodic interrupts
 *  ✅ TIM: Calculating ARR from BPM (musical timing)
 *  ✅ GPIO: Multiple LED patterns for different states
 *  ✅ EXTI: Button handling with debouncing
 *  ✅ NVIC: Multiple interrupt sources (timer + button)
 *  ✅ Data Structures: Aligned structures for flash storage
 *  ✅ Magic Numbers: Using signature bytes to validate stored data
 *  
 *  
 *  📚 FLASH PROGRAMMING KEY POINTS:
 *  
 *  1. Flash must be ERASED before writing (sets all bits to 1)
 *  2. Programming can only change 1s to 0s
 *  3. STM32H7 requires 32-byte (256-bit) aligned writes
 *  4. Erasing takes ~1 second (blocking operation)
 *  5. Flash has limited write cycles (~10,000-100,000)
 *  
 *  
 *  🔧 EXPERIMENT IDEAS:
 *  
 *  • Add more tempo presets
 *  • Add a "tap tempo" feature (tap button rhythmically to set BPM)
 *  • Reduce flash wear by only saving if tempo actually changed
 *  • Add visual accent on first beat of measure (flash brighter)
 *  • Store multiple settings (tempo + other preferences)
 * 
 * ============================================================================ */
