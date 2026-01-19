# 🔧 STM32 Bare-Metal Academy

<p align="center">
  <img src="https://img.shields.io/badge/Platform-STM32H753ZI-blue?style=for-the-badge&logo=stmicroelectronics" alt="STM32H753ZI"/>
  <img src="https://img.shields.io/badge/Language-C-green?style=for-the-badge&logo=c" alt="C"/>
  <img src="https://img.shields.io/badge/Level-Bare%20Metal-red?style=for-the-badge" alt="Bare Metal"/>
  <img src="https://img.shields.io/badge/Tutorials-17-orange?style=for-the-badge" alt="17 Tutorials"/>
</p>

<p align="center">
  <b>Learn embedded systems programming the RIGHT way — without HAL, without abstractions, just you and the hardware.</b>
</p>

---

## 📖 What Is This?

This repository is a **complete, hands-on curriculum** for learning STM32 microcontroller programming at the **register level**. No HAL. No LL. No hand-holding. Just pure bare-metal C programming on the **Nucleo-H753ZI** board (ARM Cortex-M7, 480 MHz).

Each tutorial is an **interactive learning experience** with:
- 📚 **Lessons** explaining how the hardware works
- ✏️ **Exercises** with `???` placeholders for you to fill in
- 💡 **Hints** to guide you without giving away the answer
- ✅ **Solutions** when you need to check your work

---

## 🎯 Who Is This For?

- 🎓 **Students** learning embedded systems
- 💼 **Engineers** transitioning from HAL to bare-metal
- 🔍 **Interview candidates** preparing for embedded roles
- 🧠 **Curious developers** who want to understand what's really happening

---

## 📂 Repository Structure

```
📁 STM32-Bare-Metal-Academy/
├── 📁 Questions and Tests/
│   ├── 📄 STM32_Interview_Questions.md  🎤 150 Interview Questions
│   └── 📄 STM32_Quiz.md                 📝 Test Your Knowledge
├── 📁 Tutorial Projects/
│   ├── 📄 project1_reaction_game.c      🎮 Hands-on Project
│   ├── 📄 project2_digital_clock.c      ⏰ Hands-on Project
│   ├── 📄 project3_led_metronome.c      🎵 Hands-on Project
│   └── 📄 project4_uart_console.c       💻 Hands-on Project
├── 📁 Tutorials/
│   ├── 📄 00_bit_manipulation_tutorial.c ⭐ Start here!
│   ├── 📄 gpio_tutorial.c               ⭐⭐
│   ├── 📄 rcc_tutorial.c                ⭐⭐
│   ├── 📄 nvic_tutorial.c               ⭐⭐
│   ├── 📄 exti_tutorial.c               ⭐⭐
│   ├── 📄 uart_tutorial.c               ⭐⭐⭐
│   ├── 📄 tim_tutorial.c                ⭐⭐⭐
│   ├── 📄 adc_tutorial.c                ⭐⭐⭐
│   ├── 📄 dac_tutorial.c                ⭐⭐
│   ├── 📄 dma_tutorial.c                ⭐⭐⭐
│   ├── 📄 spi_tutorial.c                ⭐⭐⭐
│   ├── 📄 i2c_tutorial.c                ⭐⭐⭐
│   ├── 📄 flash_tutorial.c              ⭐⭐⭐
│   ├── 📄 rtc_tutorial.c                ⭐⭐⭐
│   ├── 📄 watchdog_tutorial.c           ⭐⭐⭐
│   ├── 📄 cortex_tutorial.c             ⭐⭐⭐⭐
│   └── 📄 eth_tutorial.c                ⭐⭐⭐⭐⭐
└── 📄 README.md
```

---

## 🗺️ Learning Path

### Phase 1: Foundations
| # | Tutorial | Topics | Difficulty |
|---|----------|--------|------------|
| 1 | `bit_manipulation_tutorial.c` | Bitwise AND, OR, XOR, shifts, masks | ⭐ |
| 2 | `gpio_tutorial.c` | Pin modes, input/output, LED blink | ⭐⭐ |
| 3 | `rcc_tutorial.c` | Clock tree, HSI/HSE, prescalers | ⭐⭐ |

### Phase 2: Interrupts & Timing
| # | Tutorial | Topics | Difficulty |
|---|----------|--------|------------|
| 4 | `nvic_tutorial.c` | NVIC, priorities, enable/disable IRQs | ⭐⭐ |
| 5 | `exti_tutorial.c` | External interrupts, edge detection | ⭐⭐ |
| 6 | `tim_tutorial.c` | Timer basics, PWM, delays | ⭐⭐⭐ |
| 7 | `watchdog_tutorial.c` | IWDG, WWDG, system recovery | ⭐⭐⭐ |
| 8 | `rtc_tutorial.c` | Real-time clock, backup domain | ⭐⭐⭐ |

### Phase 3: Communication
| # | Tutorial | Topics | Difficulty |
|---|----------|--------|------------|
| 9 | `uart_tutorial.c` | USART, baud rate, TX/RX | ⭐⭐⭐ |
| 10 | `spi_tutorial.c` | SPI master, clock polarity/phase | ⭐⭐⭐ |
| 11 | `i2c_tutorial.c` | I2C master, addressing, ACK/NACK | ⭐⭐⭐ |

