# 📖 STM32 Bare-Metal Glossary

> **A quick reference for all the acronyms, terms, and concepts used in the tutorials.**  
> Because embedded systems love their abbreviations! 🔤

---

## 📑 Table of Contents

- [🔤 Core Acronyms](#-core-acronyms)
- [🧠 CPU & Architecture](#-cpu--architecture)
- [⚡ Clocks & Power](#-clocks--power)
- [🚦 Interrupts & Events](#-interrupts--events)
- [📡 Communication Protocols](#-communication-protocols)
- [📊 Analog & Conversion](#-analog--conversion)
- [💾 Memory & Storage](#-memory--storage)
- [⏱️ Timers & Timing](#️-timers--timing)
- [🛡️ Safety & Protection](#️-safety--protection)
- [🔌 Hardware Terms](#-hardware-terms)
- [📝 Register & Bit Terms](#-register--bit-terms)

---

## 🔤 Core Acronyms

### **MCU** — Microcontroller Unit
| | |
|---|---|
| **What** | A small computer on a single chip (CPU + Memory + Peripherals) |
| **Intuition** | 🧠 "Mini Computer Unit" — it's a tiny computer that runs your code |
| **Role** | The brain of your embedded system |

### **GPIO** — General Purpose Input/Output
| | |
|---|---|
| **What** | Pins you can configure to read signals (input) or control devices (output) |
| **Intuition** | 🚪 "General Purpose I/O" — like a door that can go in OR out |
| **Role** | Connect to LEDs, buttons, sensors, and other hardware |

### **HAL** — Hardware Abstraction Layer
| | |
|---|---|
| **What** | ST's library that hides register details behind function calls |
| **Intuition** | 🎭 "Hides All Layers" — abstracts the hardware so you don't see it |
| **Role** | Makes code portable but hides what's really happening (we DON'T use this!) |

### **LL** — Low-Level (Library)
| | |
|---|---|
| **What** | ST's thinner library, closer to registers than HAL |
| **Intuition** | 📉 "Lower Level" than HAL, but still not bare metal |
| **Role** | Middle ground between HAL and bare metal |

---

## 🧠 CPU & Architecture

### **ARM** — Advanced RISC Machines
| | |
|---|---|
| **What** | Company that designs the CPU cores used in STM32 chips |
| **Intuition** | 💪 "ARM" designs the muscle (CPU) inside your chip |
| **Role** | Licenses CPU designs to chip makers like ST |

### **Cortex-M7** — ARM Cortex Microcontroller Profile 7
| | |
|---|---|
| **What** | The specific CPU core inside STM32H7 (32-bit, high performance) |
| **Intuition** | 🏎️ The "engine" inside your MCU — M7 is the fast sports car version |
| **Role** | Executes your code, handles interrupts, does math |

### **FPU** — Floating Point Unit
| | |
|---|---|
| **What** | Hardware that does decimal math (3.14159...) fast |
| **Intuition** | 🧮 "Fast decimal math coProcessor Unit" |
| **Role** | Accelerates calculations with decimal numbers |

### **ALU** — Arithmetic Logic Unit
| | |
|---|---|
| **What** | The part of CPU that does math and logic operations |
| **Intuition** | ➕ "Adds, Logic, compUtes" — the calculator inside the CPU |
| **Role** | Performs add, subtract, AND, OR, etc. |

### **PC** — Program Counter
| | |
|---|---|
| **What** | Register that holds the address of the next instruction |
| **Intuition** | 👆 "Points to Current instruction" — like a bookmark in your code |
| **Role** | Tells CPU where to fetch the next instruction |

### **SP** — Stack Pointer
| | |
|---|---|
| **What** | Register that points to the top of the stack |
| **Intuition** | 📚 "Stack Position" — points to top of the pile |
| **Role** | Manages function calls, local variables, context saving |

### **LR** — Link Register
| | |
|---|---|
| **What** | Register that stores the return address when calling a function |
| **Intuition** | 🔗 "Link back Register" — remembers where to return |
| **Role** | Enables returning from function calls |

### **PSR** — Program Status Register
| | |
|---|---|
| **What** | Register containing CPU flags (Zero, Negative, Carry, oVerflow) |
| **Intuition** | 🚥 "Program Status Report" — flags about the last operation |
| **Role** | Used for conditional branching (if/else) |

---

## ⚡ Clocks & Power

### **RCC** — Reset and Clock Control
| | |
|---|---|
| **What** | Peripheral that manages all clocks and reset signals |
| **Intuition** | 🔌 "Really Critical Component" — nothing works without it! |
| **Role** | Enables peripheral clocks (THE #1 RULE!), configures clock sources |

### **HSI** — High-Speed Internal (oscillator)
| | |
|---|---|
| **What** | Internal 64 MHz clock source (RC oscillator) |
| **Intuition** | 🏠 "High Speed Inside" — built-in, no external parts needed |
| **Role** | Default clock at startup, less accurate than HSE |

### **HSE** — High-Speed External (oscillator)
| | |
|---|---|
| **What** | External crystal oscillator (8 MHz on Nucleo) |
| **Intuition** | 🌐 "High Speed External crystal" — more accurate |
| **Role** | Precise clock source for high-accuracy timing |

### **LSI** — Low-Speed Internal (oscillator)
| | |
|---|---|
| **What** | Internal ~32 kHz clock (RC oscillator) |
| **Intuition** | 🐌 "Low Speed Inside" — slow but always available |
| **Role** | Powers watchdog and RTC when no LSE crystal |

### **LSE** — Low-Speed External (oscillator)
| | |
|---|---|
| **What** | External 32.768 kHz crystal |
| **Intuition** | ⏰ "Low Speed External" — 32768 = 2^15, perfect for timing |
| **Role** | Accurate RTC clock (divides evenly into 1 second) |

### **PLL** — Phase-Locked Loop
| | |
|---|---|
| **What** | Circuit that multiplies clock frequency (8 MHz → 480 MHz) |
| **Intuition** | 🚀 "Pumps up the frequency" — a clock multiplier |
| **Role** | Generates high-speed system clock from slower source |

### **SYSCLK** — System Clock
| | |
|---|---|
| **What** | Main clock that drives the CPU and buses |
| **Intuition** | ❤️ "The heartbeat" of the system |
| **Role** | Determines how fast the CPU runs |

### **AHB** — Advanced High-performance Bus
| | |
|---|---|
| **What** | High-speed bus connecting CPU to fast peripherals |
| **Intuition** | 🛣️ "A Highway for fast Bits" — the fast lane |
| **Role** | Connects to GPIO, DMA, memory controllers |

### **APB** — Advanced Peripheral Bus
| | |
|---|---|
| **What** | Lower-speed bus for peripherals |
| **Intuition** | 🛤️ "A Peripheral road for Bits" — slower but sufficient |
| **Role** | Connects to UART, SPI, I2C, timers |

---

## 🚦 Interrupts & Events

### **IRQ** — Interrupt Request
| | |
|---|---|
| **What** | A signal from hardware asking CPU to stop and handle something |
| **Intuition** | 🖐️ "Interrupt! Request attention now!" — like someone tapping your shoulder |
| **Role** | Each peripheral has an IRQ number for NVIC |

### **NVIC** — Nested Vectored Interrupt Controller
| | |
|---|---|
| **What** | Hardware that manages all interrupts in Cortex-M |
| **Intuition** | 👮 "Nice organizer of Vectored Interrupt Control" — the interrupt traffic cop |
| **Role** | Enables/disables interrupts, sets priorities, handles nesting |

### **ISR** — Interrupt Service Routine
| | |
|---|---|
| **What** | The function that runs when an interrupt occurs |
| **Intuition** | 🏃 "Immediate Service Routine" — the code that handles the interrupt |
| **Role** | Contains the code to respond to the interrupt |

### **EXTI** — External Interrupt/Event Controller
| | |
|---|---|
| **What** | Peripheral that detects external events on GPIO pins |
| **Intuition** | 🔔 "External Trigger Interface" — watches for pin changes |
| **Role** | Triggers interrupts on rising/falling edges of GPIO |

### **Vector Table**
| | |
|---|---|
| **What** | Table of function pointers for all interrupt handlers |
| **Intuition** | 📋 Like a phone directory — "for interrupt X, call function Y" |
| **Role** | CPU looks up which function to call for each interrupt |

### **Pending**
| | |
|---|---|
| **What** | State when an interrupt is waiting to be serviced |
| **Intuition** | ⏳ "In the queue" — interrupt raised but not yet handled |
| **Role** | Interrupt will be serviced when priority allows |

### **Preemption**
| | |
|---|---|
| **What** | Higher priority interrupt interrupting a lower priority one |
| **Intuition** | 🚨 "Pre-empt = take over" — VIP cutting in line |
| **Role** | Ensures urgent interrupts are handled first |

---

## 📡 Communication Protocols

### **UART** — Universal Asynchronous Receiver/Transmitter
| | |
|---|---|
| **What** | Serial communication without a clock signal |
| **Intuition** | 📞 "You speak, I listen, one bit at a time, no sync needed" |
| **Role** | Debug output, GPS, Bluetooth modules |

### **USART** — Universal Synchronous/Asynchronous Receiver/Transmitter
| | |
|---|---|
| **What** | UART + optional synchronous mode with clock |
| **Intuition** | 📞➕ "UART with optional Sync mode added" |
| **Role** | Same as UART, but can also do synchronous communication |

### **TX** — Transmit
| | |
|---|---|
| **What** | Pin that sends data OUT |
| **Intuition** | 📤 "Transmit = send out" |
| **Role** | Connects to other device's RX |

### **RX** — Receive
| | |
|---|---|
| **What** | Pin that receives data IN |
| **Intuition** | 📥 "Receive = take in" |
| **Role** | Connects to other device's TX |

### **Baud Rate**
| | |
|---|---|
| **What** | Speed of serial communication (bits per second) |
| **Intuition** | 🏎️ "How fast we talk" — 115200 = 115,200 bits/second |
| **Role** | Both sides must use same baud rate! |

### **SPI** — Serial Peripheral Interface
| | |
|---|---|
| **What** | Fast synchronous serial bus with clock, 4 wires |
| **Intuition** | 🚄 "Super fast Peripheral Interface" — like a 4-lane highway |
| **Role** | SD cards, displays, flash memory, sensors |

### **MOSI** — Master Out, Slave In
| | |
|---|---|
| **What** | SPI data line from master to slave |
| **Intuition** | ➡️ "Master sends Out, Slave takes In" |
| **Role** | Commands and data TO the slave device |

### **MISO** — Master In, Slave Out
| | |
|---|---|
| **What** | SPI data line from slave to master |
| **Intuition** | ⬅️ "Master takes In, Slave sends Out" |
| **Role** | Responses and data FROM the slave device |

### **SCK / SCLK** — Serial Clock
| | |
|---|---|
| **What** | Clock signal generated by SPI master |
| **Intuition** | ⏰ "Serial ClocK" — the timing reference |
| **Role** | Synchronizes data transfer between master and slave |

### **CS / SS** — Chip Select / Slave Select
| | |
|---|---|
| **What** | Pin that selects which slave to talk to |
| **Intuition** | 👆 "Chip Select = tap on shoulder to get attention" |
| **Role** | Active low — pull LOW to select a device |

### **CPOL** — Clock Polarity
| | |
|---|---|
| **What** | Idle state of clock (high or low) |
| **Intuition** | "Clock POLarity = is clock HIGH or LOW when idle?" |
| **Role** | Must match slave device configuration |

### **CPHA** — Clock Phase
| | |
|---|---|
| **What** | Which clock edge samples data (first or second) |
| **Intuition** | "Clock PHAse = WHEN do we read the data?" |
| **Role** | Must match slave device configuration |

### **I2C / I²C** — Inter-Integrated Circuit
| | |
|---|---|
| **What** | Two-wire serial bus with addressing (SDA + SCL) |
| **Intuition** | 🔗 "I-squared-C = chips talking on 2 wires" |
| **Role** | Sensors, EEPROMs, RTCs — many devices, few wires |

### **SDA** — Serial Data
| | |
|---|---|
| **What** | I2C bidirectional data line |
| **Intuition** | 📝 "Serial DAta line" — carries the actual data |
| **Role** | Open-drain, needs pull-up resistor |

### **SCL** — Serial Clock
| | |
|---|---|
| **What** | I2C clock line generated by master |
| **Intuition** | ⏰ "Serial CLock line" |
| **Role** | Open-drain, needs pull-up resistor |

### **ACK / NACK** — Acknowledge / Not Acknowledge
| | |
|---|---|
| **What** | Response bit in I2C indicating success or failure |
| **Intuition** | ✅❌ "Got it" vs "Didn't get it" |
| **Role** | ACK = continue, NACK = stop or error |

### **MAC** — Media Access Controller
| | |
|---|---|
| **What** | Hardware that handles Ethernet frame transmission/reception |
| **Intuition** | 📬 "Manages Access to the Cable" — handles addressing and framing |
| **Role** | Inside the MCU, generates/checks CRC, filters addresses |

### **PHY** — Physical Layer
| | |
|---|---|
| **What** | Chip that converts digital signals to actual wire signals |
| **Intuition** | 🔌 "PHYsical connection" — interfaces with the cable |
| **Role** | External chip (LAN8742A on Nucleo), handles electrical signaling |

### **RMII** — Reduced Media Independent Interface
| | |
|---|---|
| **What** | 7-signal interface between MAC and PHY |
| **Intuition** | 📉 "Reduced MII" — fewer wires than MII (7 vs 16) |
| **Role** | Connects STM32 to Ethernet PHY |

### **MDIO** — Management Data Input/Output
| | |
|---|---|
| **What** | Interface to configure PHY registers |
| **Intuition** | ⚙️ "Management Data I/O" — like I2C for the PHY |
| **Role** | Configure PHY speed, duplex, read status |

---

## 📊 Analog & Conversion

### **ADC** — Analog-to-Digital Converter
| | |
|---|---|
| **What** | Converts analog voltage (0-3.3V) to digital number |
| **Intuition** | 📏 "Analog → Digital" — measures voltage as a number |
| **Role** | Read sensors, potentiometers, voltage levels |

### **DAC** — Digital-to-Analog Converter
| | |
|---|---|
| **What** | Converts digital number to analog voltage |
| **Intuition** | 🔊 "Digital → Analog" — outputs a voltage from a number |
| **Role** | Audio output, waveform generation, analog control |

### **Resolution**
| | |
|---|---|
| **What** | Number of bits in ADC/DAC conversion |
| **Intuition** | 📐 "How precisely can we measure?" — more bits = finer steps |
| **Role** | 12-bit = 4096 steps, 16-bit = 65536 steps |

### **Vref** — Voltage Reference
| | |
|---|---|
| **What** | Reference voltage for ADC/DAC (usually 3.3V) |
| **Intuition** | 📏 "The ruler" — what voltage = max reading? |
| **Role** | Determines the scale of conversion |

---

## 💾 Memory & Storage

### **Flash**
| | |
|---|---|
| **What** | Non-volatile memory that stores your program |
| **Intuition** | 💿 "Permanent storage" — survives power off |
| **Role** | Stores code, can also store user data |

### **SRAM** — Static Random Access Memory
| | |
|---|---|
| **What** | Fast volatile memory for variables and stack |
| **Intuition** | 📋 "Scratch pad" — lost when power off |
| **Role** | Runtime storage for your program's data |

### **DMA** — Direct Memory Access
| | |
|---|---|
| **What** | Hardware that copies data without CPU |
| **Intuition** | 🤖 "Data Moving Assistant" — copies while CPU does other things |
| **Role** | Fast transfers between memory and peripherals |

### **DMAMUX** — DMA Request Multiplexer
| | |
|---|---|
| **What** | Routes peripheral requests to DMA channels |
| **Intuition** | 🔀 "DMA switchboard" — connects peripherals to DMA |
| **Role** | Configures which peripheral triggers which DMA channel |

### **Sector**
| | |
|---|---|
| **What** | Smallest erasable unit of Flash memory |
| **Intuition** | 📦 "A box of Flash" — must erase whole box at once |
| **Role** | STM32H7 has 128KB sectors |

---

## ⏱️ Timers & Timing

### **TIM** — Timer
| | |
|---|---|
| **What** | Hardware counter that counts clock pulses |
| **Intuition** | ⏱️ "TIMer" — counts ticks at a known rate |
| **Role** | Delays, PWM, input capture, event counting |

### **PWM** — Pulse Width Modulation
| | |
|---|---|
| **What** | Square wave with variable ON time (duty cycle) |
| **Intuition** | 💡 "Percentage of ON time" — 50% duty = half brightness |
| **Role** | LED dimming, motor speed, servo control |

### **PSC** — Prescaler
| | |
|---|---|
| **What** | Divides clock before it reaches counter |
| **Intuition** | ➗ "Pre-divider" — slows down the counting |
| **Role** | Timer Freq = Clock / (PSC + 1) |

### **ARR** — Auto-Reload Register
| | |
|---|---|
| **What** | Value at which timer resets to zero |
| **Intuition** | 🔄 "Restart At this value" — the countdown limit |
| **Role** | Determines timer overflow period |

### **CNT** — Counter
| | |
|---|---|
| **What** | The actual counting register |
| **Intuition** | 🔢 "The CouNTer value" — 0, 1, 2, 3... |
| **Role** | Current count value |

### **CCR** — Capture/Compare Register
| | |
|---|---|
| **What** | Value to compare against counter |
| **Intuition** | 🎯 "Compare value" — when CNT = CCR, event! |
| **Role** | Sets PWM duty cycle or capture events |

### **SysTick** — System Tick Timer
| | |
|---|---|
| **What** | 24-bit timer built into Cortex-M core |
| **Intuition** | ⏰ "System heartbeat" — simple periodic interrupt |
| **Role** | OS ticks, simple delays |

### **RTC** — Real-Time Clock
| | |
|---|---|
| **What** | Clock that keeps time even when MCU sleeps |
| **Intuition** | 📅 "The wall clock" — hours, minutes, seconds, date |
| **Role** | Timestamping, alarms, calendar functions |

---

## 🛡️ Safety & Protection

### **IWDG** — Independent Watchdog
| | |
|---|---|
| **What** | Simple timeout watchdog with its own clock |
| **Intuition** | 🐕 "Guard dog" — reset system if not fed in time |
| **Role** | Recovers from crashes; uses LSI, works even if main clock fails |

### **WWDG** — Window Watchdog
| | |
|---|---|
| **What** | Watchdog with refresh window (not too early, not too late) |
| **Intuition** | 🪟 "Window guard dog" — must feed at the right time |
| **Role** | Detects stuck loops; uses APB clock |

### **MPU** — Memory Protection Unit
| | |
|---|---|
| **What** | Hardware that restricts memory access per region |
| **Intuition** | 🚧 "Memory Police Unit" — prevents bad memory access |
| **Role** | Protects critical memory regions, catches bugs |

---

## 🔌 Hardware Terms

### **Pull-up Resistor**
| | |
|---|---|
| **What** | Resistor connecting pin to VDD (3.3V) |
| **Intuition** | ⬆️ "Pulls the voltage UP when nothing else drives it" |
| **Role** | Default HIGH state, prevents floating |

### **Pull-down Resistor**
| | |
|---|---|
| **What** | Resistor connecting pin to GND |
| **Intuition** | ⬇️ "Pulls the voltage DOWN when nothing else drives it" |
| **Role** | Default LOW state, prevents floating |

### **Open-Drain**
| | |
|---|---|
| **What** | Output that can only pull LOW, needs external pull-up |
| **Intuition** | 🚰 "Drain can only sink current" — needs resistor for HIGH |
| **Role** | Used by I2C, allows multiple devices on same wire |

### **Push-Pull**
| | |
|---|---|
| **What** | Output that can drive both HIGH and LOW |
| **Intuition** | 💪 "Can push AND pull" — actively drives both states |
| **Role** | Default GPIO output mode |

### **Alternate Function (AF)**
| | |
|---|---|
| **What** | Pin mode that connects GPIO to a peripheral |
| **Intuition** | 🔀 "Alternative job for the pin" — not GPIO, but UART/SPI/etc. |
| **Role** | Route peripheral signals to physical pins |

### **Rising Edge**
| | |
|---|---|
| **What** | Signal transition from LOW to HIGH |
| **Intuition** | 📈 "Going UP" — 0 → 1 transition |
| **Role** | Trigger events on this transition |

### **Falling Edge**
| | |
|---|---|
| **What** | Signal transition from HIGH to LOW |
| **Intuition** | 📉 "Going DOWN" — 1 → 0 transition |
| **Role** | Trigger events on this transition |

---

## 📝 Register & Bit Terms

### **Register**
| | |
|---|---|
| **What** | Memory location that controls hardware |
| **Intuition** | 🎛️ "Control knobs" — write to configure, read to check status |
| **Role** | Every peripheral is controlled through registers |

### **Bit Field**
| | |
|---|---|
| **What** | Group of bits within a register with specific meaning |
| **Intuition** | 📝 "A section of the register" — bits 4-5 might mean mode |
| **Role** | Different fields control different features |

### **Memory-Mapped I/O**
| | |
|---|---|
| **What** | Accessing hardware registers like memory addresses |
| **Intuition** | 🗺️ "Peripherals have addresses" — write to address = configure |
| **Role** | STM32 peripherals are at specific addresses |

### **Volatile**
| | |
|---|---|
| **What** | C keyword telling compiler the value can change unexpectedly |
| **Intuition** | ⚠️ "Don't optimize this!" — hardware might change it |
| **Role** | MUST use for all hardware registers |

### **Bit Mask**
| | |
|---|---|
| **What** | Value used to select specific bits |
| **Intuition** | 😷 "Mask = hide/show specific bits" |
| **Role** | `0x0F` masks lower 4 bits |

### **Set (bit)**
| | |
|---|---|
| **What** | Make a bit = 1 |
| **Intuition** | ✅ "Turn ON" using OR: `REG |= (1 << bit)` |
| **Role** | Enable features |

### **Clear (bit)**
| | |
|---|---|
| **What** | Make a bit = 0 |
| **Intuition** | ❌ "Turn OFF" using AND NOT: `REG &= ~(1 << bit)` |
| **Role** | Disable features |

### **Toggle (bit)**
| | |
|---|---|
| **What** | Flip bit from 0→1 or 1→0 |
| **Intuition** | 🔄 "Flip it" using XOR: `REG ^= (1 << bit)` |
| **Role** | Change state |

---

## 🎯 Quick Reference Card

| Acronym | Full Name | One-Line Summary |
|---------|-----------|------------------|
| GPIO | General Purpose I/O | Read/write digital pins |
| RCC | Reset and Clock Control | Enable clocks (rule #1!) |
| NVIC | Nested Vectored Interrupt Controller | Manage interrupts |
| EXTI | External Interrupt Controller | GPIO interrupt triggers |
| UART | Universal Async Receiver/Transmitter | Serial debug/communication |
| SPI | Serial Peripheral Interface | Fast 4-wire protocol |
| I2C | Inter-Integrated Circuit | 2-wire addressed protocol |
| DMA | Direct Memory Access | Copy data without CPU |
| ADC | Analog-to-Digital Converter | Read analog voltages |
| DAC | Digital-to-Analog Converter | Output analog voltages |
| TIM | Timer | Count, delay, PWM |
| RTC | Real-Time Clock | Keep time (calendar) |
| IWDG | Independent Watchdog | Reset on crash (simple) |
| WWDG | Window Watchdog | Reset on crash (window) |
| IRQ | Interrupt Request | "Hey CPU, handle this!" |
| ISR | Interrupt Service Routine | Handler function |

---

<p align="center">
  <b>📖 Keep this open while learning — you'll need it! 📖</b>
</p>

<p align="center">
  Made with ❤️ by <b>Tommyl404</b>
</p>
