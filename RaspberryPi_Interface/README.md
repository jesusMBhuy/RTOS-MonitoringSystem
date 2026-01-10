# Raspberry Pi Interface - Quick Start Guide

## Setup Instructions

### 1. Enable UART on Raspberry Pi
```bash
sudo raspi-config
```
Navigate to:
- **Interfacing Options** → **Serial Port**
- "Would you like a login shell accessible over serial?" → **No**
- "Would you like the serial port hardware enabled?" → **Yes**

Reboot:
```bash
sudo reboot
```

### 2. Install Python Dependencies
```bash
cd RaspberryPi_Interface
pip3 install -r requirements.txt
```

### 3. Connect Hardware
- Connect STM32 **TX (PA9)** to Raspberry Pi **RX (GPIO15/Pin 10)**
- Connect STM32 **RX (PA10)** to Raspberry Pi **TX (GPIO14/Pin 8)**
- Connect **GND** between both devices
- **Important**: Verify voltage levels are compatible (both should be 3.3V)

### 4. Test Serial Connection
```bash
# Check if serial port is available
ls -l /dev/ttyAMA0
ls -l /dev/serial0

# Add user to dialout group (if permission denied)
sudo usermod -a -G dialout $USER
# Then log out and back in
```

### 5. Run the Application
```bash
python3 rasp_data_monitor.py
```

## GUI Controls

The application window displays:
- **4 Real-time Graphs**: Vehicle speed, engine speed, throttle, and gear
- **3 Control Buttons**:
  - **Direccional derecha** (Right turn signal) - Sends 'R' command
  - **Direccional izquierda** (Left turn signal) - Sends 'L' command
  - **Frenado** (Brake) - Sends 'F' command (toggle button)

## Data Output

Data is automatically saved to `DATOS_EV2.csv` in the format:
```
Marca de tiempo, Velocidad del tractor, Velocidad del motor, Acelerador, Marcha
2026-01-09 19:30:45.123, 45.23, 2150.45, 65.00, 3.00
```

## Troubleshooting

### No data received
1. Check serial connections (TX-RX crossover)
2. Verify STM32 is running and transmitting
3. Check serial port name:
   ```bash
   dmesg | grep tty
   ```
4. Try alternative port in code:
   ```python
   ser = serial.Serial("/dev/serial0", baudrate=115200, ...)
   ```

### Permission denied error
```bash
sudo chmod 666 /dev/ttyAMA0
# Or permanently:
sudo usermod -a -G dialout $USER
```

### Application crashes on start
- Ensure X server is running (for GUI)
- Check all Python dependencies are installed
- Verify PyQt5 is compatible with your system

### Graphs not updating
- Confirm data format from STM32 matches expected pattern
- Check terminal output for parsing errors
- Verify serial baud rate matches STM32 (115200)

## Expected Data Format from STM32

```
Velocidad del tractor: XX.XX,Velocidad del motor: XXXX.XX,Acelerador: XX.XX,Marcha: X.XX
```

Each field is followed by a comma except the last one which ends with `\n\r`.

## Performance

- **Update Rate**: ~50ms (graph refresh)
- **Data Rate**: Depends on STM32 transmission (typically ~463ms)
- **Buffer Size**: 1000 points (automatically pruned to prevent memory issues)

---
For more details, see the main [README.md](../README.md) in the project root.
