# STM32 Firmware - Build and Flash Guide

## Prerequisites

- **STM32CubeIDE** (recommended) or any ARM GCC toolchain
- **ST-Link** debugger/programmer
- **Hardware**: STM32F103RBT6 development board

## Building the Project

### Using STM32CubeIDE (Recommended)

1. **Import Project**
   ```
   File → Open Projects from File System
   Import source: [Select STM32_Firmware folder]
   Finish
   ```

2. **Build**
   ```
   Project → Build All (Ctrl+B)
   ```
   
   Or right-click project → **Build Project**

3. **Expected Output**
   ```
   Debug/Equipo3_Evidencia2___.elf
   Debug/Equipo3_Evidencia2___.bin
   Debug/Equipo3_Evidencia2___.map
   ```

### Using Command Line (Advanced)

```bash
cd STM32_Firmware

# Clean previous build
make clean

# Build
make all

# Output will be in Debug/ folder
```

## Flashing the Firmware

### Using STM32CubeIDE

1. Connect ST-Link to the board
2. Click **Run → Debug** (F11) or **Run → Run** (Ctrl+F11)
3. The IDE will automatically flash and start debugging

### Using st-flash (Command Line)

```bash
# Flash the binary
st-flash write Debug/Equipo3_Evidencia2___.bin 0x8000000

# Reset the board
st-flash reset
```

### Using OpenOCD

```bash
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
  -c "program Debug/Equipo3_Evidencia2___.elf verify reset exit"
```

## Hardware Configuration

### Pin Connections

| Pin | Function | Description |
|-----|----------|-------------|
| PA0 | ADC1_CH0 | Throttle potentiometer (0-3.3V) |
| PA9 | USART1_TX | UART transmit to Raspberry Pi |
| PA10 | USART1_RX | UART receive from Raspberry Pi |
| PB11 | GPIO_Output | LED - Left turn signal |
| PB12 | GPIO_Output | LED - Right turn signal |
| PB0-PB7 | GPIO | LCD data/control (parallel mode) |
| PC0-PC3 | GPIO | 4x4 Keypad rows |
| PC4-PC7 | GPIO | 4x4 Keypad columns |

### Clock Configuration

- **Source**: HSI (8 MHz internal oscillator)
- **PLL**: x16 multiplier
- **SYSCLK**: 64 MHz
- **AHB**: 64 MHz (no division)
- **APB1**: 32 MHz (÷2 - max 36 MHz)
- **APB2**: 64 MHz (÷1)

### Keypad Layout

```
┌────┬────┬────┬────┐
│ 1  │ 2  │ 3  │ A  │
├────┼────┼────┼────┤
│ 4  │ 5  │ 6  │ B  │  ← Use '4' and '6' for turn signals
├────┼────┼────┼────┤
│ 7  │ 8  │ 9  │ C  │
├────┼────┼────┼────┤
│ *  │ 0  │ #  │ D  │
└────┴────┴────┴────┘
```

- Press **'4'**: Left turn signal
- Press **'6'**: Right turn signal
- Press **'2'**: Manual brake (1000 Nm)

## Debugging

### Serial Monitor (View Output)

Use any serial terminal at **115200 baud, 8N1**:

```bash
# Linux/Mac
screen /dev/ttyUSB0 115200

# Windows (PuTTY, Tera Term, etc.)
# Configure: COM port, 115200, 8-N-1
```

Expected output format:
```
Velocidad del tractor: 45.23,Velocidad del motor: 2150.45,Acelerador: 65.00,Marcha: 3.00
```

### Common Build Errors

**Error**: `undefined reference to '_sbrk'`
- **Solution**: Ensure `syscalls.c` and `sysmem.c` are included in build

**Error**: FreeRTOS headers not found
- **Solution**: Verify `Middlewares/Third_Party/FreeRTOS/` exists and include paths are correct

**Error**: HAL library errors
- **Solution**: Check `Drivers/STM32F1xx_HAL_Driver/` is present

### Runtime Issues

**LCD shows garbage**
- Check I2C address or parallel connections
- Verify `LCD_Init()` is called before use

**No UART output**
- Verify `printf()` retarget in `syscalls.c` redirects to USART1
- Check PA9/PA10 configuration

**ADC reads constant value**
- Confirm PA0 is configured as analog input
- Check potentiometer connections

## Code Structure

```
main.c
├── main()                        # Entry point, initialization
├── Task1_Init()                  # Peripheral initialization
├── startTask1()                  # ADC & UART RX (33ms)
├── startTask2()                  # Model update & control (200ms)
├── startTask3()                  # UART TX - data send (463ms)
├── startTask4()                  # LCD display (582ms)
├── SystemClock_Config()          # 64 MHz clock setup
├── USER_RCC_Init()               # Peripheral clocks
├── USER_GPIO_Init()              # GPIO configuration
├── USER_ADC1_Enable()            # ADC initialization
└── ADC_ConvertirValor()          # ADC read function
```

## Modifying the Code

### Change Task Periods

Edit `osDelay()` calls in each task:
```c
void startTask1(void) {
    for (;;) {
        // ... task code ...
        osDelay(33);  // Change this value (in milliseconds)
    }
}
```

### Adjust Throttle Range

In `startTask1()`:
```c
throttle = ((adcValue * 100) / 4095.0);
if (throttle < 5.0) {
    throttle = 5.0;  // Minimum throttle percentage
}
```

### Modify Brake Torque

In `startTask2()`:
```c
else if (button == '2') {
    EngTrModel_U.BrakeTorque = 1000.0;  // Change this value
    // ...
}
```

## Performance Optimization

- **Stack Sizes**: Each task has 384 bytes stack (adjust in `osThreadDef()` if needed)
- **Queue Size**: Message queue holds 4 `AccelerationMessage` structures
- **Mutex**: Protects LCD access between Task2 and Task4

## Memory Usage

- **Flash**: ~80-100 KB (includes HAL, FreeRTOS, and Simulink model)
- **RAM**: ~15-18 KB (heap, stacks, global variables)
- **Available**: STM32F103RBT6 has 128KB Flash, 20KB RAM

---

For detailed project information, see the main [README.md](../README.md) in the project root.
