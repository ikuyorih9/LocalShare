import smbus
import time

# Endereço I2C do ESP3
I2C_ADDRESS = 0x08
DELAY_BUFFER = 0.005

# Inicializa o barramento I2C
bus = smbus.SMBus(1)

def readBytes(n):
        if n == 1:
                byte = bus.read_i2c_block_data(I2C_ADDRESS, 0, 1)[0]
                time.sleep(DELAY_BUFFER)
                return byte

        bytes = []
        for i in range(n):
                byte = bus.read_i2c_block_data(I2C_ADDRESS, 0, 1)[0]
                bytes.append(byte)
                time.sleep(DELAY_BUFFER)
        return bytes

def writeBytes(bytes):
	if not isinstance(bytes, list):
		bus.write_i2c_block_data(I2C_ADDRESS, 0, [bytes])
	else:
		bus.write_i2c_block_data(I2C_ADDRESS, 0, bytes)
	time.sleep(DELAY_BUFFER)

def sendTransmissionCode():
	print("ENVIANDO TRANSMISSAO")
	bus.write_byte_data(I2C_ADDRESS, 0, ord('9'))
	time.sleep(DELAY_BUFFER)
	bus.write_byte_data(I2C_ADDRESS, 0, ord('x'))
	time.sleep(DELAY_BUFFER)
	bus.write_byte_data(I2C_ADDRESS, 0, ord('1'))
	time.sleep(DELAY_BUFFER)
	bus.write_byte_data(I2C_ADDRESS, 0, ord('a'))
	time.sleep(DELAY_BUFFER)
	bus.write_byte_data(I2C_ADDRESS, 0, ord('g'))
	time.sleep(DELAY_BUFFER)
	bus.write_byte_data(I2C_ADDRESS, 0, ord('h'))
	time.sleep(DELAY_BUFFER)
	bus.write_byte_data(I2C_ADDRESS, 0, ord('i'))
	time.sleep(DELAY_BUFFER)
	bus.write_byte_data(I2C_ADDRESS, 0, ord('H'))
	time.sleep(DELAY_BUFFER)

def convertByteToChar(byte):
        return chr(byte)

def convertBytesToString(bytes):
        return ''.join(chr(byte) for byte in bytes)
