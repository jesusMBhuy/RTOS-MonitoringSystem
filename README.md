# Tractor Engine & Transmission Simulation System

A real-time embedded system that simulates a tractor's engine and transmission behavior using an STM32F103 microcontroller with FreeRTOS, interfaced with a Raspberry Pi for data visualization and remote control.

## Project Overview

This project implements a comprehensive vehicle dynamics simulation system consisting of:

- **STM32F103 Firmware**: Real-time simulation of engine, transmission, and vehicle dynamics
- **Raspberry Pi Interface**: Data acquisition, visualization, and remote control interface
- **UART Communication**: High-speed serial link between both systems (115200 baud)
- **Real-time Control**: FreeRTOS-based multitasking with mutex synchronization

## Team Members

- **Jesús Javier Martínez Hernández** - A00833296
- **Diego Hilario López Rodriguez** - A00836492
- **Diego José Roca Rodriguez** - A00836390
- **Victor Alejandro Meneses Garza** - A01384002

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    STM32F103RB (Cortex-M3)                  │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  FreeRTOS Tasks:                                     │   │
│  │  • Task1: ADC Reading & UART Reception (33ms)       │   │
│  │  • Task2: Engine Model & Control Logic (200ms)      │   │
│  │  • Task3: UART Data Transmission (463ms)            │   │
│  │  • Task4: LCD Display Update (582ms)                │   │
│  └──────────────────────────────────────────────────────┘   │
│                           ↕ UART (115200 baud)              │
└─────────────────────────────────────────────────────────────┘
                            ↕
┌─────────────────────────────────────────────────────────────┐
│                    Raspberry Pi 3/4                         │
│  • PyQt5 GUI with real-time graphs                          │
│  • CSV data logging                                         │
│  • Remote control buttons (Turn signals, Brake)            │
└─────────────────────────────────────────────────────────────┘
```

## Hardware Requirements

### STM32 System
- **Microcontroller**: STM32F103RBT6 (128KB Flash, 20KB RAM)
- **Clock**: 64 MHz (PLL from HSI)
- **Peripherals**:
  - ADC1 (Channel 0 - PA0): Throttle input
  - USART1 (PA9/PA10): Raspberry Pi communication
  - GPIO (PB11/PB12): LED indicators (turn signals)
  - I2C/Parallel: 16x2 LCD display
  - GPIO Matrix: 4x4 Keypad
  - TIM2, TIM4: System timers

### Raspberry Pi System
- **Model**: Raspberry Pi 3/4
- **OS**: Raspbian/Raspberry Pi OS
- **Serial Port**: `/dev/ttyAMA0` or `/dev/serial0`

### Additional Components
- 16x2 LCD Display (I2C or parallel interface)
- 4x4 Matrix Keypad
- Potentiometer (throttle input)
- LEDs for turn signal indicators

## Software Dependencies

### STM32 Firmware
- STM32CubeIDE (v1.x or later)
- STM32 HAL Library
- FreeRTOS (included in project)
- ARM GCC Toolchain

### Raspberry Pi
```bash
pip install pyserial numpy pyqtgraph PyQt5
```

## Getting Started

### 1. STM32 Firmware Setup

#### Build and Flash
```bash
cd Tractor_Engine_Simulation_System/STM32_Firmware

# Using STM32CubeIDE:
# 1. Import project: File → Open Projects from File System
# 2. Select STM32_Firmware folder
# 3. Build: Project → Build All (Ctrl+B)
# 4. Flash: Run → Debug (F11)

# Or using command line:
make clean
make all
# Flash using st-flash or OpenOCD
```

#### Configuration
- **ADC**: Configured on PA0 for throttle input (0-4095 → 5-100%)
- **UART1**: 115200 baud, 8N1 format
- **System Clock**: 64 MHz from HSI via PLL

### 2. Raspberry Pi Interface Setup

```bash
cd Tractor_Engine_Simulation_System/RaspberryPi_Interface

# Enable UART on Raspberry Pi
sudo raspi-config
# Navigate to: Interface Options → Serial Port
# - Login shell: No
# - Serial hardware: Yes

