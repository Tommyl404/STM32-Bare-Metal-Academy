/**
 ******************************************************************************
 * @file           : gpio_tutorial.c
 * @brief          : Learning GPIO without HAL - Bare Metal
 ******************************************************************************
 * 
 *   ██████╗ ██████╗ ██╗ ██████╗     ████████╗██╗   ██╗████████╗ ██████╗ ██████╗ 
 *  ██╔════╝ ██╔══██╗██║██╔═══██╗    ╚══██╔══╝██║   ██║╚══██╔══╝██╔═══██╗██╔══██╗
 *  ██║  ███╗██████╔╝██║██║   ██║       ██║   ██║   ██║   ██║   ██║   ██║██████╔╝
 *  ██║   ██║██╔═══╝ ██║██║   ██║       ██║   ██║   ██║   ██║   ██║   ██║██╔══██╗
 *  ╚██████╔╝██║     ██║╚██████╔╝       ██║   ╚██████╔╝   ██║   ╚██████╔╝██║  ██║
 *   ╚═════╝ ╚═╝     ╚═╝ ╚═════╝        ╚═╝    ╚═════╝    ╚═╝    ╚═════╝ ╚═╝  ╚═╝
 * 
 *  INTERACTIVE LEARNING: GPIO (General Purpose Input/Output)
 * 
 *  WHAT YOU'LL LEARN:
 *  1. What GPIO is and how pins work
 *  2. How to configure pins as OUTPUT (LED blinking)
 *  3. How to configure pins as INPUT (button reading)
 *  4. Pull-up/Pull-down resistors
 *  5. Output speed and output types
 *  6. Alternate Functions (for peripherals)
 * 
 *  HARDWARE (Nucleo-H753ZI):
 *  - PB0  = LED1 (Green)
 *  - PB7  = LED2 (Blue)  
 *  - PB14 = LED3 (Red)
 *  - PC13 = USER Button (active LOW with external pull-up)
 * 
 *  DIFFICULTY: ⭐ (Beginner - Start Here!)
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 * 
 *  LESSON 0: WHAT IS GPIO?
 *  ========================
 * 
 *  GPIO = General Purpose Input/Output
 *  
 *  Each pin on the microcontroller can be configured as:
 *  - INPUT:  Read external signals (buttons, sensors)
 *  - OUTPUT: Control external devices (LEDs, relays)
 *  - ALTERNATE FUNCTION: Connect to peripherals (UART, SPI, I2C)
 *  - ANALOG: For ADC/DAC
 *  
 *  VISUAL: A GPIO Pin
 *  ──────────────────────────────────────────────────────────────
 *  
 *                    VDD (3.3V)
 *                      │
 *                     [R] ← Pull-up resistor (optional)
 *                      │
 *    External    ┌─────┴─────┐
 *    World  ─────┤    PIN    ├───── Internal Logic
 *                └─────┬─────┘
 *                      │
 *                     [R] ← Pull-down resistor (optional)
 *                      │
 *                     GND
 *  
 *  
 *  PIN MODES (MODER register - 2 bits per pin):
 *  ┌───────┬────────────────────────────────────┐
 *  │ Mode  │ Description                        │
 *  ├───────┼────────────────────────────────────┤
 *  │  00   │ INPUT (default after reset)        │
 *  │  01   │ OUTPUT                             │
 *  │  10   │ ALTERNATE FUNCTION (UART, SPI...)  │
 *  │  11   │ ANALOG (ADC, DAC)                  │
 *  └───────┴────────────────────────────────────┘
 * 
 * ============================================================================ */

/* ============================================================================
 *  PERIPHERAL BASE ADDRESSES
 * ============================================================================ */
#define RCC_BASE        0x58024400UL
#define GPIOA_BASE      0x58020000UL
#define GPIOB_BASE      0x58020400UL
#define GPIOC_BASE      0x58020800UL

