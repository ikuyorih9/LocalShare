import socket
import time

def cria_servidor(IP, PORTA):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((IP, PORTA))
    server_socket.listen(2)

    print(f"Servidor em {IP}:{PORTA}")

    return server_socket

def envia_arquivo(client, filepath):
    print(f"Abrindo arquivo {filepath}...")
    try:
        with open(filepath, 'rb') as file:
            byte = file.read(1)
            while byte:
                if byte != b'\r':
                    print(f"Byte: {byte}")
                    client.sendall(byte)
                byte = file.read(1)
                time.sleep(0.1)
            client.sendall(bytes([0]))
                
    except FileNotFoundError:
        print(f"Arquivo {filepath} não foi encontrado!")
    except IOError as e:
        print(f"Erro de IO ao abrir o arquivo {filepath}: {e}")
    except Exception as e:
        print(f"Ocorreu um erro inesperado: {e}")