# Run the monitoring application
python3 rasp_data_monitor.py
```

## System Features

### Engine & Transmission Model
The system uses a **Simulink-generated model** (`EngTrModel`) that simulates:
- **Engine Dynamics**: RPM calculation based on throttle and load
- **Transmission**: Automatic gear shifting with configurable shift points
- **Torque Converter**: Impeller/turbine torque transfer
- **Vehicle Speed**: Calculated from engine speed and gear ratios
- **Braking System**: Manual and automatic brake torque application

### Control Inputs

#### Physical Controls (STM32)
| Input | Function |
|-------|----------|
| Potentiometer (PA0) | Throttle control (5-100%) |
| Keypad '4' | Turn signal left |
| Keypad '6' | Turn signal right |
| Keypad '2' | Manual brake |

#### Remote Controls (Raspberry Pi)
| Button | Command | Function |
|--------|---------|----------|
| "Direccional izquierda" | 'L' | Left turn signal (toggle LED) |
| "Direccional derecha" | 'R' | Right turn signal (toggle LED) |
| "Frenado" | 'F' | Emergency brake (3000 Nm torque) |

### Data Transmission Format

The STM32 sends data via UART at **463ms intervals**:
```
Velocidad del tractor: 45.23,Velocidad del motor: 2150.45,Acelerador: 65.00,Marcha: 3.00
```

Fields:
- **Velocidad del tractor**: Vehicle speed (km/h or m/s)
- **Velocidad del motor**: Engine speed (RPM)
- **Acelerador**: Throttle percentage (5-100%)
- **Marcha**: Current gear (1-4)

### Real-time Visualization

The Raspberry Pi displays 4 synchronized graphs:
1. **Tractor Speed** (Yellow)
2. **Engine Speed** (Red)
3. **Throttle Position** (Green)
4. **Current Gear** (Blue)

Data is also logged to `DATOS_EV2.csv` with timestamps.

## FreeRTOS Task Scheduling

| Task | Priority | Stack | Period | Function |
|------|----------|-------|--------|----------|
| Task1 | Normal | 384 | 33ms | ADC sampling & UART RX |
| Task2 | Normal | 384 | 200ms | Model update & control |
| Task3 | Normal | 384 | 463ms | UART TX (data transmission) |
| Task4 | Normal | 384 | 582ms | LCD display update |

**Synchronization**: Mutex `Mutex1Handle` protects LCD access between Task2 and Task4.

## Project Structure

```
Tractor_Engine_Simulation_System/
├── .gitignore
├── README.md
│
├── STM32_Firmware/
│   ├── Core/
│   │   ├── Inc/
│   │   │   ├── main.h
│   │   │   ├── EngTrModel.h          # Simulink model header
│   │   │   ├── lcd.h                  # LCD driver
│   │   │   ├── uart.h                 # UART driver
│   │   │   ├── teclado.h              # Keypad driver
│   │   │   ├── timer.h
│   │   │   └── FreeRTOSConfig.h
│   │   └── Src/
│   │       ├── main.c                 # Main application & tasks
│   │       ├── EngTrModel.c           # Generated Simulink code
│   │       ├── lcd.c
│   │       ├── uart.c
│   │       ├── teclado.c
│   │       ├── timer.c
│   │       └── freertos.c
│   ├── Drivers/
│   │   ├── STM32F1xx_HAL_Driver/     # HAL library
│   │   └── CMSIS/                     # ARM CMSIS
│   ├── Middlewares/
│   │   └── Third_Party/
│   │       └── FreeRTOS/              # FreeRTOS kernel
│   ├── STM32F103RBTX_FLASH.ld        # Linker script
│   ├── STM32_Debug.launch             # Debug configuration
│   └── EQ3_EvidenciaFinal.ioc        # STM32CubeMX project
│
└── RaspberryPi_Interface/
    └── rasp_data_monitor.py           # Main Python application
```

## Customization

### Modifying the Engine Model
The engine/transmission model is generated from Simulink. To modify:
1. Edit the Simulink model (`.slx` file - not included)
2. Generate code using Embedded Coder
3. Replace files in `Core/Inc/` and `Core/Src/` with prefix `EngTrModel*`

### Adjusting Task Periods
In [main.c](STM32_Firmware/Core/Src/main.c), modify `osDelay()` values:
```c
osDelay(33);   // Task1: ADC sampling rate
osDelay(200);  // Task2: Control loop frequency
osDelay(463);  // Task3: Data transmission rate
osDelay(582);  // Task4: Display refresh rate
```

### Changing UART Baud Rate
**STM32**: Modify `USER_USART1_Init()` in [uart.c](STM32_Firmware/Core/Src/uart.c)

**Raspberry Pi**: Change `baudrate` parameter:
```python
ser = serial.Serial("/dev/ttyAMA0", baudrate=115200, ...)
```

## Troubleshooting

### STM32 Issues

**Problem**: Build errors about missing FreeRTOS headers
- **Solution**: Ensure `Middlewares/Third_Party/FreeRTOS` is present and paths are configured in project

**Problem**: ADC reads zero
- **Solution**: Check PA0 connection and verify `USER_ADC1_Enable()` is called

**Problem**: UART not transmitting
- **Solution**: Verify PA9/PA10 configuration and check `printf()` redirect in `syscalls.c`

### Raspberry Pi Issues

**Problem**: `Permission denied: /dev/ttyAMA0`
- **Solution**: Add user to dialout group:
  ```bash
  sudo usermod -a -G dialout $USER
  # Log out and back in
  ```

**Problem**: No data received
- **Solution**: 
  - Check serial port name (`/dev/serial0` or `/dev/ttyAMA0`)
  - Verify UART is enabled in `raspi-config`
  - Check TX/RX connections (TX-STM32 → RX-RasPi)

**Problem**: Graph not updating
- **Solution**: Ensure PyQtGraph timer is running and data format matches expected pattern

## Performance Metrics

- **ADC Sampling**: 30 Hz (33ms period)
- **Control Loop**: 5 Hz (200ms period)
- **Data Transmission**: ~2.16 Hz (463ms period)
- **Display Update**: ~1.72 Hz (582ms period)
- **CPU Usage**: ~40-60% (estimated, with all tasks running)

## License

This project was developed as part of a university course. Please check with the original authors for licensing information.

## Contributing

For improvements or bug fixes, please contact the team members listed above.

## Support

For questions or issues, contact any of the team members via their institutional email addresses.

---

**Last Updated**: January 9, 2026
**Version**: 2.0
**Course**: Sistemas en Chip (Chip-on-Systems)
**Institution**: Tecnológico de Monterrey
