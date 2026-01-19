# STM32 Embedded Systems Interview Questions

## 150 Interview Questions with Answers

---

# TIER 1: High Priority Questions (Most Likely to Be Asked)

These topics have the highest ROI for interview preparation.

---

## RCC / Clocks / Clock Tree Basics (Questions 1-10)

### Q1: What does RCC stand for and what is its purpose?
**Answer:** RCC stands for Reset and Clock Control. It manages all clock sources (HSI, HSE, PLL, LSI, LSE), distributes clocks to peripherals via bus prescalers, and handles system reset sources. Without enabling a peripheral's clock, the peripheral won't function.

---

### Q2: What are the main clock sources available on STM32?
**Answer:**
- **HSI** (High-Speed Internal): Internal RC oscillator (~64MHz on H7), less accurate but no external components
- **HSE** (High-Speed External): External crystal (8-25MHz typical), more accurate
- **LSI** (Low-Speed Internal): ~32kHz for RTC/watchdog
- **LSE** (Low-Speed External): 32.768kHz crystal for accurate RTC
- **PLL**: Multiplies HSI/HSE to achieve higher frequencies (up to 480MHz on H7)

---

### Q3: Why must you enable a peripheral clock before using it?
**Answer:** STM32 peripherals are clock-gated by default to save power. The peripheral registers are not accessible and the peripheral logic doesn't function until its clock is enabled in the RCC enable register. Accessing a disabled peripheral can cause a bus fault.

---

### Q4: What is the AHB and APB bus, and how do they differ?
**Answer:**
- **AHB** (Advanced High-performance Bus): High-speed bus for CPU, memory, DMA. Runs at or near system clock speed.
- **APB** (Advanced Peripheral Bus): Lower-speed bus for peripherals (UART, SPI, I2C, timers). Has prescalers to divide the clock.
- APB is slower but adequate for most peripherals; AHB is used where speed matters.

---

### Q5: What happens if you don't wait after enabling a peripheral clock?
**Answer:** There's a hardware delay (typically 2 clock cycles) between writing to the clock enable register and the clock actually reaching the peripheral. If you immediately access the peripheral, you may read garbage or cause undefined behavior. Best practice: read back the enable register to ensure synchronization.

---

### Q6: How do you switch the system clock to use PLL?
**Answer:**
1. Enable HSE or HSI as PLL source
2. Configure PLL multipliers/dividers (PLLM, PLLN, PLLP/Q/R)
3. Enable PLL and wait for PLL ready flag (PLLRDY)
4. Configure Flash wait states for new frequency
5. Switch system clock source to PLL via RCC_CFGR
6. Wait for switch confirmation (SWS bits)

---

### Q7: Why do Flash wait states matter when changing clock speed?
**Answer:** Flash memory has a maximum access speed (~30ns typical). At higher CPU frequencies, the CPU is faster than Flash, so wait states (extra clock cycles) are inserted. Too few wait states = data corruption. Too many = wasted performance. The reference manual provides a table mapping voltage and frequency to required wait states.

---

### Q8: What is the purpose of the LSE oscillator?
**Answer:** The LSE (Low-Speed External) uses a 32.768kHz crystal specifically chosen because it divides evenly into 1 second (2^15 = 32768). It provides an accurate time base for the RTC (Real-Time Clock) and can run in low-power modes when the main oscillators are off.

---

### Q9: How can you verify which clock source is currently active?
**Answer:** Read the SWS (System clock Switch Status) bits in RCC_CFGR register. These indicate the actual clock source (00=HSI, 01=CSI, 10=HSE, 11=PLL). You should always verify the switch completed rather than assuming it happened instantly.

---

### Q10: What is clock security system (CSS)?
**Answer:** CSS monitors the HSE oscillator. If HSE fails (crystal breaks, connection issue), CSS automatically switches to HSI and generates an NMI interrupt. This prevents the system from crashing due to clock loss and allows graceful degradation.

---

## GPIO + Alternate Function + Pull/Speed/Output Type (Questions 11-20)

### Q11: What are the four GPIO modes available on STM32?
**Answer:**
- **Input (00)**: Read external signals
- **Output (01)**: Drive pins high/low
- **Alternate Function (10)**: Connect pin to peripheral (UART, SPI, etc.)
- **Analog (11)**: For ADC/DAC, disables digital input buffer

---

### Q12: What is the difference between push-pull and open-drain output?
**Answer:**
- **Push-pull**: Can actively drive high (to VDD) and low (to GND). Standard output type.
- **Open-drain**: Can only pull low; high state is achieved by external pull-up resistor. Used for I2C, wired-OR logic, and level shifting.

---

### Q13: What are pull-up and pull-down resistors used for?
**Answer:** They define a default state for floating inputs. Pull-up connects to VDD (default HIGH), pull-down connects to GND (default LOW). Used on buttons (to prevent floating input when released) and I2C lines (pull-up required for open-drain).

---

### Q14: Why does GPIO speed setting matter?
**Answer:** GPIO speed controls the slew rate (how fast the output transitions). Higher speed = faster edges but more EMI/noise and higher power consumption. Lower speed = slower edges, less noise, lower power. Use only the speed you need (low for LEDs, high for high-speed SPI).

---

### Q15: What is an alternate function and how do you configure it?
**Answer:** Alternate functions connect GPIO pins to internal peripherals (UART TX/RX, SPI MOSI/MISO, etc.). Configuration:
1. Set MODER to Alternate Function mode (10)
2. Set AFR[0] or AFR[1] register with the AF number (0-15)
Each pin has a fixed mapping of which AF number connects to which peripheral.

---

### Q16: How do you find which AF number to use for a specific peripheral?
**Answer:** Check the datasheet's "Alternate Function Mapping" table. It shows each pin and which AF0-AF15 connects to which peripheral. For example, PA9 with AF7 = USART1_TX. The reference manual also has this information.

