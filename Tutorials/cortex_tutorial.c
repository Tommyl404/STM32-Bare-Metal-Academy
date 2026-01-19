/**
 ******************************************************************************
 * @file           : cortex_tutorial.c
 * @brief          : Learning ARM Cortex-M7 CPU - Bare Metal
 ******************************************************************************
 * 
 *   ██████╗ ██████╗ ██████╗ ████████╗███████╗██╗  ██╗      ███╗   ███╗███████╗
 *  ██╔════╝██╔═══██╗██╔══██╗╚══██╔══╝██╔════╝╚██╗██╔╝      ████╗ ████║╚════██║
 *  ██║     ██║   ██║██████╔╝   ██║   █████╗   ╚███╔╝ █████╗██╔████╔██║    ██╔╝
 *  ██║     ██║   ██║██╔══██╗   ██║   ██╔══╝   ██╔██╗ ╚════╝██║╚██╔╝██║   ██╔╝ 
 *  ╚██████╗╚██████╔╝██║  ██║   ██║   ███████╗██╔╝ ██╗      ██║ ╚═╝ ██║   ██║  
 *   ╚═════╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚══════╝╚═╝  ╚═╝      ╚═╝     ╚═╝   ╚═╝  
 * 
 *  INTERACTIVE LEARNING: ARM CORTEX-M7 CORE
 * 
 *  WHAT YOU'LL LEARN:
 *  1. What is the Cortex-M7 processor
 *  2. CPU registers (R0-R15, PSR, CONTROL)
 *  3. Memory architecture
 *  4. Stack operations
 *  5. Inline assembly basics
 *  6. Core system control registers
 * 
 *  CORTEX-M7:
 *  - 32-bit ARM processor
 *  - Harvard architecture (separate instruction/data buses)
 *  - 6-stage superscalar pipeline
 *  - Hardware FPU (Floating Point Unit)
 *  - Memory Protection Unit (MPU)
 * 
 *  DIFFICULTY: ⭐⭐⭐⭐ (Advanced)
 * 
 ******************************************************************************
 */

#include <stdint.h>

/* ============================================================================
 * 
 *  LESSON 0: WHAT IS CORTEX-M7?
 *  =============================
 * 
 *  Cortex-M7 is the ARM processor CORE inside the STM32H753 chip.
 *  
 *  CHIP STRUCTURE:
 *  ┌──────────────────────────────────────────────────────────┐
 *  │  STM32H753 Microcontroller                               │
 *  │                                                          │
 *  │  ┌────────────────────────────────────────────┐          │
 *  │  │  ARM Cortex-M7 CPU CORE                    │          │
 *  │  │                                            │          │
 *  │  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  │          │
 *  │  │  │  ALU     │  │   FPU    │  │  Cache   │  │          │
 *  │  │  │  (Math)  │  │ (Float)  │  │ (Fast!)  │  │          │
 *  │  │  └──────────┘  └──────────┘  └──────────┘  │          │
 *  │  │                                            │          │
 *  │  │  Registers: R0-R15, PSR, CONTROL           │          │
 *  │  └────────────────────────────────────────────┘          │
 *  │           │                                              │
 *  │           ├──► AHB4 Bus ──► GPIO, RCC, etc.              │
 *  │           ├──► AHB1 Bus ──► DMA, Flash                   │
 *  │           └──► APB Buses ──► UART, SPI, I2C              │
 *  │                                                          │
 *  │  [Flash Memory]  [SRAM]  [Peripherals]                   │
 *  └──────────────────────────────────────────────────────────┘
 * 
 *  KEY FEATURES:
 *  • 32-bit RISC processor
 *  • Thumb-2 instruction set (16-bit and 32-bit instructions)
 *  • Up to 480 MHz clock
 *  • Hardware multiply/divide
 *  • Double-precision FPU
 *  • I-Cache and D-Cache (16KB each)
 *  • Branch prediction
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  LESSON 1: CPU REGISTERS
 *  ========================
 * 
 *  The Cortex-M7 has 16 main registers:
 *  
 *  R0-R12:  General purpose registers (your variables)
 *  R13:     Stack Pointer (SP)
 *  R14:     Link Register (LR) - stores return address
 *  R15:     Program Counter (PC) - current instruction address
 *  
 *  SPECIAL REGISTERS:
 *  PSR:     Program Status Register (flags: N, Z, C, V)
 *  CONTROL: Control register (privilege level, stack selection)
 *  
 *  VISUAL:
 *  ┌─────────┬─────────────────────────────────────────┐
 *  │   R0    │  General Purpose (calculations)         │
 *  │   R1    │  General Purpose                        │
 *  │   R2    │  General Purpose                        │
 *  │   R3    │  General Purpose                        │
 *  │   R4    │  General Purpose                        │
 *  │   R5    │  General Purpose                        │
 *  │   R6    │  General Purpose                        │
 *  │   R7    │  General Purpose                        │
 *  │   R8    │  General Purpose                        │
 *  │   R9    │  General Purpose                        │
 *  │  R10    │  General Purpose                        │
 *  │  R11    │  General Purpose                        │
 *  │  R12    │  General Purpose                        │
 *  ├─────────┼─────────────────────────────────────────┤
 *  │  R13/SP │  Stack Pointer (top of stack)           │
 *  │  R14/LR │  Link Register (return address)         │
 *  │  R15/PC │  Program Counter (current instruction)  │
 *  └─────────┴─────────────────────────────────────────┘
 * 
 * ============================================================================ */