/* ============================================================================
 *  RCC REGISTERS (Clock Control)
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
 * 
 *  LESSON 1: GPIO REGISTER STRUCTURE
 *  ===================================
 * 
 *  Each GPIO port (A, B, C...) has these registers:
 *  
 *  ┌───────────┬─────────────────────────────────────────────────┐
 *  │ Register  │ Purpose                                         │
 *  ├───────────┼─────────────────────────────────────────────────┤
 *  │ MODER     │ Mode: Input/Output/AF/Analog (2 bits per pin)   │
 *  │ OTYPER    │ Output Type: Push-Pull/Open-Drain (1 bit)       │
 *  │ OSPEEDR   │ Output Speed: Low/Medium/High/Very High (2 bits)│
 *  │ PUPDR     │ Pull-Up/Pull-Down: None/Up/Down (2 bits)        │
 *  │ IDR       │ Input Data Register (READ pin state)            │
 *  │ ODR       │ Output Data Register (SET pin state)            │
 *  │ BSRR      │ Bit Set/Reset Register (atomic set/clear)       │
 *  │ AFR[2]    │ Alternate Function (4 bits per pin)             │
 *  └───────────┴─────────────────────────────────────────────────┘
 * 
 * ============================================================================ */

typedef struct {
    volatile uint32_t MODER;    /* 0x00 - Mode register */
    volatile uint32_t OTYPER;   /* 0x04 - Output type register */
    volatile uint32_t OSPEEDR;  /* 0x08 - Output speed register */
    volatile uint32_t PUPDR;    /* 0x0C - Pull-up/pull-down register */
    volatile uint32_t IDR;      /* 0x10 - Input data register */
    volatile uint32_t ODR;      /* 0x14 - Output data register */
    volatile uint32_t BSRR;     /* 0x18 - Bit set/reset register */
    volatile uint32_t LCKR;     /* 0x1C - Lock register */
    volatile uint32_t AFR[2];   /* 0x20-0x24 - Alternate function registers */
} GPIO_TypeDef;

#define GPIOA   ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB   ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC   ((GPIO_TypeDef *) GPIOC_BASE)

/* ============================================================================
 * 
 *  LESSON 2: IMPORTANT BIT DEFINITIONS
 *  ====================================
 * 
 * ============================================================================ */

/* RCC Clock Enable for GPIO */
#define RCC_AHB4ENR_GPIOAEN     (1U << 0)
#define RCC_AHB4ENR_GPIOBEN     (1U << 1)
#define RCC_AHB4ENR_GPIOCEN     (1U << 2)

/* GPIO MODER Values (2 bits per pin) */
#define GPIO_MODE_INPUT         0U      /* 00: Input mode */
#define GPIO_MODE_OUTPUT        1U      /* 01: Output mode */
#define GPIO_MODE_AF            2U      /* 10: Alternate function */
#define GPIO_MODE_ANALOG        3U      /* 11: Analog mode */

/* GPIO OTYPER Values (1 bit per pin) */
#define GPIO_OTYPE_PUSHPULL     0U      /* 0: Push-pull */
#define GPIO_OTYPE_OPENDRAIN    1U      /* 1: Open-drain */

/* GPIO OSPEEDR Values (2 bits per pin) */
#define GPIO_SPEED_LOW          0U      /* 00: Low speed */
#define GPIO_SPEED_MEDIUM       1U      /* 01: Medium speed */
#define GPIO_SPEED_HIGH         2U      /* 10: High speed */
#define GPIO_SPEED_VERYHIGH     3U      /* 11: Very high speed */

/* GPIO PUPDR Values (2 bits per pin) */
#define GPIO_PUPD_NONE          0U      /* 00: No pull-up/pull-down */
#define GPIO_PUPD_PULLUP        1U      /* 01: Pull-up */
#define GPIO_PUPD_PULLDOWN      2U      /* 10: Pull-down */