---

### Q17: What is the difference between ODR and BSRR for setting GPIO outputs?
**Answer:**
- **ODR** (Output Data Register): Read-modify-write operation, not atomic, can be interrupted
- **BSRR** (Bit Set/Reset Register): Write-only, atomic operation. Writing 1 to lower 16 bits sets, writing 1 to upper 16 bits resets. Preferred for thread-safe GPIO control.

---

### Q18: Why would you configure a GPIO pin as analog mode when not using ADC?
**Answer:** Analog mode disables the digital input buffer, reducing power consumption and preventing noise injection into the digital logic. It's the lowest power state for unused pins. Also required when using ADC/DAC on that pin.

---

### Q19: What happens if you forget to enable the GPIO clock before configuring pins?
**Answer:** The GPIO register writes will have no effect (the peripheral is powered off). Reads may return 0 or garbage. On some STM32 families, accessing a disabled peripheral can cause a bus fault. Always enable the clock first!

---

### Q20: How do you read the current state of an input pin?
**Answer:** Read the IDR (Input Data Register) and mask the specific bit: `if (GPIOB->IDR & (1 << pin_number))`. IDR reflects the actual voltage level on the pin, regardless of whether it's configured as input or output.

---

## Interrupts: NVIC + EXTI + ISR Rules (Questions 21-30)

### Q21: What does NVIC stand for and what does it do?
**Answer:** NVIC = Nested Vectored Interrupt Controller. It's part of the ARM Cortex-M core (not STM32-specific). It manages interrupt enabling, priority levels, pending status, and automatically handles context saving/restoring when entering/exiting ISRs.

---

### Q22: What does "nested" mean in NVIC?
**Answer:** Higher-priority interrupts can preempt (interrupt) lower-priority interrupt handlers. When the higher-priority ISR completes, execution returns to the lower-priority ISR. This allows time-critical events to be handled immediately.

---

### Q23: What does "vectored" mean in NVIC?
**Answer:** Each interrupt has a fixed entry in the vector table (an array of function pointers in memory). When an interrupt occurs, hardware directly jumps to the correct handler without software lookup. This makes interrupt entry very fast.

---

### Q24: How does interrupt priority work on STM32?
**Answer:** Lower number = higher priority (0 is highest). STM32H7 uses 4 priority bits (0-15). The priority register is 8 bits but only upper 4 bits are used, so you shift left by 4. Priority 0 can preempt priority 1, which can preempt priority 2, etc.

---

### Q25: What is EXTI and how is it different from NVIC?
**Answer:**
- **EXTI** (External Interrupt/Event Controller): STM32 peripheral that detects edges on GPIO pins and generates interrupt requests
- **NVIC**: ARM core component that receives and prioritizes interrupt requests
EXTI generates the request, NVIC handles it.

---

### Q26: How do you configure an external interrupt on a GPIO pin?
**Answer:**
1. Enable GPIO and SYSCFG clocks
2. Configure pin as input
3. Use SYSCFG->EXTICR to select which port drives the EXTI line
4. Configure edge detection (RTSR/FTSR for rising/falling)
5. Unmask the interrupt (EXTI->IMR)
6. Enable the interrupt in NVIC
7. Write the ISR handler function

---

### Q27: Why do multiple EXTI lines share one IRQ handler?
**Answer:** To reduce the number of interrupt vectors, ARM/ST groups EXTI lines: EXTI0-4 have individual handlers, but EXTI5-9 and EXTI10-15 share handlers. Inside the shared handler, you must check EXTI->PR (pending register) to determine which line(s) triggered.

---

### Q28: What is the most important thing to do in an EXTI ISR?
**Answer:** Clear the pending flag! Write 1 to EXTI->PR for the triggered line(s). If you don't clear it, the interrupt will fire again immediately after exiting, creating an infinite loop that hangs the system.

---

### Q29: What are best practices for writing ISR code?
**Answer:**
- Keep it SHORT (do minimal work)
- Clear the interrupt flag ASAP
- Use volatile for shared variables
- Don't call blocking functions (delays, printf, malloc)
- Set a flag and process in main loop if complex work needed
- Avoid accessing slow peripherals

---

### Q30: How do you disable all interrupts temporarily?
**Answer:** Use `__disable_irq()` (assembly: CPSID I) to set PRIMASK, blocking all interrupts except NMI and HardFault. Use `__enable_irq()` (CPSIE I) to re-enable. Keep this critical section as short as possible to avoid missing events.

---

## UART/USART: Polling vs Interrupt, Baud Rate (Questions 31-40)

### Q31: What is the difference between UART and USART?
**Answer:**
- **UART**: Universal Asynchronous Receiver/Transmitter - async only
- **USART**: Universal Synchronous/Asynchronous - can also do synchronous with clock line
In practice, USART is usually used in async mode, so they're often interchangeable terms.

---

### Q32: What is baud rate and how is it calculated?
**Answer:** Baud rate = symbols (bits) per second. For UART: `Baud = fPCLK / (16 * USARTDIV)` where USARTDIV is written to BRR register. For 115200 baud with 64MHz clock: `BRR = 64000000 / 115200 = 555`. Both sides must use the same baud rate.

---

### Q33: What does 8N1 mean in UART configuration?
**Answer:**
- **8**: 8 data bits per frame
- **N**: No parity bit
- **1**: 1 stop bit
This is the most common UART configuration. Other options: 7 data bits, even/odd parity, 2 stop bits.

---

### Q34: What is the difference between polling and interrupt-driven UART?
**Answer:**
- **Polling**: CPU continuously checks status flags (TXE, RXNE) in a loop. Simple but wastes CPU cycles.
- **Interrupt**: CPU does other work; hardware triggers ISR when data ready. Efficient but more complex code.
Use polling for simple/debug, interrupts for production.

---