/* ============================================================================
 *  SYSTEM CONTROL BLOCK (SCB) - Cortex-M7 Core Registers
 * ============================================================================ */
#define SCB_BASE            0xE000ED00UL
#define SYSTICK_BASE        0xE000E010UL

typedef struct {
    volatile uint32_t CPUID;        /* 0x00 - CPU ID Base Register */
    volatile uint32_t ICSR;         /* 0x04 - Interrupt Control State Register */
    volatile uint32_t VTOR;         /* 0x08 - Vector Table Offset Register */
    volatile uint32_t AIRCR;        /* 0x0C - App Interrupt/Reset Control Register */
    volatile uint32_t SCR;          /* 0x10 - System Control Register */
    volatile uint32_t CCR;          /* 0x14 - Configuration Control Register */
    volatile uint8_t  SHP[12];      /* 0x18 - System Handler Priority Registers */
    volatile uint32_t SHCSR;        /* 0x24 - System Handler Control State Register */
    volatile uint32_t CFSR;         /* 0x28 - Configurable Fault Status Register */
    volatile uint32_t HFSR;         /* 0x2C - HardFault Status Register */
    volatile uint32_t DFSR;         /* 0x30 - Debug Fault Status Register */
    volatile uint32_t MMFAR;        /* 0x34 - MemManage Fault Address Register */
    volatile uint32_t BFAR;         /* 0x38 - BusFault Address Register */
    volatile uint32_t AFSR;         /* 0x3C - Auxiliary Fault Status Register */
    volatile uint32_t RESERVED0[18];
    volatile uint32_t CPACR;        /* 0x88 - Coprocessor Access Control Register */
} SCB_TypeDef;

#define SCB     ((SCB_TypeDef *) SCB_BASE)

/* SysTick Timer */
typedef struct {
    volatile uint32_t CTRL;         /* 0x00 - Control and Status Register */
    volatile uint32_t LOAD;         /* 0x04 - Reload Value Register */
    volatile uint32_t VAL;          /* 0x08 - Current Value Register */
    volatile uint32_t CALIB;        /* 0x0C - Calibration Register */
} SysTick_TypeDef;

#define SysTick ((SysTick_TypeDef *) SYSTICK_BASE)

/* ============================================================================
 * 
 *  LESSON 2: IMPORTANT BIT DEFINITIONS
 *  ====================================
 * 
 * ============================================================================ */

/* SCB CPUID Register */
#define SCB_CPUID_IMPLEMENTER_POS   24
#define SCB_CPUID_VARIANT_POS       20
#define SCB_CPUID_ARCHITECTURE_POS  16
#define SCB_CPUID_PARTNO_POS        4
#define SCB_CPUID_REVISION_POS      0

/* SCB AIRCR Register */
#define SCB_AIRCR_VECTKEY_POS       16
#define SCB_AIRCR_VECTKEYSTAT       (0x05FAUL << SCB_AIRCR_VECTKEY_POS)
#define SCB_AIRCR_PRIGROUP_POS      8
#define SCB_AIRCR_SYSRESETREQ       (1U << 2)

/* SCB CCR Register */
#define SCB_CCR_IC                  (1U << 17)  /* Instruction cache enable */
#define SCB_CCR_DC                  (1U << 16)  /* Data cache enable */
#define SCB_CCR_STKALIGN            (1U << 9)   /* Stack alignment */
#define SCB_CCR_UNALIGN_TRP         (1U << 3)   /* Unaligned access trap */

/* SCB CPACR Register (Coprocessor Access) */
#define SCB_CPACR_CP10_FULL         (3U << 20)  /* FPU CP10 full access */
#define SCB_CPACR_CP11_FULL         (3U << 22)  /* FPU CP11 full access */

