/**
 ******************************************************************************
 * @file           : nvic_tutorial.c
 * @brief          : Learning NVIC without HAL - Bare Metal
 ******************************************************************************
 * 
 *  ███╗   ██╗██╗   ██╗██╗ ██████╗    ████████╗██╗   ██╗████████╗ ██████╗ ██████╗ 
 *  ████╗  ██║██║   ██║██║██╔════╝    ╚══██╔══╝██║   ██║╚══██╔══╝██╔═══██╗██╔══██╗
 *  ██╔██╗ ██║██║   ██║██║██║            ██║   ██║   ██║   ██║   ██║   ██║██████╔╝
 *  ██║╚██╗██║╚██╗ ██╔╝██║██║            ██║   ██║   ██║   ██║   ██║   ██║██╔══██╗
 *  ██║ ╚████║ ╚████╔╝ ██║╚██████╗       ██║   ╚██████╔╝   ██║   ╚██████╔╝██║  ██║
 *  ╚═╝  ╚═══╝  ╚═══╝  ╚═╝ ╚═════╝       ╚═╝    ╚═════╝    ╚═╝    ╚═════╝ ╚═╝  ╚═╝
 * 
 *  INTERACTIVE LEARNING: NVIC (Nested Vectored Interrupt Controller)
 * 
 *  WHAT YOU'LL LEARN:
 *  1. What NVIC is and how interrupts work
 *  2. How to enable/disable interrupts
 *  3. How to set interrupt priorities
 *  4. How to handle nested interrupts
 * 
 *  DIFFICULTY: ⭐⭐⭐ (Intermediate)
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 * 
 *  LESSON 0: WHAT IS NVIC?
 *  ========================
 * 
 *  NVIC = Nested Vectored Interrupt Controller
 *  It's part of the ARM Cortex-M core (not STM32 specific).
 *  
 *  KEY FEATURES:
 *  - Up to 240 interrupt sources
 *  - Programmable priority levels (0-255, lower = higher priority)
 *  - Automatic state saving on interrupt entry
 *  - Tail-chaining (fast interrupt-to-interrupt switching)
 *  - Late arrival (higher priority can preempt)
 *  
 *  INTERRUPT FLOW:
 *  
 *  1. Peripheral raises interrupt request
 *  2. NVIC checks if enabled and priority
 *  3. CPU saves context (registers)
 *  4. CPU jumps to interrupt handler (via vector table)
 *  5. Handler executes
 *  6. Handler returns, CPU restores context
 *  7. CPU continues normal execution
 *  
 *  PRIORITY (STM32H7 uses 4 priority bits):
 *  
 *  ┌──────────┬─────────────────────────────────────┐
 *  │ Priority │ Description                         │
 *  ├──────────┼─────────────────────────────────────┤
 *  │ 0        │ Highest priority (most urgent)      │
 *  │ 1        │ ...                                 │
 *  │ 15       │ Lowest priority (least urgent)      │
 *  └──────────┴─────────────────────────────────────┘
 * 
 * ============================================================================ */

/* ============================================================================
 *  NVIC REGISTERS (Part of ARM Cortex-M core)
 *  Located at fixed addresses in the System Control Space
 * ============================================================================ */

/* NVIC Register Addresses */
#define NVIC_ISER_BASE      0xE000E100UL    /* Interrupt Set-Enable Registers */
#define NVIC_ICER_BASE      0xE000E180UL    /* Interrupt Clear-Enable Registers */
#define NVIC_ISPR_BASE      0xE000E200UL    /* Interrupt Set-Pending Registers */
#define NVIC_ICPR_BASE      0xE000E280UL    /* Interrupt Clear-Pending Registers */
#define NVIC_IABR_BASE      0xE000E300UL    /* Interrupt Active Bit Registers */
#define NVIC_IPR_BASE       0xE000E400UL    /* Interrupt Priority Registers */

/* System Control Block */
#define SCB_BASE            0xE000ED00UL
#define SCB_AIRCR           (*(volatile uint32_t *)(SCB_BASE + 0x0C))

/* NVIC register arrays */
#define NVIC_ISER           ((volatile uint32_t *) NVIC_ISER_BASE)
#define NVIC_ICER           ((volatile uint32_t *) NVIC_ICER_BASE)
#define NVIC_ISPR           ((volatile uint32_t *) NVIC_ISPR_BASE)
#define NVIC_ICPR           ((volatile uint32_t *) NVIC_ICPR_BASE)
#define NVIC_IABR           ((volatile uint32_t *) NVIC_IABR_BASE)
#define NVIC_IPR            ((volatile uint8_t *)  NVIC_IPR_BASE)

