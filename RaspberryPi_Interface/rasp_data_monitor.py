import serial
import csv
import datetime
import threading
import numpy as np
import pyqtgraph as pg
from PyQt5 import QtCore, QtGui, QtWidgets

# Configura la comunicación serial
ser = serial.Serial("/dev/ttyAMA0", baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=1)

# Crea el archivo CSV y escribe el encabezado
archivo_csv = open('DATOS_EV2.csv', 'w', newline='')
escritor_csv = csv.writer(archivo_csv)
escritor_csv.writerow(['Marca de tiempo', 'Velocidad del tractor', 'Velocidad del motor', 'Acelerador', 'Marcha'])  # Encabezado

# Inicializa PyQtGraph
app = pg.mkQApp()

# Configura las gráficas
win = pg.GraphicsLayoutWidget(show=True)
win.setWindowTitle('Real-time Tractor Data')
plot1 = win.addPlot(title="Velocidad del tractor")
curve1 = plot1.plot(pen='y')
plot2 = win.addPlot(title="Velocidad del motor")
curve2 = plot2.plot(pen='r')
plot3 = win.addPlot(title="Acelerador")
curve3 = plot3.plot(pen='g')
plot4 = win.addPlot(title="Marcha")
curve4 = plot4.plot(pen='b')

# Listas para almacenar los datos para graficar
max_points = 1000  # Limitar el tamaño de las listas a 1000 puntos
marcas_de_tiempo = []
velocidades_vehiculo = []
velocidades_motor = []
aceleradores = []
marchas = []

# Función para actualizar la gráfica en tiempo real
def actualizar_grafica():
    if len(marcas_de_tiempo) > 0:
        marcas_de_tiempo_np = np.array(marcas_de_tiempo, dtype='datetime64[ms]').astype(np.float64)
        velocidades_vehiculo_np = np.array(velocidades_vehiculo, dtype=float)
        velocidades_motor_np = np.array(velocidades_motor, dtype=float)
        aceleradores_np = np.array(aceleradores, dtype=float)
        marchas_np = np.array(marchas, dtype=float)
        
        curve1.setData(marcas_de_tiempo_np, velocidades_vehiculo_np)
        curve2.setData(marcas_de_tiempo_np, velocidades_motor_np)
        curve3.setData(marcas_de_tiempo_np, aceleradores_np)
        curve4.setData(marcas_de_tiempo_np, marchas_np)

# Función para leer datos del puerto serial
def leer_datos():
    primera_linea = True
    while True:
        try:
            # Leer una línea del puerto serial
            linea = ser.readline()
            
            # Verificar que la línea no esté vacía
            if linea:
                # Decodificar la línea y dividirla en partes
                datos = linea.decode().strip().split(',')
                if datos:
                    if primera_linea:
                        escritor_csv.writerow(datos)
                        primera_linea = False
                    else:
                        try:
                            timestamp = datetime.datetime.now()
                            velocidad_vehiculo = float(datos[0].split(': ')[1])
                            velocidad_motor = float(datos[1].split(': ')[1])
                            acelerador = float(datos[2].split(': ')[1])
                            marcha = float(datos[3].split(': ')[1])

                            marcas_de_tiempo.append(timestamp)
                            velocidades_vehiculo.append(velocidad_vehiculo)
                            velocidades_motor.append(velocidad_motor)
                            aceleradores.append(acelerador)
                            marchas.append(marcha)

                            escritor_csv.writerow([timestamp, velocidad_vehiculo, velocidad_motor, acelerador, marcha])
                            archivo_csv.flush()

                            # Limitar el tamaño de las listas para no agotar la memoria
                            if len(marcas_de_tiempo) > max_points:
                                marcas_de_tiempo.pop(0)
                                velocidades_vehiculo.pop(0)
                                velocidades_motor.pop(0)
                                aceleradores.pop(0)
                                marchas.pop(0)

                        except ValueError:
                            print(f"Error de conversión de valor: {datos}")
                        except IndexError:
                            print(f"Error de índice: {datos}")

                print(f'Datos recibidos: {datos}')
        except serial.SerialException as e:
            print(f"Error en la comunicación serial: {e}")
        except Exception as e:
            print(f"Error desconocido: {e}")

# Crear hilos para la lectura de datos y actualización de gráficas
thread_lectura = threading.Thread(target=leer_datos)
thread_lectura.daemon = True
thread_lectura.start()

# Actualizar la gráfica periódicamente
timer = QtCore.QTimer()
timer.timeout.connect(actualizar_grafica)
timer.start(0)

# Crear la aplicación PyQt5
class MainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()

        central_widget = QtWidgets.QWidget()
        self.setCentralWidget(central_widget)

        layout = QtWidgets.QVBoxLayout()
        central_widget.setLayout(layout)

        # Botón para Direccional derecha
        self.button_derecha = QtWidgets.QPushButton('Direccional derecha')
        self.button_derecha.clicked.connect(self.enviar_R)
        layout.addWidget(self.button_derecha)

        # Botón para Direccional izquierda
        self.button_izquierda = QtWidgets.QPushButton('Direccional izquierda')
        self.button_izquierda.clicked.connect(self.enviar_L)
        layout.addWidget(self.button_izquierda)

        # Botón para frenado
        self.button_frenar = QtWidgets.QPushButton('Frenado')
        self.button_frenar.setCheckable(True)  # Hacer el botón conmutado
        self.button_frenar.clicked.connect(self.toggle_enviar_F)
        layout.addWidget(self.button_frenar)

        # Agregar el widget de PyQtGraph a la interfaz
        layout.addWidget(win)

    def enviar_R(self):
        ser.write('R'.encode() + b'\n')

    def enviar_L(self):
        ser.write('L'.encode() + b'\n')

    def toggle_enviar_F(self, pressed):
        if pressed:
            ser.write('F'.encode() + b'\n')

# Ejecutar la aplicación PyQt5
if __name__ == '__main__':
    main_app = QtWidgets.QApplication([])
    main_window = MainWindow()
    main_window.show()
    main_app.exec_()

    # Cerrar la comunicación serial y el archivo CSV cuando se interrumpe el programa
    ser.close()
    archivo_csv.close()

