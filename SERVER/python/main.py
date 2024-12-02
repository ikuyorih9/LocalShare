from functions.server import *
import os

ip = "192.168.137.1"
porta = 1234
DELAY_BUFFER = 0.001

command = 1
username = input("Usuário: ")
username_len = len(username)
password = input("Senha: ")
password_len = len(password)
# filename = input("Arquivo: ")
filename = "banana_1.png"
filepath = "../../FILES/banana_1.png"
filename_len = len(filename)

try:
    server = cria_servidor(ip, porta)
    if not socket:
        print("Servidor não pode ser criado!")
        exit(-1)

    client, addr = server.accept()
    print(f"Nova conexão de {addr}")

    try:
        client.sendall(bytes([command]))

        print("Enviando tamanho do username...")
        client.sendall(bytes([username_len]))
        print(f"Enviando o username {username}")
        client.sendall(username.encode())

        print("Enviando tamanho da senha...")
        client.sendall(bytes([password_len]))
        print(f"Enviando a senha {password}")
        client.sendall(password.encode())

        if command == 0:
            print("Enviando tamanho do nome arquivo...")
            client.sendall(bytes([filename_len]))
            print(f"Enviando o nome arquivo {filename}")
            client.sendall(filename.encode())
        
            tamanho = os.path.getsize(filepath)
            print(f"Enviando tamanho do nome arquivo ({tamanho})({tamanho.to_bytes(4, byteorder='big')})...")
            client.sendall(tamanho.to_bytes(4, byteorder='big'))
            print("Enviando arquivo:")
            envia_arquivo(client, filepath)
        
        elif command == 1:
            print("Enviando tamanho do nome arquivo...")
            client.sendall(bytes([filename_len]))
            print(f"Enviando o nome arquivo {filename}")
            client.sendall(filename.encode())
            tam_bytes = []
            for i in range (4):
                tam_bytes.append(client.recv(1)[0])
            tam = int.from_bytes(tam_bytes, byteorder='big')
            
            print(f"TAMANHO: {tam}")

            with open("foto.png", "wb") as file:  # Abre ou cria o arquivo para escrita em modo binário
                for i in range(tam):
                    response = client.recv(1)  # Lê até 1024 bytes
                    file.write(response)  # Escreve os bytes no arquivo
                    print(f"{i} cliente: {response}")
                    time.sleep(0.001)

        while True:
            pass
    except Exception as e:
        print(f"Erro ao enviar: {e}")
    finally:
            client.close()
except KeyboardInterrupt as k:
    print("Terminando...")




