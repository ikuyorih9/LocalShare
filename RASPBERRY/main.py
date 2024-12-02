import smbus
import time
from commands import *
from authenticate import *

criar_diretorio_storage()

while True:
	try:
		command, username, password = read_header()

		userExists = verificar_login(username, password)
		if not userExists:
			cadastrar_usuario(username, password)

		if command == 0:
			store_file(username)
		elif command == 1:
			load_file(username)
		elif command == 2:
			list_files(username)
	except KeyboardInterrupt as k:
		print(f"Fim de execução!")
	time.sleep(1)  # Espera 1 segundo antes de tentar novamente