/* SysTick Control */
#define SYSTICK_CTRL_ENABLE         (1U << 0)
#define SYSTICK_CTRL_TICKINT        (1U << 1)
#define SYSTICK_CTRL_CLKSOURCE      (1U << 2)   /* 1 = CPU clock */
#define SYSTICK_CTRL_COUNTFLAG      (1U << 16)

/* ============================================================================
 * 
 *  LESSON 3: INLINE ASSEMBLY BASICS
 *  =================================
 * 
 *  C code is compiled to assembly. Sometimes we need direct CPU control!
 *  
 *  ARM Assembly Syntax:
 *  INSTRUCTION  DESTINATION, SOURCE1, SOURCE2
 *  
 *  Examples:
 *  MOV R0, #42      ; R0 = 42
 *  ADD R0, R1, R2   ; R0 = R1 + R2
 *  LDR R0, [R1]     ; R0 = *R1 (load from memory)
 *  STR R0, [R1]     ; *R1 = R0 (store to memory)
 *  
 *  In GCC inline assembly:
 *  __asm volatile ("INSTRUCTION" : outputs : inputs : clobbers);
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 1: READ CPU ID
 *  ============================
 * 
 *  Every ARM Cortex core has a unique ID that tells us:
 *  - Who made it (Implementer)
 *  - Which variant (Cortex-M7 = 0xC27)
 *  - Revision
 * 
 * ============================================================================ */

typedef struct {
    uint8_t revision;
    uint16_t partno;
    uint8_t variant;
    uint8_t implementer;
} CPU_ID_t;

