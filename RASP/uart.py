import serial

from Header import Header
from File import File

def setup():
  # Configura a porta serial
  ser = serial.Serial('/dev/serial0', 115200, timeout=1)  # GPIO 14 (TX), 15 (RX)
  ser.flush()
  
  print("Pronto para receber dados da ESP32")
  return ser

def get_header(ser):
  function = ser.read(1)
  function = int.from_bytes(function, byteorder='big')
  
  user_name_size = ser.read(1)
  user_name_size = int.from_bytes(user_name_size, byteorder='big')

  user_name = ser.read(user_name_size)
  user_name = user_name.decode('utf-8')

  password_size = ser.read(1)
  password_size = int.from_bytes(password_size, byteorder='big')

  password = ser.read(password_size)
  password = password.decode('utf-8')

  return Header(function, user_name_size, user_name, password_size, password)

def get_file(ser):
  file_name_size = ser.read(1)
  file_name_size = int.from_bytes(file_name_size, byteorder='big')

  file_name = ser.read(file_name_size)
  file_name = file_name.decode('utf-8')
  
  return File(file_name_size, file_name)

def write_file(ser, f : File):
  file_size = ser.read(4)
  f.file_size = int.from_bytes(file_size, byteorder='big')

  with open(f.file_name, 'wb') as file:
    while f.bytes_written < f.file_size:
      file_data = ser.read(1)
      file.write(file_data)
      f.bytes_written += 1

def send_file(ser, f : File):
  ser.write(f.file_name_size.to_bytes(1, byteorder='big'))
  ser.write(f.file_name.encode('utf-8'))

  ser.write(f.file_size.to_bytes(4, byteorder='big'))

  with open(f.file_name, 'rb') as file:
    for _ in range(f.file_size):
        file_data = file.read(1)
        ser.write(file_data)