### Q35: What are the key UART status flags?
**Answer:**
- **TXE** (TX Empty): Transmit buffer ready for new data
- **RXNE** (RX Not Empty): Received data available
- **TC** (Transmission Complete): All bits shifted out
- **ORE** (Overrun Error): New data arrived before old was read
- **FE** (Framing Error): Stop bit not detected
- **PE** (Parity Error): Parity mismatch

---

### Q36: What causes an overrun error (ORE)?
**Answer:** A new byte arrived while the previous byte was still in the receive buffer (RXNE=1, not read yet). The old data is lost. Prevent by: reading data fast enough, using DMA, using higher priority interrupt, or implementing flow control.

---

### Q37: How do you transmit a byte using polling?
**Answer:**
```c
while (!(USART1->ISR & USART_ISR_TXE));  // Wait for TX buffer empty
USART1->TDR = data;                       // Write data to transmit register
```

---

### Q38: How do you receive a byte using polling?
**Answer:**
```c
while (!(USART1->ISR & USART_ISR_RXNE));  // Wait for data available
data = USART1->RDR;                        // Read received data
```

---

### Q39: What is the purpose of the UART TX and RX enable bits?
**Answer:** TE (Transmitter Enable) and RE (Receiver Enable) in CR1 independently enable transmit/receive functionality. You must enable UE (USART Enable) first, then TE/RE. This allows half-duplex operation or disabling unused functions to save power.

---

### Q40: What GPIO mode is required for UART pins?
**Answer:** Alternate Function mode with the correct AF number (check datasheet). For example, USART3 on STM32H7: PD8=AF7 for TX, PD9=AF7 for RX. TX should be push-pull; RX is input so output type doesn't matter.

---

## DMA: What/Why/How, Basic Flow (Questions 41-50)

### Q41: What does DMA stand for and what is its purpose?
**Answer:** DMA = Direct Memory Access. It transfers data between memory and peripherals (or memory-to-memory) without CPU involvement. The CPU sets up the transfer, DMA executes it, and CPU is free to do other work. Essential for high-throughput applications.

---

### Q42: What are the main benefits of using DMA?
**Answer:**
- CPU is free during transfers (can do calculations, sleep)
- Higher throughput (no CPU overhead per byte)
- Deterministic timing (no interrupt latency per byte)
- Lower power (CPU can sleep during transfers)
- Required for very high-speed peripherals (ADC, high-speed SPI)

---

### Q43: What is a DMA stream and channel?
**Answer:**
- **Stream**: A DMA hardware unit that can perform one transfer at a time (STM32H7 has multiple streams)
- **Channel**: Selects which peripheral request triggers this stream
Each peripheral has specific channel/stream assignments (check reference manual).

---

### Q44: What are the three main DMA transfer directions?
**Answer:**
- **Peripheral-to-Memory**: ADC→buffer, UART RX→buffer
- **Memory-to-Peripheral**: buffer→UART TX, buffer→DAC
- **Memory-to-Memory**: Copy between RAM locations (no peripheral trigger)

---

### Q45: What is circular mode in DMA?
**Answer:** When transfer completes, DMA automatically restarts from the beginning of the buffer. Useful for continuous ADC sampling, audio streaming, or ping-pong buffers. Without circular mode, you must manually restart or reconfigure the DMA.

---

### Q46: What information must you configure for a DMA transfer?
**Answer:**
- Source address (peripheral or memory)
- Destination address
- Number of data items to transfer
- Data size (byte, half-word, word)
- Increment mode (fixed address or increment)
- Direction (P→M, M→P, M→M)
- Priority level
- Circular mode (yes/no)

---

### Q47: What triggers a DMA transfer?
**Answer:** Either:
- **Peripheral request**: Peripheral (UART, ADC, SPI) signals when it's ready
- **Software trigger**: For memory-to-memory transfers
The peripheral must have DMA enabled (e.g., USART->CR3 DMAT/DMAR bits).

---

### Q48: What DMA interrupts are commonly used?
**Answer:**
- **TC** (Transfer Complete): All items transferred
- **HT** (Half Transfer): Half of items done (useful for ping-pong)
- **TE** (Transfer Error): Bus error or configuration error
Always check for errors!

---

### Q49: What is a common DMA use case with UART?
**Answer:** Receiving variable-length data: Configure DMA in circular mode to continuously fill a buffer. Use UART IDLE line interrupt to detect end of message (gap in transmission). Process the received message and reset the buffer pointer.

---

### Q50: What happens if you don't clear DMA flags after an interrupt?
**Answer:** The interrupt will fire again immediately, creating an infinite loop. Always clear the appropriate flag in DMA_LIFCR or DMA_HIFCR (Low/High Interrupt Flag Clear Register) at the start of your ISR.

---

---

# TIER 2: Role-Dependent Questions (Commonly Asked)

These topics depend on the specific job role and project requirements.

---

## Timers: PWM, Input Capture, Timebase (Questions 51-70)

### Q51: What is a timer in STM32?
**Answer:** A hardware counter that increments (or decrements) at a configurable rate derived from the system clock. Timers can generate periodic interrupts, PWM signals, measure input pulses, and trigger other peripherals (ADC, DMA).

---

### Q52: What is the difference between basic, general-purpose, and advanced timers?
**Answer:**
- **Basic** (TIM6, TIM7): Simple counter, good for timebase, triggers DAC
- **General-purpose** (TIM2-5, TIM12-17): PWM, input capture, multiple channels
- **Advanced** (TIM1, TIM8): All above plus complementary outputs, dead-time, break input for motor control

---

### Q53: What are PSC (Prescaler) and ARR (Auto-Reload Register)?
**Answer:**
- **PSC**: Divides the input clock. Timer clock = fPCLK / (PSC + 1)
- **ARR**: The counter counts up to ARR, then resets to 0 (generates update event)
- Together: Update frequency = fPCLK / ((PSC + 1) × (ARR + 1))