/* ============================================================================
 * 
 *  LESSON 3: THE BIT POSITION FORMULA
 *  ====================================
 * 
 *  MODER, OSPEEDR, PUPDR use 2 bits per pin.
 *  
 *  Formula: bit_position = pin_number × 2
 *  
 *  Example for Pin 7:
 *  bit_position = 7 × 2 = 14
 *  
 *  To SET bits: register |= (value << (pin × 2))
 *  To CLEAR bits: register &= ~(0x3 << (pin × 2))
 *  
 *  VISUAL: MODER Register (32 bits)
 *  ┌────┬────┬────┬────┬────┬────┬────┬────┬ ─ ─ ┬────┬────┐
 *  │P15 │P14 │P13 │P12 │P11 │P10 │ P9 │ P8 │     │ P1 │ P0 │
 *  │31:30│29:28│27:26│25:24│23:22│21:20│19:18│17:16│     │3:2 │1:0 │
 *  └────┴────┴────┴────┴────┴────┴────┴────┴ ─ ─ ┴────┴────┘
 *         2 bits per pin!
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 1: ENABLE GPIO CLOCKS
 *  ====================================
 * 
 *  Before using any GPIO, you MUST enable its clock!
 *  GPIO ports are on the AHB4 bus.
 * 
 * ============================================================================ */

