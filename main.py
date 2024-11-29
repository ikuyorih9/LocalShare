from SERVER.server import *
import os

ip = "192.168.137.1"
porta = 1234

username = input("Usuário: ")
username_len = len(username)
password = input("Senha:")
password_len = len(password)
# filename = input("Arquivo: ")
filename = "./FILES/banana_1.png"
filename_len = len(filename)

try:
    server = cria_servidor(ip, porta)
    if not socket:
        print("Servidor não pode ser criado!")
        exit(-1)

    client, addr = server.accept()
    print(f"Nova conexão de {addr}")

    try:
        client.sendall(bytes([0]))

        print("Enviando tamanho do username...")
        client.sendall(bytes([username_len]))
        print(f"Enviando o username {username}")
        client.sendall(username.encode())

        print("Enviando tamanho da senha...")
        client.sendall(bytes([password_len]))
        print(f"Enviando a senha {password}")
        client.sendall(password.encode())

        print("Enviando tamanho do nome arquivo...")
        client.sendall(bytes([filename_len]))
        print(f"Enviando o nome arquivo {filename}")
        client.sendall(filename.encode())
    
        # tamanho = os.path.getsize(filename)
        # print(f"Enviando tamanho do nome arquivo ({tamanho})({tamanho.to_bytes(4, byteorder='little')})...")
        # client.sendall(tamanho.to_bytes(4, byteorder='little'))
        # print("Enviando arquivo:")
        # envia_arquivo(client, filename)
        while True:
            pass
    except Exception as e:
        print(f"Erro ao enviar: {e}")
    finally:
            client.close()
except KeyboardInterrupt as k:
    print("Terminando...")