/* ============================================================================
 * 
 *  LESSON 1: COMMON IRQ NUMBERS
 *  =============================
 * 
 *  Each peripheral has a unique IRQ number. Here are some common ones:
 * 
 * ============================================================================ */

/* Common IRQ numbers for STM32H753 */
#define WWDG_IRQn               0
#define PVD_AVD_IRQn            1
#define RTC_TAMP_STAMP_IRQn     2
#define RTC_WKUP_IRQn           3
#define FLASH_IRQn              4
#define RCC_IRQn                5
#define EXTI0_IRQn              6
#define EXTI1_IRQn              7
#define EXTI2_IRQn              8
#define EXTI3_IRQn              9
#define EXTI4_IRQn              10
#define DMA1_Stream0_IRQn       11
#define DMA1_Stream1_IRQn       12
#define DMA1_Stream2_IRQn       13
#define DMA1_Stream3_IRQn       14
#define DMA1_Stream4_IRQn       15
#define DMA1_Stream5_IRQn       16
#define DMA1_Stream6_IRQn       17
#define ADC_IRQn                18
#define TIM1_BRK_IRQn           23
#define TIM1_UP_IRQn            24
#define TIM1_TRG_COM_IRQn       25
#define TIM1_CC_IRQn            26
#define TIM2_IRQn               28
#define TIM3_IRQn               29
#define TIM4_IRQn               30
#define I2C1_EV_IRQn            31
#define I2C1_ER_IRQn            32
#define SPI1_IRQn               35
#define USART1_IRQn             37
#define USART2_IRQn             38
#define USART3_IRQn             39
#define EXTI15_10_IRQn          40
#define TIM5_IRQn               50
#define SPI2_IRQn               51
#define UART4_IRQn              52
#define TIM6_DAC_IRQn           54
#define TIM7_IRQn               55
#define DMA2_Stream0_IRQn       56
#define I2C3_EV_IRQn            72
#define I2C3_ER_IRQn            73

/* ============================================================================
 * 
 *  ✏️  EXERCISE 1: ENABLE AN INTERRUPT
 *  =====================================
 * 
 *  Each IRQ is controlled by one bit in the ISER (Interrupt Set-Enable Registers) registers .
 *  There are 8 ISER registers (ISER0-ISER7), each with 32 bits.
 *  
 *  To enable IRQ n:
 *    Register index = n / 32
 *    Bit position = n % 32  
 *  
 *  WHY DOES THIS WORK?
 *  ───────────────────────────────────────────────────────────────────
 *  
 *  Each ISER register is 32 bits, so:
 *  - ISER[0] handles IRQs 0-31   (32 interrupts)
 *  - ISER[1] handles IRQs 32-63  (32 interrupts)
 *  - ISER[2] handles IRQs 64-95  (32 interrupts)
 *  - etc.
 *  
 *  Example 1: IRQ 28 (TIM2)
 *    Register = 28 / 32 = 0  → Use ISER[0]
 *    Bit      = 28 % 32 = 28 → Use bit 28
 *    Result: ISER[0] |= (1U << 28)
 *  
 *  Example 2: IRQ 37 (USART1)
 *    Register = 37 / 32 = 1  → Use ISER[1]
 *    Bit      = 37 % 32 = 5  → Use bit 5
 *    Result: ISER[1] |= (1U << 5)
 *  
 *  Example 3: IRQ 72 (I2C3_EV)
 *    Register = 72 / 32 = 2  → Use ISER[2]
 *    Bit      = 72 % 32 = 8  → Use bit 8
 *    Result: ISER[2] |= (1U << 8)
 *  
 *  The division (/) tells us WHICH register to use.
 *  The modulo (%) tells us WHICH BIT in that register. * 
 * ============================================================================ */