void GPIO_EnableClocks(void) {
    /* ✏️ YOUR TURN: Enable GPIOB clock (for LEDs) */
    RCC->AHB4ENR |= ???;        /* HINT: Which bit in AHB4ENR enables port B? Check the define names... */
    
    /* ✏️ YOUR TURN: Enable GPIOC clock (for button) */
    RCC->AHB4ENR |= ???;        /* HINT: Same pattern as above, but for port C */
    
    /* Dummy read to ensure clock is enabled */
    (void)RCC->AHB4ENR;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void GPIO_EnableClocks(void) {
 *     RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
 *     RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;
 *     (void)RCC->AHB4ENR;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 2: CONFIGURE LED AS OUTPUT
 *  ========================================
 * 
 *  Configure PB0 (LED1 Green) as push-pull output.
 * 
 *  Steps:
 *  1. Clear the mode bits for pin 0
 *  2. Set mode to OUTPUT (01)
 * 
 * ============================================================================ */

void LED_Init(void) {
    /* ────────────────────────────────────────────────────────────────────────
     * Step 1: Clear mode bits for PB0 (bits 1:0)
     * ──────────────────────────────────────────────────────────────────────── */
    
    /* ✏️ YOUR TURN: Clear bits [1:0] using AND with inverted mask */
    GPIOB->MODER &= ???;        /* HINT: To CLEAR bits, AND with inverted mask. Pin 0 uses bits [1:0]. What mask covers 2 bits? */
    
    /* ────────────────────────────────────────────────────────────────────────
     * Step 2: Set mode to OUTPUT (01)
     * ──────────────────────────────────────────────────────────────────────── */
    
    /* ✏️ YOUR TURN: Set bits [1:0] to 01 (output mode) */
    GPIOB->MODER |= ???;        /* HINT: Output mode = 01. What value shifted to pin 0's position gives 01 in bits [1:0]? */
    
    /* Output type: Push-pull (default, bit = 0) */
    GPIOB->OTYPER &= ~(1U << 0);
    
    /* Speed: Low speed is fine for LED */
    GPIOB->OSPEEDR &= ~(3U << (0 * 2));
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void LED_Init(void) {
 *     GPIOB->MODER &= ~(3U << (0 * 2));  // Clear bits 1:0
 *     GPIOB->MODER |= (1U << (0 * 2));   // Set to 01 (output)
 *     GPIOB->OTYPER &= ~(1U << 0);       // Push-pull
 *     GPIOB->OSPEEDR &= ~(3U << (0 * 2));// Low speed
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 3: TURN LED ON AND OFF
 *  =====================================
 * 
 *  Use ODR (Output Data Register) to control the LED.
 *  
 *  ODR is simple:
 *  - Bit = 1 → Pin is HIGH (3.3V) → LED ON
 *  - Bit = 0 → Pin is LOW (0V) → LED OFF
 * 
 * ============================================================================ */

void LED_On(void) {
    /* ✏️ YOUR TURN: Set bit 0 to turn LED on */
    GPIOB->ODR |= ???;          /* HINT: OR with a mask that has only bit 0 set */
}

void LED_Off(void) {
    /* ✏️ YOUR TURN: Clear bit 0 to turn LED off */
    GPIOB->ODR &= ???;          /* HINT: AND with inverted mask to clear. Think: ~(mask for bit 0) */
}

void LED_Toggle(void) {
    /* ✏️ YOUR TURN: Toggle bit 0 using XOR */
    GPIOB->ODR ^= ???;          /* HINT: XOR with a mask flips those bits. Which bit is LED on? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void LED_On(void)     { GPIOB->ODR |= (1U << 0); }
 * void LED_Off(void)    { GPIOB->ODR &= ~(1U << 0); }
 * void LED_Toggle(void) { GPIOB->ODR ^= (1U << 0); }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 4: BSRR - ATOMIC BIT OPERATIONS
 *  =======================================
 * 
 *  BSRR (Bit Set Reset Register) is BETTER than ODR for setting/clearing bits!
 *  
 *  Why? It's ATOMIC - no read-modify-write race condition!
 *  
 *  ┌─────────────────────────────────────────────────────────────┐
 *  │ BSRR Register (32 bits)                                    │
 *  ├─────────────────────────────┬───────────────────────────────┤
 *  │ Bits [31:16] = RESET bits   │ Bits [15:0] = SET bits        │
 *  │ Write 1 to CLEAR a pin      │ Write 1 to SET a pin          │
 *  └─────────────────────────────┴───────────────────────────────┘
 *  
 *  Example: Set PB0
 *  GPIOB->BSRR = (1U << 0);      // Sets pin 0 HIGH
 *  
 *  Example: Reset PB0
 *  GPIOB->BSRR = (1U << 16);     // Sets pin 0 LOW (bit 0 + 16)
 * 
 * ============================================================================ */

void LED_On_Atomic(void) {
    GPIOB->BSRR = (1U << 0);        /* Set bit 0 */
}

void LED_Off_Atomic(void) {
    GPIOB->BSRR = (1U << (0 + 16)); /* Reset bit 0 (bit 16) */
}

/* ============================================================================
 * 
 *  ✏️  EXERCISE 4: CONFIGURE BUTTON AS INPUT
 *  ===========================================
 * 
 *  Configure PC13 (USER Button) as input.
 *  The Nucleo board has an external pull-up, so no internal pull needed.
 *  
 *  Button behavior:
 *  - Not pressed: PC13 = HIGH (1) due to pull-up
 *  - Pressed: PC13 = LOW (0) connected to GND
 * 
 * ============================================================================ */

void Button_Init(void) {
    /* ✏️ YOUR TURN: Clear mode bits for PC13 (set to input = 00) */
    GPIOC->MODER &= ???;        /* HINT: Input = 00. Clear both mode bits for pin 13. Bit position = pin × 2 */
    
    /* No pull-up/pull-down needed (external pull-up on board) */
    GPIOC->PUPDR &= ~(3U << (13 * 2));
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void Button_Init(void) {
 *     GPIOC->MODER &= ~(3U << (13 * 2));  // Input mode = 00
 *     GPIOC->PUPDR &= ~(3U << (13 * 2));  // No pull
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 5: READ BUTTON STATE
 *  ==================================
 * 
 *  Use IDR (Input Data Register) to read the pin state.
 * 
 * ============================================================================ */

uint8_t Button_IsPressed(void) {
    /* ✏️ YOUR TURN: Read bit 13 from IDR
     * Return 1 if pressed (pin is LOW), 0 if not pressed */
    if (GPIOC->IDR & ???) {     /* HINT: Create a mask to test pin 13. IDR has 1 bit per pin. */
        return 0;   /* Pin HIGH = not pressed */
    } else {
        return 1;   /* Pin LOW = pressed */
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * uint8_t Button_IsPressed(void) {
 *     if (GPIOC->IDR & (1U << 13)) {
 *         return 0;  // HIGH = not pressed
 *     } else {
 *         return 1;  // LOW = pressed
 *     }
 * }
 * 
 * // Or shorter version:
 * uint8_t Button_IsPressed(void) {
 *     return !(GPIOC->IDR & (1U << 13));
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 5: PULL-UP AND PULL-DOWN RESISTORS
 *  ==========================================
 * 
 *  When a pin is floating (not connected), it reads random garbage!
 *  Pull resistors give it a default state.
 *  
 *  PULL-UP: Pin defaults to HIGH (1)
 *  ─────────────────────────────────
 *       VDD (3.3V)
 *         │
 *        [R] Pull-up resistor (inside chip)
 *         │
 *    ─────●───── Pin (reads HIGH when floating)
 *  
 *  
 *  PULL-DOWN: Pin defaults to LOW (0)
 *  ──────────────────────────────────
 *    ─────●───── Pin (reads LOW when floating)
 *         │
 *        [R] Pull-down resistor (inside chip)
 *         │
 *        GND
 *  
 *  
 *  PUPDR Values:
 *  00 = No pull-up/pull-down
 *  01 = Pull-up
 *  10 = Pull-down
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 6: CONFIGURE INPUT WITH PULL-UP
 *  ==============================================
 * 
 *  Configure PA0 as input with internal pull-up.
 *  (Useful if you have a button connected to GND)
 * 
 * ============================================================================ */

void Input_WithPullUp_Init(void) {
    /* Enable GPIOA clock */
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
    
    /* Set PA0 as input (mode = 00) */
    GPIOA->MODER &= ~(3U << (0 * 2));
    
    /* ✏️ YOUR TURN: Enable pull-up on PA0 */
    GPIOA->PUPDR &= ~(3U << (0 * 2));   /* Clear first */
    GPIOA->PUPDR |= ???;        /* HINT: PUPDR uses 2 bits per pin. 01 = pull-up. What value at pin 0's position? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void Input_WithPullUp_Init(void) {
 *     RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
 *     GPIOA->MODER &= ~(3U << (0 * 2));
 *     GPIOA->PUPDR &= ~(3U << (0 * 2));
 *     GPIOA->PUPDR |= (1U << (0 * 2));   // 01 = Pull-up
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 6: OUTPUT TYPES
 *  =======================
 * 
 *  PUSH-PULL (default):
 *  - Can drive HIGH or LOW
 *  - Actively pushes current both ways
 *  - Use for: LEDs, most outputs
 *  
 *       VDD
 *        │
 *       [T] ← Upper transistor (pushes HIGH)
 *        │
 *    ────●──── Output Pin
 *        │
 *       [T] ← Lower transistor (pulls LOW)
 *        │
 *       GND
 *  
 *  
 *  OPEN-DRAIN:
 *  - Can only drive LOW (or float)
 *  - Needs external pull-up for HIGH
 *  - Use for: I2C, shared buses, level shifting
 *  
 *        │
 *       [R] ← External pull-up required!
 *        │
 *    ────●──── Output Pin
 *        │
 *       [T] ← Only lower transistor
 *        │
 *       GND
 * 
 * ============================================================================ */

void Output_OpenDrain_Init(void) {
    /* Set PB1 as output */
    GPIOB->MODER &= ~(3U << (1 * 2));
    GPIOB->MODER |= (1U << (1 * 2));
    
    /* Set as open-drain */
    GPIOB->OTYPER |= (1U << 1);     /* 1 = Open-drain */
    
    /* Enable internal pull-up (or use external) */
    GPIOB->PUPDR &= ~(3U << (1 * 2));
    GPIOB->PUPDR |= (1U << (1 * 2)); /* Pull-up */
}

/* ============================================================================
 * 
 *  LESSON 7: OUTPUT SPEED
 *  =======================
 * 
 *  Speed affects how fast the pin can change state.
 *  Higher speed = faster edges, but more noise/EMI.
 *  
 *  ┌────────────┬───────────────────┬─────────────────────────┐
 *  │ Speed      │ Approx. Frequency │ Use Case                │
 *  ├────────────┼───────────────────┼─────────────────────────┤
 *  │ Low (00)   │ ~4 MHz            │ LEDs, slow signals      │
 *  │ Medium (01)│ ~25 MHz           │ General purpose         │
 *  │ High (10)  │ ~50 MHz           │ SPI, fast GPIO          │
 *  │ VeryHigh(11)│ ~100 MHz         │ High-speed interfaces   │
 *  └────────────┴───────────────────┴─────────────────────────┘
 *  
 *  Rule: Use the LOWEST speed that works!
 *  - LEDs: Low speed
 *  - SPI/UART: High speed
 *  - SDIO/Ethernet: Very High speed
 * 
 * ============================================================================ */

void Output_HighSpeed_Init(void) {
    /* Set PB7 (LED2) as output with high speed */
    GPIOB->MODER &= ~(3U << (7 * 2));
    GPIOB->MODER |= (1U << (7 * 2));     /* Output */
    
    /* Set high speed */
    GPIOB->OSPEEDR &= ~(3U << (7 * 2));
    GPIOB->OSPEEDR |= (2U << (7 * 2));   /* 10 = High speed */
}

/* ============================================================================
 * 
 *  LESSON 8: ALTERNATE FUNCTIONS
 *  ==============================
 * 
 *  Pins can be connected to peripherals like UART, SPI, I2C, Timers.
 *  
 *  Each pin has up to 16 alternate functions (AF0-AF15).
 *  Check the datasheet for which AF connects to which peripheral!
 *  
 *  Example: PA9 can be USART1_TX (AF7)
 *  
 *  AFR[0] = Alternate function for pins 0-7 (4 bits each)
 *  AFR[1] = Alternate function for pins 8-15 (4 bits each)
 *  
 *  Formula for AFR[0] (pins 0-7):  bit_position = pin × 4
 *  Formula for AFR[1] (pins 8-15): bit_position = (pin - 8) × 4
 * 
 * ============================================================================ */

void GPIO_SetAlternateFunction(GPIO_TypeDef *port, uint8_t pin, uint8_t af) {
    /* Set mode to Alternate Function (10) */
    port->MODER &= ~(3U << (pin * 2));
    port->MODER |= (2U << (pin * 2));
    
    /* Set the alternate function number */
    if (pin < 8) {
        port->AFR[0] &= ~(0xFU << (pin * 4));
        port->AFR[0] |= (af << (pin * 4));
    } else {
        port->AFR[1] &= ~(0xFU << ((pin - 8) * 4));
        port->AFR[1] |= (af << ((pin - 8) * 4));
    }
}

/* ============================================================================
 *  DELAY HELPER
 * ============================================================================ */

void delay(volatile uint32_t count) {
    while (count--);
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
 *  MAIN PROGRAM - Blink LED and respond to button
 * 
 * ============================================================================ */

int main(void)
{
    /* Enable clocks for GPIO B and C */
    GPIO_EnableClocks();
    
    /* Initialize LED on PB0 */
    LED_Init();
    
    /* Initialize button on PC13 */
    Button_Init();
    
    for(;;) {
        /* If button is pressed, LED on. Otherwise blink. */
        if (Button_IsPressed()) {
            LED_On();
        } else {
            LED_Toggle();
            delay(1000000);
        }
    }
}

/* ============================================================================
 * 
 *  🎉 CONGRATULATIONS!
 *  
 *  You've learned GPIO without HAL:
 *  
 *  ✅ Enabling GPIO clocks
 *  ✅ Configuring pins as OUTPUT
 *  ✅ Configuring pins as INPUT
 *  ✅ Using ODR to set/clear pins
 *  ✅ Using BSRR for atomic operations
 *  ✅ Using IDR to read input
 *  ✅ Pull-up and pull-down resistors
 *  ✅ Output types (push-pull vs open-drain)
 *  ✅ Output speed configuration
 *  ✅ Alternate function configuration
 *  
 *  KEY FORMULAS TO REMEMBER:
 *  ─────────────────────────────────────────
 *  MODER/OSPEEDR/PUPDR: bit_position = pin × 2
 *  AFR[0] (pins 0-7):   bit_position = pin × 4
 *  AFR[1] (pins 8-15):  bit_position = (pin - 8) × 4
 *  
 *  COMMON OPERATIONS:
 *  ─────────────────────────────────────────
 *  Set output HIGH:   GPIOx->ODR |= (1U << pin);
 *  Set output LOW:    GPIOx->ODR &= ~(1U << pin);
 *  Toggle output:     GPIOx->ODR ^= (1U << pin);
 *  Read input:        if (GPIOx->IDR & (1U << pin))
 *  
 *  NEXT STEPS:
 *  • Try blinking multiple LEDs
 *  • Add debouncing to button reading
 *  • Use timers for precise timing
 *  • Try external interrupts (EXTI) for buttons
 * 
 * ============================================================================ */