void CPU_GetID(CPU_ID_t *id) {
    /* ✏️ YOUR TURN: Read CPUID register */
    uint32_t cpuid = ???;       /* HINT: Which SCB register has CPU identification? */
    
    id->implementer = (cpuid >> 24) & 0xFF;
    id->variant = (cpuid >> 20) & 0x0F;
    id->partno = (cpuid >> 4) & 0xFFF;
    id->revision = cpuid & 0x0F;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void CPU_GetID(CPU_ID_t *id) {
 *     uint32_t cpuid = SCB->CPUID;
 *     id->implementer = (cpuid >> 24) & 0xFF;  // 0x41 = ARM
 *     id->variant = (cpuid >> 20) & 0x0F;
 *     id->partno = (cpuid >> 4) & 0xFFF;       // 0xC27 = Cortex-M7
 *     id->revision = cpuid & 0x0F;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 2: ENABLE INSTRUCTION CACHE
 *  =========================================
 * 
 *  Cortex-M7 has 16KB instruction cache for faster code execution!
 * 
 * ============================================================================ */

void CPU_EnableICache(void) {
    /* ✏️ YOUR TURN: Enable instruction cache in CCR register */
    SCB->CCR |= ???;            /* HINT: Which bit enables I-cache? */
    
    __asm volatile ("dsb");     /* Data Synchronization Barrier */
    __asm volatile ("isb");     /* Instruction Synchronization Barrier */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void CPU_EnableICache(void) {
 *     SCB->CCR |= SCB_CCR_IC;
 *     __asm volatile ("dsb");
 *     __asm volatile ("isb");
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 3: ENABLE DATA CACHE
 *  ==================================
 * 
 *  Data cache speeds up memory reads/writes!
 * 
 * ============================================================================ */

void CPU_EnableDCache(void) {
    /* ✏️ YOUR TURN: Enable data cache */
    SCB->CCR |= ???;            /* HINT: Which bit enables D-cache? */
    
    __asm volatile ("dsb");
    __asm volatile ("isb");
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void CPU_EnableDCache(void) {
 *     SCB->CCR |= SCB_CCR_DC;
 *     __asm volatile ("dsb");
 *     __asm volatile ("isb");
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 4: ENABLE FPU (FLOATING POINT UNIT)
 *  =================================================
 * 
 *  Cortex-M7 has hardware FPU for fast floating point math!
 *  Without this, float/double operations will be SLOW software emulation.
 * 
 * ============================================================================ */

void CPU_EnableFPU(void) {
    /* ✏️ YOUR TURN: Enable full access to CP10 and CP11 coprocessors (FPU) */
    SCB->CPACR |= ???;          /* HINT: Which bits give full access to both FPU coprocessors? */
    
    __asm volatile ("dsb");
    __asm volatile ("isb");
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void CPU_EnableFPU(void) {
 *     SCB->CPACR |= SCB_CPACR_CP10_FULL | SCB_CPACR_CP11_FULL;
 *     __asm volatile ("dsb");
 *     __asm volatile ("isb");
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 4: STACK OPERATIONS
 *  ===========================
 * 
 *  The STACK is a memory region that grows DOWNWARD:
 *  
 *  High Memory
 *       │
 *       ▼
 *  ┌─────────┐  ◄── Stack Top (SP initially points here)
 *  │  Empty  │
 *  ├─────────┤
 *  │  Data   │  ◄── SP after PUSH (moves down)
 *  ├─────────┤
 *  │  Data   │
 *  ├─────────┤
 *  │  Data   │  ◄── Current SP
 *  └─────────┘
 *  Low Memory
 *  
 *  PUSH: Store value, SP decrements (moves down)
 *  POP:  Load value, SP increments (moves up)
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 5: READ STACK POINTER
 *  ===================================
 * 
 *  Read the current Main Stack Pointer (MSP) value.
 * 
 * ============================================================================ */

uint32_t CPU_GetMSP(void) {
    uint32_t result;
    
    /* ✏️ YOUR TURN: Read MSP register using inline assembly */
    __asm volatile ("MRS %0, MSP" : ???);   /* HINT: Output constraint that writes to result variable? */
    
    return result;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * uint32_t CPU_GetMSP(void) {
 *     uint32_t result;
 *     __asm volatile ("MRS %0, MSP" : "=r" (result));
 *     return result;
 * }
 * 
 * Explanation:
 * MRS = Move to Register from Special register
 * %0 = first output operand
 * "=r" = write to any general register
 * (result) = store in 'result' variable
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 6: DISABLE/ENABLE INTERRUPTS
 *  ==========================================
 * 
 *  Critical sections need interrupts disabled!
 * 
 * ============================================================================ */

void CPU_DisableIRQ(void) {
    /* ✏️ YOUR TURN: Execute CPSID i instruction (disable interrupts) */
    __asm volatile (???);       /* HINT: Which instruction disables interrupts? */
}

void CPU_EnableIRQ(void) {
    /* ✏️ YOUR TURN: Execute CPSIE i instruction (enable interrupts) */
    __asm volatile (???);       /* HINT: Which instruction enables interrupts? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void CPU_DisableIRQ(void) {
 *     __asm volatile ("CPSID i" : : : "memory");
 * }
 * 
 * void CPU_EnableIRQ(void) {
 *     __asm volatile ("CPSIE i" : : : "memory");
 * }
 * 
 * CPSID i = Change Processor State, Disable Interrupts
 * CPSIE i = Change Processor State, Enable Interrupts
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 5: SYSTICK TIMER
 *  ========================
 * 
 *  SysTick is a simple 24-bit timer built into every Cortex-M!
 *  Perfect for delays and OS tick generation.
 *  
 *  It counts DOWN from LOAD to 0, then reloads and repeats.
 * 
 * ============================================================================ */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 7: CONFIGURE SYSTICK FOR 1ms TICK
 *  ===============================================
 * 
 *  With 64 MHz CPU clock, for 1ms tick:
 *  LOAD = 64,000,000 / 1,000 - 1 = 63,999
 * 
 * ============================================================================ */

volatile uint32_t systick_counter = 0;

void SysTick_Handler(void) {
    systick_counter++;
}

void SysTick_Init(uint32_t ticks) {
    /* ✏️ YOUR TURN: Set reload value (24-bit max = 0xFFFFFF) */
    SysTick->LOAD = ???;        /* HINT: How do you set a countdown timer? (ticks minus what?) */
    
    /* Reset current value */
    SysTick->VAL = 0;
    
    /* ✏️ YOUR TURN: Enable SysTick with CPU clock and interrupt */
    SysTick->CTRL = ???;        /* HINT: Which bits enable timer + interrupt + CPU clock source? */
}
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void SysTick_Init(uint32_t ticks) {
 *     SysTick->LOAD = (ticks - 1) & 0xFFFFFF;
 *     SysTick->VAL = 0;
 *     SysTick->CTRL = SYSTICK_CTRL_ENABLE | 
 *                     SYSTICK_CTRL_TICKINT | 
 *                     SYSTICK_CTRL_CLKSOURCE;
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  ✏️  EXERCISE 8: DELAY USING SYSTICK
 *  ====================================
 * 
 * ============================================================================ */

void delay_ms(uint32_t ms) {
    uint32_t start = systick_counter;
    
    /* ✏️ YOUR TURN: Wait until systick_counter increases by 'ms' */
    while ((systick_counter - start) < ???);    /* HINT: What value to wait for? */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * void delay_ms(uint32_t ms) {
 *     uint32_t start = systick_counter;
 *     while ((systick_counter - start) < ms);
 * }
 * ───────────────────────────────────────────────────────────────────────────── */

/* ============================================================================
 * 
 *  LESSON 6: MEMORY BARRIERS
 *  ==========================
 * 
 *  Cortex-M7 can execute instructions OUT OF ORDER for speed!
 *  Memory barriers ensure ordering when needed.
 *  
 *  DMB = Data Memory Barrier (finish all memory accesses)
 *  DSB = Data Synchronization Barrier (wait for everything)
 *  ISB = Instruction Synchronization Barrier (flush pipeline)
 *  
 *  USE CASES:
 *  • After enabling cache/FPU
 *  • Before entering low power mode
 *  • When changing memory protection
 *  • Synchronizing with DMA/peripherals
 * 
 * ============================================================================ */

void CPU_DataMemoryBarrier(void) {
    __asm volatile ("dmb" : : : "memory");
}

void CPU_DataSyncBarrier(void) {
    __asm volatile ("dsb" : : : "memory");
}

void CPU_InstructionSyncBarrier(void) {
    __asm volatile ("isb" : : : "memory");
}

/* ============================================================================
 * 
 *  LESSON 7: SOFTWARE RESET
 *  =========================
 * 
 *  Request a system reset via AIRCR register.
 * 
 * ============================================================================ */

void CPU_SystemReset(void) {
    CPU_DataSyncBarrier();
    
    /* Request system reset with correct key */
    SCB->AIRCR = SCB_AIRCR_VECTKEYSTAT |        /* Key = 0x05FA */
                 SCB_AIRCR_SYSRESETREQ;         /* Request reset */
    
    CPU_DataSyncBarrier();
    
    /* Wait for reset */
    while (1);
}

/* ============================================================================
 * 
 *  LESSON 8: NOP (NO OPERATION)
 *  =============================
 * 
 *  NOP does nothing but waste 1 CPU cycle. Useful for precise delays.
 * 
 * ============================================================================ */

void CPU_Nop(void) {
    __asm volatile ("nop");
}

/* Very short delay (busy wait) */
void delay_cycles(uint32_t cycles) {
    for (uint32_t i = 0; i < cycles; i++) {
        CPU_Nop();
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
 *  MAIN PROGRAM - CPU Feature Demo
 * 
 * ============================================================================ */

int main(void)
{
    CPU_ID_t cpu_id;
    uint32_t stack_pointer;
    
    /* Initialize CPU features */
    CPU_EnableICache();
    CPU_EnableDCache();
    CPU_EnableFPU();
    
    /* Get CPU information */
    CPU_GetID(&cpu_id);
    /* Use debugger to inspect:
     * cpu_id.implementer = 0x41 (ARM)
     * cpu_id.partno = 0xC27 (Cortex-M7)
     */
    
    /* Get current stack pointer */
    stack_pointer = CPU_GetMSP();
    
    /* Initialize SysTick for 1ms ticks (assuming 64 MHz) */
    SysTick_Init(64000);
    
    /* Enable global interrupts */
    CPU_EnableIRQ();
    
    for(;;) {
        /* Use SysTick-based delay */
        delay_ms(1000);
        
        /* Use cycle-based delay */
        delay_cycles(1000000);
        
        /* 💡 Try adding LED toggle here to visualize timing */
        
        /* Uncomment to test system reset:
         * CPU_SystemReset();
         */
    }
}

/* ============================================================================
 * 
 *  🎉 CONGRATULATIONS!
 *  
 *  You've learned ARM Cortex-M7 Core Programming:
 *  
 *  ✅ CPU architecture and register overview
 *  ✅ Reading CPU ID register
 *  ✅ Enabling instruction and data cache
 *  ✅ Enabling hardware FPU
 *  ✅ Reading stack pointer
 *  ✅ Disabling/enabling interrupts
 *  ✅ SysTick timer configuration
 *  ✅ Memory barriers (DMB, DSB, ISB)
 *  ✅ System reset
 *  ✅ Inline assembly basics
 *  
 *  KEY CONCEPTS:
 *  • Cortex-M7 is the CPU core inside STM32H753
 *  • 16 main registers (R0-R15)
 *  • Hardware caches and FPU for performance
 *  • SysTick for simple timing
 *  • Memory barriers for synchronization
 *  
 *  ADVANCED TOPICS TO EXPLORE:
 *  • Memory Protection Unit (MPU)
 *  • Fault handlers and debugging
 *  • Branch prediction optimization
 *  • Tightly Coupled Memory (TCM)
 *  • ARM Thumb-2 instruction set details
 *  • Exception/interrupt vectoring
 * 
 * ============================================================================ */