void NVIC_EnableIRQ(uint8_t irq) {
    /* ✏️ YOUR TURN: Calculate register index and bit position */
    uint8_t reg = irq / 32;
    uint8_t bit = irq % 32;
    
    /* ✏️ YOUR TURN: Set the enable bit */
    NVIC_ISER[reg] = ???;       /* HINT: Create a mask with just the calculated bit set */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void NVIC_EnableIRQ(uint8_t irq) {
 *     uint8_t reg = irq / 32;
 *     uint8_t bit = irq % 32;
 *     NVIC_ISER[reg] = (1U << bit);
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 2: DISABLE AN INTERRUPT
 *  ======================================
 * 
 *  Use ICER (Interrupt Clear-Enable Register) to disable.
 *  Writing 1 disables the interrupt. Writing 0 does nothing.
 * 
 * ============================================================================ */

void NVIC_DisableIRQ(uint8_t irq) {
    uint8_t reg = irq / 32;
    uint8_t bit = irq % 32;
    
    /* ✏️ YOUR TURN: Clear the enable bit */
    NVIC_ICER[reg] = ???;       /* HINT: Write 1 to the bit position to disable (W1 to clear) */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void NVIC_DisableIRQ(uint8_t irq) {
 *     uint8_t reg = irq / 32;
 *     uint8_t bit = irq % 32;
 *     NVIC_ICER[reg] = (1U << bit);
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 3: SET INTERRUPT PRIORITY
 *  ========================================
 * 
 *  STM32H7 uses 4 priority bits (values 0-15, shifted to upper 4 bits).
 *  Lower number = Higher priority!
 *  
 *  The priority register is 8 bits per IRQ, BUT ONLY UPPER 4 BITS ARE USED!
 *  So priority 0 → 0x00, priority 1 → 0x10, priority 15 → 0xF0
 * 
 * ============================================================================ */

void NVIC_SetPriority(uint8_t irq, uint8_t priority) {
    /* Shift priority to upper 4 bits */
    /* ✏️ YOUR TURN: Write priority (shifted to upper nibble) */
    NVIC_IPR[irq] = ???;        /* HINT: Only upper 4 bits are used. Where should value go? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void NVIC_SetPriority(uint8_t irq, uint8_t priority) {
 *     NVIC_IPR[irq] = (priority << 4);
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 4: CHECK IF INTERRUPT IS PENDING
 *  ===============================================
 * 
 *  An interrupt is "pending" when:
 *  - The peripheral has requested the interrupt
 *  - But the interrupt hasn't been serviced yet
 *  
 *  This happens when:
 *  - The interrupt is disabled in NVIC
 *  - A higher priority interrupt is running
 *  - All interrupts are disabled (__disable_irq)
 *  
 *  Use case: Check if an interrupt triggered before enabling it,
 *            or debug why an interrupt isn't firing.
 * 
 * ============================================================================ */

uint8_t NVIC_GetPending(uint8_t irq) {
    uint8_t reg = irq / 32;
    uint8_t bit = irq % 32;
    
    /* ✏️ YOUR TURN: Check the pending bit */
    return (NVIC_ISPR[reg] & ???) ? 1 : 0;  /* HINT: AND with mask to check bit, convert to 0 or 1 */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * uint8_t NVIC_GetPending(uint8_t irq) {
 *     uint8_t reg = irq / 32;
 *     uint8_t bit = irq % 32;
 *     return (NVIC_ISPR[reg] & (1U << bit)) ? 1 : 0;
 * }
 * 
 * WHY THIS WORKS:
 *   - (1U << bit) creates a mask with only that bit set
 *   - NVIC_ISPR[reg] & mask → if bit is set, result is non-zero
 *   - The ternary operator (? :) converts:
 *       non-zero → 1 (interrupt is pending)
 *       zero     → 0 (interrupt is not pending)
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 5: CLEAR PENDING INTERRUPT
 *  ========================================
 * 
 *  Sometimes you need to manually clear a pending interrupt before or 
 *  after handling it. This prevents the interrupt from firing unexpectedly.
 *  
 *  Use cases:
 *  - Clear old pending interrupts before enabling
 *  - Cancel an interrupt that's no longer needed
 *  - Software-triggered interrupts that you want to abort
 *  
 *  Note: Write 1 to ICPR to clear (not 0!)
 * 
 * ============================================================================ */

void NVIC_ClearPending(uint8_t irq) {
    uint8_t reg = irq / 32;
    uint8_t bit = irq % 32;
    
    /* ✏️ YOUR TURN: Clear the pending bit (write 1 to clear) */
    NVIC_ICPR[reg] = ???;       /* HINT: W1C: Write 1 to the bit to Clear it */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void NVIC_ClearPending(uint8_t irq) {
 *     uint8_t reg = irq / 32;
 *     uint8_t bit = irq % 32;
 *     NVIC_ICPR[reg] = (1U << bit);
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 2: GLOBAL INTERRUPT CONTROL
 *  ====================================
 * 
 *  Sometimes you need to disable ALL interrupts temporarily.
 *  Use PRIMASK register for this.
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 6: DISABLE ALL INTERRUPTS
 *  ========================================
 * 
 * ============================================================================ */

void __disable_irq(void) {
    /* ✏️ YOUR TURN: Set PRIMASK to disable all interrupts */
    __asm volatile ("cpsid i" ::: ???);
    /* HINT: Add "memory" clobber to tell compiler about side effects */
}

void __enable_irq(void) {
    /* ✏️ YOUR TURN: Clear PRIMASK to enable all interrupts */
    __asm volatile ("cpsie i" ::: ???);
    /* HINT: Same clobber as disable - tells compiler memory may change */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void __disable_irq(void) {
 *     __asm volatile ("cpsid i" ::: "memory");
 * }
 * 
 * void __enable_irq(void) {
 *     __asm volatile ("cpsie i" ::: "memory");
 * }
 * 
 * EXPLANATION:
 *   - These use inline assembly (direct CPU instructions)
 *   - "cpsid i" = Change Processor State, Disable Interrupts
 *   - "cpsie i" = Change Processor State, Enable Interrupts
 *   - "memory" clobber tells compiler not to reorder memory operations
 *   
 *   Use case: Critical sections where NO interrupts should occur
 *   ⚠️ WARNING: Keep these sections SHORT! Long periods with interrupts
 *               disabled can cause missed events and timing issues.
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 3: INTERRUPT PRIORITIES IN ACTION
 *  ==========================================
 * 
 *  NESTED INTERRUPTS:
 *  
 *  Task running                    Task running
 *       │                               │
 *       ▼                               ▼
 *  ┌─────────────────────────────────────────────┐
 *  │  Low priority IRQ                          │
 *  │       │                                     │
 *  │       ▼                                     │
 *  │  ┌────────────────────┐                    │
 *  │  │ High priority IRQ  │ ← Preempts!        │
 *  │  └────────────────────┘                    │
 *  │       │                                     │
 *  │       ▼ (returns to low priority)          │
 *  └─────────────────────────────────────────────┘
 *       │
 *       ▼
 *  Task continues
 * 
 * ============================================================================ */

/* Example: Configure USART3 interrupt with medium priority */
void USART3_InterruptConfig(void) {
    /* Set priority to 5 (middle priority) */
    NVIC_SetPriority(USART3_IRQn, 5);
    
    /* Enable the interrupt */
    NVIC_EnableIRQ(USART3_IRQn);
}

/* Example: Configure TIM2 interrupt with high priority */
void TIM2_InterruptConfig(void) {
    /* Set priority to 1 (high priority) */
    NVIC_SetPriority(TIM2_IRQn, 1);
    
    /* Enable the interrupt */
    NVIC_EnableIRQ(TIM2_IRQn);
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
 *  MAIN PROGRAM - Demonstrate NVIC functions
 * 
 * ============================================================================ */

/* Example interrupt handlers */
void TIM2_IRQHandler(void) {
    /* High priority timer interrupt */
    /* Clear interrupt flag, do fast processing */
}

void USART3_IRQHandler(void) {
    /* Medium priority UART interrupt */
    /* Handle received data */
}

int main(void)
{
    /* Example: Configure multiple interrupts with different priorities */
    
    /* TIM2 = Priority 1 (high) - for timing-critical operations */
    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_EnableIRQ(TIM2_IRQn);
    
    /* USART3 = Priority 5 (medium) - for serial communication */
    NVIC_SetPriority(USART3_IRQn, 5);
    NVIC_EnableIRQ(USART3_IRQn);
    
    /* DMA = Priority 2 (high) - for fast data transfer */
    NVIC_SetPriority(DMA1_Stream0_IRQn, 2);
    NVIC_EnableIRQ(DMA1_Stream0_IRQn);
    
    /* EXTI = Priority 8 (low) - for button handling */
    NVIC_SetPriority(EXTI15_10_IRQn, 8);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    
    /* Critical section example */
    __disable_irq();
    /* ... do something that must not be interrupted ... */
    __enable_irq();
    
    for(;;) {
        /* Main loop */
    }
}

/* ============================================================================
 * 
 *  🎉 CONGRATULATIONS!
 *  
 *  You've learned NVIC without HAL:
 *  
 *  ✅ What NVIC is and how interrupts work
 *  ✅ Enabling/disabling individual interrupts
 *  ✅ Setting interrupt priorities
 *  ✅ Checking and clearing pending interrupts
 *  ✅ Global interrupt enable/disable
 *  ✅ Nested interrupt concept
 *  
 *  PRIORITY GUIDELINES:
 *  • 0-3: Critical (motor control, safety)
 *  • 4-7: High (timers, DMA, fast peripherals)
 *  • 8-11: Medium (UART, SPI, I2C)
 *  • 12-15: Low (buttons, non-critical events)
 *  
 *  TIPS:
 *  • Keep interrupt handlers SHORT
 *  • Don't call blocking functions in ISRs
 *  • Use volatile for shared variables
 *  • Be careful with priority inversion
 * 
 * ============================================================================ */