### Phase 4: Analog & Memory
| # | Tutorial | Topics | Difficulty |
|---|----------|--------|------------|
| 12 | `adc_tutorial.c` | ADC conversion, channels, resolution | ⭐⭐⭐ |
| 13 | `dac_tutorial.c` | DAC output, waveform generation | ⭐⭐ |
| 14 | `dma_tutorial.c` | DMA transfers, circular mode | ⭐⭐⭐ |
| 15 | `flash_tutorial.c` | Flash erase/program, unlock sequence | ⭐⭐⭐ |

### Phase 5: Advanced
| # | Tutorial | Topics | Difficulty |
|---|----------|--------|------------|
| 16 | `cortex_tutorial.c` | CPU registers, cache, FPU, SysTick | ⭐⭐⭐⭐ |
| 17 | `eth_tutorial.c` | Ethernet MAC, PHY, DMA descriptors | ⭐⭐⭐⭐⭐ |

---

## 🎤 Interview Questions

The repository includes **150 curated interview questions** across three tiers:

| Tier | Level | Questions | Topics |
|------|-------|-----------|--------|
| 🥉 Tier 1 | Junior | 50 | GPIO, RCC, basic peripherals |
| 🥈 Tier 2 | Mid-Level | 50 | Interrupts, DMA, communication protocols |
| 🥇 Tier 3 | Senior | 50 | System design, optimization, debugging |

Perfect for:
- Interview preparation
- Self-assessment
- Technical discussions

---

## 🛠️ Hardware Requirements

- **Board**: [NUCLEO-H753ZI](https://www.st.com/en/evaluation-tools/nucleo-h753zi.html)
- **MCU**: STM32H753ZI (Cortex-M7, 480 MHz, 2MB Flash, 1MB RAM)
- **Debugger**: ST-Link V3 (built into Nucleo board)
- **Optional**: Oscilloscope, logic analyzer, sensors

---

## 🚀 Getting Started

### Prerequisites
- ARM GCC Toolchain (`arm-none-eabi-gcc`)
- CMake (3.20+)
- OpenOCD or STM32CubeProgrammer
- VS Code with Cortex-Debug extension (recommended)

---

## 📝 How to Use the Tutorials

Each tutorial file follows this format:

```c
/* ============================================================================
 *  LESSON: Explanation of the concept
 * ============================================================================ */

/* ============================================================================
 *  ✏️  EXERCISE: Your task
 * ============================================================================ */

void SomeFunction(void) {
    /* ✏️ YOUR TURN: Description of what to do */
    PERIPHERAL->REG = ???;    /* HINT: Conceptual hint here */
}

/* ─────────────────────────────────────────────────────────────────────────────
 * 💡 SOLUTION:
 * 
 * PERIPHERAL->REG = ACTUAL_VALUE;
 * ───────────────────────────────────────────────────────────────────────────── */
```

1. **Read the LESSON** to understand the concept
2. **Attempt the EXERCISE** by replacing `???` with your answer
3. **Use the HINT** if you're stuck (it won't give away the answer!)
4. **Check the SOLUTION** to verify your work

---

## 🎓 What You'll Learn

By completing all tutorials, you will understand:

- ✅ How to configure GPIO pins at the register level
- ✅ How the STM32 clock tree works (RCC)
- ✅ How interrupts work (NVIC, EXTI)
- ✅ How to implement UART, SPI, I2C from scratch
- ✅ How ADC and DAC peripherals function
- ✅ How DMA transfers work
- ✅ How to read/write internal Flash memory
- ✅ How timers and PWM work
- ✅ How watchdog timers protect your system
- ✅ How the Cortex-M7 CPU core operates
- ✅ How Ethernet MAC and PHY communicate

---

## 🤔 Why Bare Metal?

| HAL/Libraries | Bare Metal |
|---------------|------------|
| Fast development | Deep understanding |
| Abstracted complexity | Full control |
| Portable code | Optimized performance |
| Hidden behavior | Transparent operation |
| Larger binary size | Minimal footprint |

**Learn bare metal first, then use HAL with understanding.**

---

## 📚 Additional Resources

- [STM32H753 Reference Manual (RM0433)](https://www.st.com/resource/en/reference_manual/rm0433-stm32h742-stm32h743753-and-stm32h750-value-line-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) 
- [STM32H753 Datasheet](https://www.st.com/resource/en/datasheet/stm32h753zi.pdf)
- [Cortex-M7 Technical Reference Manual](https://developer.arm.com/documentation/ddi0489/latest/)
- [ARM Architecture Reference Manual](https://developer.arm.com/documentation/ddi0403/latest/)

---

## 🤝 Contributing

Contributions are welcome! Feel free to:
- Add new tutorials
- Improve explanations
- Fix bugs
- Add more interview questions

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

<p align="center">
  <b>⭐ If this helped you learn, give it a star! ⭐</b>
</p>

<p align="center">
  Made with ❤️ for the embedded community by <b>Tommyl404</b>
</p>
