# STM32 Microprocessor Projects

A collection of embedded systems projects developed for STM32 microcontroller programming, featuring real-time applications with digital I/O, timers, filters, PWM control, and RTC functionality.

## Overview

This repository contains multiple C++ projects targeting STM32 microcontrollers using the **mbed embedded OS framework**. Each project demonstrates key embedded systems concepts through practical implementations on breadboard hardware with LEDs, buttons, sensors, and display modules.

**Lab Projects** (MIT-Labor_1 through MIT-Labor_5) form the core curriculum, progressing from basic I/O control to advanced features like real-time clocks and exponential filtering. **Exam Projects** validate mastery through integrated applications combining multiple concepts.

## Project Structure

```
MIT-Labor_1/          Digital I/O, button debouncing, LED control
MIT-Labor_2/          Display drivers, shift registers, sensor filtering
MIT-Labor_3/          Hex input shifters, measuring devices, 7-segment display
MIT-Labor_4/          PWM generation, multi-channel pulse control
MIT-Labor_5/          RTC integration, time-based scheduling
MIT-klausur2022WiSe/  Exam: integrated pattern controller with rotating animations
mit048/               Reaction time measurement application
```

Each project follows the structure:
```
projectName/
└── myCode/
    ├── main.cpp                    # Entry point
    ├── breadboard.h                # Hardware pin mappings
    ├── C*.cpp / C*.h               # Core class implementations
    └── [utilities].cpp / .h        # Helper modules
```

## Key Features

### Embedded Systems Concepts
- **Digital I/O**: Button input handling, LED output control via bus operations
- **Debouncing**: Hardware-independent software debouncer for stable input
- **Timers**: Polled timers for non-blocking time measurement and scheduling
- **Signal Processing**: Exponential and moving average filters for sensor smoothing
- **PWM Control**: Multi-channel pulse width modulation for analog-like output
- **Display Drivers**: 7-segment display and shift register output extenders
- **Real-Time Clocks**: Time synchronization and RTC integration (Labor_5)

### Software Architecture
- **Object-Oriented Design**: Modular class hierarchy with clear responsibilities
- **Hardware Abstraction**: Breadboard-agnostic implementations using mbed API
- **Reusable Components**: Common utilities (timers, debouncers, filters) across projects
- **Stateful Applications**: Pattern controllers and reaction apps with internal state machines

## Technologies

- **C++**: Core implementation language (C++11 features)
- **mbed OS**: Embedded operating system framework
- **STM32**: ARM Cortex-M microcontroller family
- **CMSIS-DAP**: Hardware debugging interface

## Building and Running

### Prerequisites
- mbed CLI toolchain installed
- ARM GCC compiler
- STM32 microcontroller with USB/CMSIS-DAP interface
- mbed Studio IDE or command-line build environment

### Compilation

```bash
# Navigate to project directory
cd MIT-Labor_X/myCode

# Compile with mbed build system
mbed compile --target <target> --toolchain GCC_ARM

# Alternatively, use mbed Studio for graphical builds
```

### Deployment

1. Connect STM32 microcontroller via USB
2. Flash compiled binary using drag-and-drop or `mbed cp` command
3. Interact via breadboard LED/button interface or serial console

## Project Descriptions

**MIT-Labor_1**: Fundamental LED control and button input patterns with debouncing
- Toggle LEDs via key presses, state persistence, and debounced edge detection

**MIT-Labor_2**: Display-driven applications combining shift registers and digital filters
- 7-segment display output, exponential smoothing for sensor data

**MIT-Labor_3**: Hex input conversion and measurement systems
- Shifter logic for multi-input conversion, measuring device with display output

**MIT-Labor_4**: Advanced PWM generation with multi-channel support
- 4-channel PWM controllers with configurable frequency and duty cycle

**MIT-Labor_5**: Time-based control and RTC functionality
- Real-time clock integration, debouncer and filter combinations, time-scheduled tasks

**MIT-klausur2022WiSe**: Rotating pattern animations (Exam integration)
- Three animation modes controlled via button interface using pattern controller

**mit048**: Reaction time measurement game
- Stimulus-response measurement with LED feedback and timing analysis

## Design Patterns

- **Timer Pattern**: `CPolledTimer` for non-blocking time tracking
- **Debouncer Pattern**: `CDebouncer` for stable digital input filtering
- **Filter Pattern**: Strategy pattern with `CExponentialFilter` and `CMovingAverageFilter`
- **Controller Pattern**: `CPatternController` managing LED outputs and display logic

## Key Learnings

- Designing embedded systems with hardware-software co-design
- Implementing robust real-time control without interrupts (polled architecture)
- Managing complexity through object-oriented design in resource-constrained environments
- Translating hardware datasheets into functional abstractions
- Testing and validation of embedded applications

## Author

Steven Atmodjo (Mat. Nr. 772550)

## License

Educational use
