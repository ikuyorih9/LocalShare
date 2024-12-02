from i2c_functions import *
from authenticate import *
import os
from time import sleep

def read_header():
	while True:
#		bus.write_byte_data(I2C_ADDRESS, 0, 0x97)

		command = readBytes(1)
		print(f"Comando: {command}({convertByteToChar(command)})")

#		continue;

		if command == 0 or command == 1 or command == 2:
			username_len = readBytes(1)
			username = convertBytesToString(readBytes(username_len))
			print(f"\tUsername ({username_len} bytes): {username}")

			password_len = readBytes(1)
			password = convertBytesToString(readBytes(password_len))
			print(f"\tPassword ({password_len} bytes): {password}")
			
			return command, username, password

def store_file(username):
	filename_len = readBytes(1)
	filename = convertBytesToString(readBytes(filename_len))
	print(f"\tFilename ({filename_len} bytes): {filename}")

	file_len = int.from_bytes(readBytes(4), byteorder='big')
	print(f"File_len: {file_len}")
	try:
		filepath = obtem_diretorio_arquivo(username, filename)
		print(f"Filepath: {filepath}")
		with open(filepath, 'wb') as file:
			for i in range (file_len):
				byte = readBytes(1)
				file.write(byte.to_bytes(1, byteorder='big'))
				print(f"{i} - Byte {byte}({chr(byte)}) escrito!")
	except FileNotFoundError as fe:
		print(f"Não foi possível encontrar o arquivo: {e}")

def load_file(username):
	filename_len = readBytes(1)
	filename = convertBytesToString(readBytes(filename_len))
	print(f"\tFilename ({filename_len} bytes): {filename}")

	filepath = obtem_diretorio_arquivo(username, filename)
	print(f"Filepath: {filepath}")

	file_len = os.path.getsize(filepath)
	print(f"File_len: {file_len}")

	file_len_bytes = list(file_len.to_bytes(4, byteorder='big'))
	bus.write_i2c_block_data(I2C_ADDRESS, 0x02, file_len_bytes)
	sleep(4*RW_DELAY)

	try:
		with open(filepath, 'rb') as file:
			byte = file.read(1)
			i = 0
			while byte:
				bus.write_byte_data(I2C_ADDRESS, 0x01, byte[0])
				print(f"{i} - {byte[0]}{byte}")
				sleep(RW_DELAY)
				byte = file.read(1)
				i += 1

		success_byte = readBytes(1)
		while success_byte == 0:
			print(f"Esperando por resposta do servidor... ({success_byte})")
			success_byte = readBytes(1)
	except Exception as e:
		print(f"{e}")


def list_files(username):
	path = obtem_diretorio_user(username)
	
	files = []
	for filename in os.listdir(path):
		if filename != ".senha.bin":
			print(f"FILE: {filename}")
			for i in range(len(filename)):
				bus.write_byte_data(I2C_ADDRESS, 0x01, ord(filename[i]))