---

### Q54: How do you generate a 1ms timer interrupt?
**Answer:** With 64MHz clock:
```
Desired: 1000Hz (1ms period)
PSC = 63 → Timer clock = 64MHz/64 = 1MHz (1µs per tick)
ARR = 999 → Counts 1000 ticks = 1ms
```
Enable update interrupt (UIE), enable NVIC, write ISR.

---

### Q55: What is PWM and how does a timer generate it?
**Answer:** PWM = Pulse Width Modulation. Timer counts 0→ARR repeatedly. Compare register (CCR) sets the threshold: output HIGH when counter < CCR, LOW when counter ≥ CCR. Duty cycle = CCR/ARR × 100%. Frequency = timer update rate.

---

### Q56: How do you change PWM duty cycle?
**Answer:** Simply write a new value to the CCRx register. For 50% duty: CCR = ARR/2. For 75% duty: CCR = ARR × 0.75. The change takes effect at the next update event (smooth transition).

---

### Q57: What is PWM mode 1 vs PWM mode 2?
**Answer:**
- **Mode 1**: Output HIGH when CNT < CCR, LOW when CNT ≥ CCR
- **Mode 2**: Output LOW when CNT < CCR, HIGH when CNT ≥ CCR
Mode 2 is the inverse of Mode 1. Choose based on whether you want active-high or active-low PWM.

---

### Q58: What is input capture mode?
**Answer:** Timer captures the counter value (into CCR) when an external edge is detected on the input pin. Used to measure pulse width, frequency, or period of external signals. The difference between two captures gives the time between edges.

---

### Q59: How do you measure the frequency of an input signal?
**Answer:** 
1. Configure timer in input capture mode, trigger on rising edge
2. On first capture interrupt, save the counter value
3. On second capture, calculate: Period = Capture2 - Capture1
4. Frequency = Timer_Clock / Period
Handle counter overflow if period is long.

---

### Q60: What is one-pulse mode?
**Answer:** Timer generates a single pulse of configurable delay and width in response to a trigger, then stops. Useful for generating precise delayed pulses without software intervention.

---

### Q61: What is encoder mode on timers?
**Answer:** Special mode where the timer counts quadrature encoder signals (two 90°-offset square waves from a rotary encoder). The counter automatically increments/decrements based on rotation direction and tracks position.

---

### Q62: What is the output compare interrupt used for?
**Answer:** Fires when the counter matches the CCR value. Useful for scheduling events at precise times or generating accurate delays without blocking. You can toggle an output, trigger an action, or just generate a timed interrupt.

---

### Q63: What is dead-time insertion and why is it needed?
**Answer:** In motor control, complementary PWM outputs drive high-side and low-side transistors. Dead-time is a brief period where BOTH are off, preventing shoot-through (short circuit when both turn on simultaneously). Advanced timers have hardware dead-time generators.

---

### Q64: How do you start a timer?
**Answer:** Set the CEN (Counter Enable) bit in TIMx->CR1: `TIM2->CR1 |= TIM_CR1_CEN;`. Before starting, configure PSC, ARR, and any channel settings. The counter begins incrementing from its current value.

---

### Q65: What is the update event?
**Answer:** Occurs when the counter overflows (reaches ARR in up-counting) or underflows (reaches 0 in down-counting). It reloads the counter, updates shadow registers (ARR, PSC, CCR), and optionally generates an interrupt (UIF flag, UIE enable).

---

### Q66: What is the difference between shadow and preload registers?
**Answer:** Many timer registers are buffered. You write to the preload register; the value transfers to the shadow (active) register at the update event. This prevents glitches when changing PWM settings mid-cycle. Enable with ARPE bit.

---

### Q67: How can a timer trigger the ADC?
**Answer:** Configure timer to output TRGO (trigger output) on update event or compare match. Configure ADC to start conversion on that trigger source (external trigger). This creates precise, jitter-free ADC sampling synchronized to timer.

---

### Q68: What is timer DMA request used for?
**Answer:** Timer can request DMA transfers on various events (update, capture, compare). Useful for: updating CCR values automatically (waveform generation), reading capture values without CPU, or burst updates to multiple registers.

---

### Q69: How do you cascade two timers?
**Answer:** Configure Timer A to output TRGO on update. Configure Timer B with slave mode trigger from Timer A. Timer B counts each update of Timer A, effectively creating a 32-bit counter from two 16-bit timers.

---

### Q70: What is the timer's repetition counter (RCR)?
**Answer:** Found on advanced timers. The update event only occurs after RCR+1 counter overflows. Useful for generating lower-frequency events or reducing interrupt rate while maintaining high PWM frequency.

---

## I2C: START/STOP, Repeated START, Key Flags (Questions 71-85)

### Q71: What is I2C and what are its main characteristics?
**Answer:** I2C = Inter-Integrated Circuit. Two-wire serial protocol: SDA (data) and SCL (clock). Multi-master, multi-slave capable. Uses addressing (7 or 10 bit). Open-drain with pull-ups required. Speeds: 100kHz (standard), 400kHz (fast), 1MHz+ (fast-mode plus).

---

### Q72: Why does I2C use open-drain outputs?
**Answer:** Multiple devices share the same bus lines. Open-drain prevents contention (no device actively drives high). Any device can pull the line low. Lines float high via pull-up resistors when no device is pulling low. Enables clock stretching and multi-master arbitration.

---

### Q73: What are START and STOP conditions in I2C?
**Answer:**
- **START**: SDA goes LOW while SCL is HIGH (falling edge on SDA while clock high)
- **STOP**: SDA goes HIGH while SCL is HIGH (rising edge on SDA while clock high)
These are unique conditions that don't occur during normal data transfer (SDA only changes when SCL is low).

---

### Q74: What is a repeated START and why use it?
**Answer:** START condition without preceding STOP. Used to change direction (write→read) without releasing the bus. Example: Write register address, repeated START, read register value. Prevents another master from taking the bus between operations.

---

### Q75: How is addressing done in I2C?
**Answer:** After START, master sends 7-bit address + R/W bit (8 bits total). Slave with matching address responds with ACK (pulls SDA low). Address 0x00 is general call (broadcast), 0x78-0x7B reserved for 10-bit addressing.

---

### Q76: What is ACK and NACK in I2C?
**Answer:**
- **ACK** (Acknowledge): Receiver pulls SDA low during 9th clock - "byte received OK"
- **NACK** (Not Acknowledge): SDA stays high during 9th clock - "problem/done"
Master sends NACK before STOP when reading to signal end of read operation.

---

### Q77: What are the key I2C status flags?
**Answer:**
- **BUSY**: Bus is busy (START detected, no STOP yet)
- **TXE**: Transmit buffer empty (ready for data)
- **RXNE**: Receive buffer not empty (data available)
- **TC**: Transfer complete (all bytes sent/received)
- **NACKF**: NACK received (slave didn't respond)
- **BERR**: Bus error (misplaced START/STOP)

---

### Q78: What is clock stretching?
**Answer:** Slave holds SCL low to pause the master (slave not ready for next byte). Master must wait for SCL to go high before continuing. This allows slow slaves to keep up. Some sensors stretch clock during ADC conversion.

---

### Q79: What pull-up resistor value should be used for I2C?
**Answer:** Typically 4.7kΩ for 100kHz, 2.2kΩ or lower for 400kHz+. Depends on bus capacitance and speed. Too high = slow rise time (communication errors). Too low = excessive current when pulling low. Calculate: R = rise_time / (0.8473 × Cbus).

---

### Q80: How do you write to an I2C register?
**Answer:**
1. Send START + slave address + Write bit (0)
2. Wait for ACK
3. Send register address, wait for ACK
4. Send data byte(s), wait for ACK after each
5. Send STOP

---

### Q81: How do you read from an I2C register?
**Answer:**
1. Send START + slave address + Write bit
2. Send register address, wait for ACK
3. Send REPEATED START + slave address + Read bit (1)
4. Receive data byte(s), send ACK after each (NACK before last)
5. Send STOP

---

### Q82: What causes I2C bus errors?
**Answer:**
- BERR: Invalid START/STOP position
- ARLO: Arbitration lost (multi-master conflict)
- Timeout: Slave stretching too long or device hung
- NACK when ACK expected: Wrong address, slave busy, or disconnected

---

### Q83: How do you recover from a stuck I2C bus?
**Answer:** Toggle SCL manually (bit-bang as GPIO) for 9+ clock cycles to flush stuck slave. Then send STOP condition. If SDA is stuck low, slave is holding it (waiting for clock). Clocking should release it.

---

### Q84: What is 10-bit addressing in I2C?
**Answer:** Extends address space beyond 127 devices. First byte: 11110XX0 where XX are upper 2 bits of address. Second byte: lower 8 bits. Less common; most devices use 7-bit addressing. Check reference manual for how to enable.

---

### Q85: What GPIO settings are required for I2C pins?
**Answer:**
- Mode: Alternate Function
- Output type: Open-drain (crucial!)
- Speed: Fast (for 400kHz)
- Pull: External pull-ups required (can use internal if capacitance is low)
- AF number: Check datasheet (AF4 is common for I2C)

---

## SPI: CPOL/CPHA, NSS, Modes (Questions 86-100)

### Q86: What is SPI and what are its main characteristics?
**Answer:** SPI = Serial Peripheral Interface. Four-wire full-duplex: MOSI, MISO, SCK, NSS/CS. Master-slave only (no multi-master). No addressing - CS pin selects device. Very fast (up to 50MHz+). Simpler than I2C but uses more wires.

---

### Q87: What are the four SPI signals?
**Answer:**
- **SCK**: Clock (always from master)
- **MOSI**: Master Out, Slave In (data from master)
- **MISO**: Master In, Slave Out (data from slave)
- **NSS/CS**: Slave Select/Chip Select (active low, selects which slave)

---

### Q88: What is full-duplex communication in SPI?
**Answer:** Data is exchanged simultaneously in both directions. When master sends a byte on MOSI, it simultaneously receives a byte on MISO. Even if you only want to read, you must transmit (send dummy bytes). Even if you only write, you receive (discard received data).

---

### Q89: What do CPOL and CPHA mean?
**Answer:**
- **CPOL** (Clock Polarity): Idle state of clock. 0=low when idle, 1=high when idle
- **CPHA** (Clock Phase): When data is sampled. 0=first edge, 1=second edge
Together they define 4 SPI modes. Both master and slave must match!

---

### Q90: What are the four SPI modes?
**Answer:**
| Mode | CPOL | CPHA | Description |
|------|------|------|-------------|
| 0 | 0 | 0 | Idle low, sample on rising edge |
| 1 | 0 | 1 | Idle low, sample on falling edge |
| 2 | 1 | 0 | Idle high, sample on falling edge |
| 3 | 1 | 1 | Idle high, sample on rising edge |

Mode 0 and Mode 3 are most common.

---

### Q91: How does chip select (NSS/CS) work?
**Answer:** Master drives CS low to select a specific slave, then performs the transfer, then drives CS high to deselect. Only the selected slave responds. For multiple slaves, each has its own CS line controlled by GPIO.

---

### Q92: What are hardware vs software NSS management?
**Answer:**
- **Hardware**: STM32 controls NSS automatically (limited flexibility)
- **Software**: SSM bit set, you control CS via GPIO (most common, more flexible)
Software management allows precise control of CS timing and works with any pin.

---

### Q93: What is the SSI bit used for?
**Answer:** When using software NSS management (SSM=1), the SSI bit provides the internal NSS value. For master mode: set SSI=1 (internally held high) to prevent mode fault. The actual CS is controlled via separate GPIO.

---

### Q94: What is a mode fault in SPI?
**Answer:** Occurs in master mode when NSS input goes low unexpectedly (another device trying to be master). Indicates a conflict. Generates MODF flag. If using software NSS management with SSI=1, mode faults don't occur.

---

### Q95: How do you set SPI baud rate?
**Answer:** BR bits in SPI_CR1 set prescaler (divide by 2, 4, 8, 16, 32, 64, 128, or 256). SPI_Clock = APB_Clock / Prescaler. For 64MHz APB and 8MHz SPI: use prescaler 8 (BR bits = 010).

---

### Q96: What is the difference between TXE and RXNE flags?
**Answer:**
- **TXE** (TX buffer Empty): Ready to accept new data for transmission
- **RXNE** (RX buffer Not Empty): Received data available to read
In full-duplex: Write when TXE, read when RXNE. Both happen for each byte exchanged.

---

### Q97: What is the BSY flag and why shouldn't you use it to determine when to read?
**Answer:** BSY indicates SPI is actively shifting data. But RXNE is set before BSY clears (data available while still shifting). Use RXNE to know when to read. BSY is mainly useful to know when it's safe to change settings or disable SPI.

---

### Q98: What is MSB-first vs LSB-first?
**Answer:** LSBFIRST bit determines bit order in each byte. MSB-first (LSBFIRST=0) is more common: bit 7 sent first. LSB-first (LSBFIRST=1): bit 0 sent first. Must match the slave device's expectation.

---

### Q99: What GPIO settings are required for SPI?
**Answer:**
- **SCK, MOSI**: Alternate function, push-pull, high speed
- **MISO**: Alternate function (input, so push-pull doesn't matter)
- **CS (software)**: General purpose output, push-pull
Check AF number for your specific pins.

---

### Q100: How do you read a register from an SPI device?
**Answer:** Typically:
1. Assert CS (low)
2. Send register address (with read bit if required)
3. Send dummy byte(s) while reading response
4. Deassert CS (high)
Protocol varies by device - check device datasheet!

---

---

# TIER 3: Advanced/Specialized Questions (For Specific Roles)

These topics are for specialized positions or demonstrate deeper knowledge.

---

## ADC: Sampling Time, Trigger, DMA (Questions 101-120)

### Q101: What does ADC stand for and what does it do?
**Answer:** ADC = Analog-to-Digital Converter. It converts analog voltage (0V to Vref) into a digital value. STM32H7 has 16-bit ADCs that output 0-65535 corresponding to 0V-3.3V (or 12-bit: 0-4095).

---

### Q102: What is ADC resolution and how does it affect precision?
**Answer:** Resolution = number of bits in the output. 12-bit = 4096 steps, 16-bit = 65536 steps. Higher resolution = smaller voltage per step (LSB). 12-bit at 3.3V: 3.3/4096 = 0.8mV/step. 16-bit: 3.3/65536 = 0.05mV/step.

---

### Q103: What is sampling time and why does it matter?
**Answer:** Time the ADC spends charging its internal capacitor from the analog input. Longer sampling time = more accurate for high-impedance sources. Too short = capacitor doesn't fully charge = wrong reading. Set based on source impedance and desired accuracy.

---

### Q104: How do you choose the correct sampling time?
**Answer:** Check reference manual formula: `Ts > (Radc + Rin) × Cadc × ln(2^(n+2))` where Radc is internal resistance, Rin is source resistance, Cadc is sampling capacitor, n is bits. For high-Z sources (>10kΩ), use maximum sampling time.

---

### Q105: What is the ADC conversion time?
**Answer:** Total time = Sampling time + Conversion time. Conversion time depends on resolution and ADC clock. For 12-bit: ~12.5 ADC clock cycles. Higher resolution = more cycles. Total rate = 1/(sampling + conversion).

---

### Q106: What is single conversion vs continuous conversion mode?
**Answer:**
- **Single**: One conversion per trigger, then stops
- **Continuous**: Automatically restarts after each conversion (free-running)
Use single for occasional readings, continuous for streaming data (typically with DMA).

---

### Q107: What triggers can start an ADC conversion?
**Answer:**
- **Software**: Write to ADSTART bit
- **Timer**: Update event, compare match, or TRGO
- **External pin**: EXTI line edge
Timer triggers provide precise, jitter-free sampling at exact intervals.

---

### Q108: What are regular vs injected channels?
**Answer:**
- **Regular**: Normal conversion sequence, one result register, use DMA for multiple channels
- **Injected**: High-priority conversions that can interrupt regular sequence, 4 dedicated result registers
Injected is useful for motor control (sample current at specific PWM phase).

---

### Q109: Why use DMA with ADC?
**Answer:** At high sample rates, CPU can't keep up reading each conversion. DMA automatically transfers results to memory buffer. CPU only processes when buffer is ready. Essential for continuous multi-channel sampling or high-speed applications.

---

### Q110: What is overrun and how to prevent it?
**Answer:** Overrun occurs when a new conversion completes before the previous result was read. Data is lost. Prevent by: using DMA, fast enough interrupt response, or enabling overrun mode (OVRMOD) to overwrite old data.

---

### Q111: How do you configure multi-channel scanning?
**Answer:**
1. Set number of channels in SQR1 (L bits)
2. Program channel sequence in SQRx registers
3. Set sampling time for each channel in SMPRx
4. Enable scan mode
5. Use DMA to store multiple results
Results are stored in sequence order.

---

### Q112: What is the ADC watchdog feature?
**Answer:** Hardware compares each conversion to high/low thresholds. If result is outside the window, it sets a flag and can trigger an interrupt. Useful for monitoring without CPU involvement (alert only when out of range).

---

### Q113: How do you convert ADC value to voltage?
**Answer:** `Voltage = (ADC_Value / Max_Value) × Vref`. For 12-bit with 3.3V reference: `Voltage = (ADC_Value / 4095.0f) × 3.3f`. Use float division to avoid truncation!

---

### Q114: What is the internal reference voltage (Vrefint)?
**Answer:** A stable internal ~1.21V reference connected to a dedicated ADC channel. Used to calibrate readings when Vref/Vdda varies (battery-powered). Formula: `Vdda = 3.3 × CAL_VALUE / ADC_Reading`.

---

### Q115: What is the internal temperature sensor?
**Answer:** An on-chip temperature sensor connected to an ADC channel. Readings vary with temperature. Calibration values stored in system memory allow conversion to °C. Useful for monitoring chip temperature, but not highly accurate.

---

### Q116: How do you power up the ADC on STM32H7?
**Answer:**
1. Exit deep power-down: Clear DEEPPWD bit
2. Enable internal regulator: Set ADVREGEN bit
3. Wait for regulator startup (~10µs)
4. Run calibration: Set ADCAL, wait for it to clear
5. Enable ADC: Set ADEN, wait for ADRDY

---

### Q117: What is ADC calibration and why is it important?
**Answer:** Calibration measures and corrects internal offset errors. The ADC stores calibration factors that are automatically applied to conversions. Run calibration after power-up for best accuracy. Can calibrate for single-ended and differential modes separately.

---

### Q118: What is differential vs single-ended input mode?
**Answer:**
- **Single-ended**: Measures voltage between pin and ground (most common)
- **Differential**: Measures voltage between two pins (rejects common-mode noise)
Differential is better for noisy environments or remote sensors.

---

### Q119: What is the ADC clock source and speed limit?
**Answer:** ADC has its own clock (ADCCLK) from PLL or AHB with prescaler. Maximum depends on chip (typically 36MHz for STM32H7). Faster clock = faster conversions but may reduce accuracy. Balance speed vs accuracy for your needs.

---

### Q120: How do you achieve synchronized multi-ADC sampling?
**Answer:** STM32H7 has multiple ADCs that can work in dual/triple mode - both ADCs trigger simultaneously from the same source. Useful for: simultaneous current/voltage sampling, 3-phase motor current sensing.

---

## Memory Map / Linker Basics (Questions 121-135)

### Q121: What is a memory map?
**Answer:** A diagram/table showing how the processor's address space is organized. It shows where Flash, RAM, peripherals, and system areas are located. On STM32: Flash at 0x08000000, SRAM at 0x20000000, peripherals at 0x40000000+, Cortex-M peripherals at 0xE0000000.

---

### Q122: Why do peripherals have memory addresses?
**Answer:** ARM uses memory-mapped I/O. Peripheral registers appear as memory locations. Reading/writing to a peripheral address actually accesses that peripheral's register. This allows using standard load/store instructions instead of special I/O instructions.

---

### Q123: What is the vector table?
**Answer:** An array of function pointers at the beginning of Flash. First entry: initial stack pointer. Second entry: Reset handler address. Remaining: exception and interrupt handler addresses. CPU uses this to know where to jump on interrupts.

---

### Q124: What information is in a linker script?
**Answer:**
- **MEMORY**: Defines available memory regions (Flash address/size, RAM address/size)
- **SECTIONS**: Defines where code (.text), data (.data), BSS (.bss), and stack go
- Entry point, stack size, heap size

---

### Q125: What is the difference between .text, .data, and .bss sections?
**Answer:**
- **.text**: Executable code and constants (read-only, in Flash)
- **.data**: Initialized global/static variables (stored in Flash, copied to RAM at startup)
- **.bss**: Uninitialized global/static variables (not stored in Flash, zeroed in RAM at startup)

---

### Q126: Why is .data copied from Flash to RAM at startup?
**Answer:** Variables in .data have initial values that must persist between power cycles (stored in Flash). But they must be modifiable at runtime (so must be in RAM). The startup code copies from Flash (LMA) to RAM (VMA) before main() runs.

---

### Q127: What is LMA vs VMA?
**Answer:**
- **LMA** (Load Memory Address): Where data is stored (Flash)
- **VMA** (Virtual Memory Address): Where data is accessed at runtime (RAM)
For .text: LMA=VMA (runs from Flash). For .data: LMA=Flash, VMA=RAM.

---

### Q128: What does the startup file do?
**Answer:**
1. Sets up stack pointer (from vector table)
2. Copies .data section from Flash to RAM
3. Zeros .bss section in RAM
4. Optionally calls initialization functions (__libc_init_array)
5. Calls main()
6. Loops forever if main() returns

---

### Q129: How do you place a variable at a specific memory address?
**Answer:** Use linker section attributes:
```c
__attribute__((section(".mydata"))) uint32_t special_var;
```
Then define the section in the linker script at the desired address.

---

### Q130: What is the heap and stack?
**Answer:**
- **Stack**: LIFO memory for local variables, function calls, return addresses. Grows downward.
- **Heap**: Dynamic memory (malloc/free). Grows upward.
Both share remaining RAM. Stack overflow = overwrites heap/data = crash.

---

### Q131: How do you detect stack overflow?
**Answer:**
- Fill stack with known pattern at startup, check if pattern is overwritten
- MPU (Memory Protection Unit) can trigger fault on stack region access
- Place stack at bottom of RAM; overflow causes HardFault
- Use static analysis tools to estimate max stack usage

---

### Q132: What is the "RAM retention" feature in low-power modes?
**Answer:** Some low-power modes turn off RAM to save power. Content is lost. Other modes retain RAM but turn off peripherals. Configurable per RAM bank on STM32H7. Place critical data in retained RAM sections.

---

### Q133: Why might you use DTCM vs SRAM on STM32H7?
**Answer:**
- **DTCM** (Data Tightly Coupled Memory): Zero wait-state access from CPU, no DMA access
- **SRAM**: Accessible by DMA and CPU, may have wait states
Put frequently accessed data (stack, critical vars) in DTCM. Put DMA buffers in SRAM.

---

### Q134: What is the difference between Flash banks?
**Answer:** STM32H7 has two Flash banks that can be read simultaneously (for execution and data fetch) or used for read-while-write (update one bank while running from other). Dual-bank is useful for firmware updates.

---

### Q135: How do you tell the compiler about available memory?
**Answer:** Through the linker script's MEMORY command:
```
MEMORY {
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 2M
  RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 512K
}
```
Linker uses this to place sections and check for overflow.

---

## Debug Workflow: GDB, Breakpoints, HardFault (Questions 136-150)

### Q136: What is JTAG vs SWD?
**Answer:**
- **JTAG**: 4+ pins (TMS, TCK, TDI, TDO), older standard, supports boundary scan
- **SWD**: 2 pins (SWDIO, SWCLK), ARM-specific, smaller footprint, same functionality
SWD is preferred for STM32 - fewer pins, equally capable for debug.

---

### Q137: What can you do with a debugger?
**Answer:**
- Start/stop/reset the CPU
- Set breakpoints (pause at specific code lines)
- Step through code (line by line)
- Inspect/modify variables and memory
- View registers (CPU and peripheral)
- Examine call stack
- Program Flash

---

### Q138: What is a breakpoint?
**Answer:** A marker that pauses execution when that instruction is reached. Hardware breakpoints (limited, usually 4-6) use CPU debug logic. Software breakpoints (unlimited) replace the instruction with a special opcode. Debugger restores original instruction when continuing.

---

### Q139: What is the difference between step-over and step-into?
**Answer:**
- **Step-into**: If at a function call, enter the function and stop at first line
- **Step-over**: Execute the entire function call and stop at next line in current function
Use step-into to debug inside functions, step-over for functions you trust.

---

### Q140: What is a watchpoint?
**Answer:** Pauses execution when a specific memory location is read or written. Useful to find where a variable is being modified unexpectedly. Hardware watchpoints are limited (typically 2-4). Also called "data breakpoints."

---

### Q141: What is HardFault?
**Answer:** A Cortex-M exception triggered by various error conditions: invalid memory access, undefined instruction, bus error, debug event. It's the "catch-all" fault. If not handled, CPU locks up. HardFault_Handler should log information and reset or halt.

---

### Q142: What are common causes of HardFault?
**Answer:**
- Null pointer dereference
- Stack overflow
- Accessing memory that doesn't exist
- Division by zero (if enabled)
- Accessing peripheral without clock enabled
- Wrong alignment for memory access
- Corrupted function pointer/return address

---

### Q143: How do you debug a HardFault?
**Answer:**
1. Implement HardFault_Handler that saves context
2. Check SCB->CFSR (Configurable Fault Status Register) for fault type
3. Check SCB->BFAR or MMFAR for faulting address
4. Examine stacked registers (PC shows faulting instruction)
5. Look at map file to find which function

---

### Q144: What is the CFSR register?
**Answer:** Configurable Fault Status Register (SCB->CFSR) contains:
- UFSR (Usage Fault): Undefined instruction, illegal state, unaligned access
- BFSR (Bus Fault): Error response from bus (invalid address, access violation)
- MMFSR (MemManage Fault): MPU violation
Reading it tells you what type of fault occurred.

---

### Q145: What does "stacked registers" mean in fault handling?
**Answer:** When an exception occurs, CPU automatically pushes R0-R3, R12, LR, PC, xPSR to the current stack. In the fault handler, you can read these to find the state at fault time. Particularly PC tells you the faulting instruction.

---

### Q146: What is the semihosting feature?
**Answer:** Semihosting redirects I/O calls (printf, file operations) through the debugger to the host PC. Useful for debug output without UART. Requires debugger connection. Slow - not for production. Enable in linker settings.

---

### Q147: What is SWO (Serial Wire Output)?
**Answer:** An additional SWD pin for trace output. Sends debug messages from code (ITM_SendChar) at high speed without CPU interruption. Faster than semihosting, minimal code overhead. Useful for real-time debugging.

---

### Q148: What is the difference between Debug and Release builds?
**Answer:**
- **Debug**: No optimization (-O0), includes debug symbols (-g), larger/slower, easier to debug
- **Release**: Optimization enabled (-O2, -Os), symbols optional, smaller/faster, harder to debug
Some bugs only appear in Release (timing-dependent, optimizer-related).

---

### Q149: Why might a variable not appear in debugger watch window?
**Answer:**
- Optimized away (not actually used, or value held in register)
- Out of scope (local variable from another function)
- No debug symbols (Release build or stripped binary)
Solution: Use `volatile`, disable optimization, or check scope.

---

### Q150: What are assertions and how do you use them in embedded?
**Answer:** `assert(condition)` checks condition at runtime; if false, halts and reports. In embedded, implement assert_failed() to: turn on error LED, log to Flash/UART, trigger breakpoint, or reset. Use liberally during development, optionally disable in production for size.

---

---

# Summary

| Tier | Topic | Questions |
|------|-------|-----------|
| 1 | RCC/Clocks | 1-10 |
| 1 | GPIO/Alternate Functions | 11-20 |
| 1 | Interrupts (NVIC/EXTI) | 21-30 |
| 1 | UART/USART | 31-40 |
| 1 | DMA | 41-50 |
| 2 | Timers | 51-70 |
| 2 | I2C | 71-85 |
| 2 | SPI | 86-100 |
| 3 | ADC | 101-120 |
| 3 | Memory/Linker | 121-135 |
| 3 | Debug/HardFault | 136-150 |

---

**Good luck with your interviews!** 🎯
