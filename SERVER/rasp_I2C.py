import os
import hashlib
from smbus2 import SMBus, i2c_msg
import time

# Configurações do I2C
I2C_ADDRESS = 0x08  # Endereço da Raspberry Pi
I2C_BUS = 1  # Geralmente o barramento I2C 1 é usado na Raspberry Pi

# Função para criar o diretório "storage"
def criar_diretorio_storage():
    caminho_storage = os.path.join(os.path.expanduser("~"), "storage")
    if not os.path.exists(caminho_storage):
        os.makedirs(caminho_storage)
        print(f"Diretório 'storage' criado em: {caminho_storage}")
    else:
        print(f"O diretório 'storage' já existe em: {caminho_storage}")

# Função para cadastrar um usuário
def cadastrar_usuario(username, password):
    caminho_storage = os.path.join(os.path.expanduser("~"), "storage")
    caminho_usuario = os.path.join(caminho_storage, username)

    if os.path.exists(caminho_usuario):
        return "USER_EXISTS"

    os.makedirs(caminho_usuario)
    senha_hash = hashlib.sha256(password.encode()).hexdigest()

    arquivo_senha = ".senha.bin"
    caminho_senha = os.path.join(caminho_usuario, arquivo_senha)

    with open(caminho_senha, "wb") as f:
        f.write(senha_hash.encode())
    
    return "SUCCESS"

# Função para verificar o login do usuário
def verificar_login(username, password):
    caminho_storage = os.path.join(os.path.expanduser("~"), "storage")
    caminho_usuario = os.path.join(caminho_storage, username)
    senha_hash = hashlib.sha256(password.encode()).hexdigest()

    arquivo_senha = ".senha.bin"
    caminho_senha = os.path.join(caminho_usuario, arquivo_senha)

    if os.path.exists(caminho_usuario) and os.path.isfile(caminho_senha):
        with open(caminho_senha, "rb") as f:
            senha_armazenada = f.read().decode()
            if senha_hash == senha_armazenada:
                return "SUCCESS"
    return "INVALID_CREDENTIALS"

# Função para processar comandos da ESP32
def processar_comando(comando):
    partes = comando.split('|')
    if len(partes) < 3:
        return "ERROR"

    acao, username, password = partes[0], partes[1], partes[2]

    if acao == "REGISTER":
        return cadastrar_usuario(username, password)
    elif acao == "LOGIN":
        return verificar_login(username, password)
    else:
        return "INVALID_COMMAND"

# Função principal para comunicação I2C
def iniciar_comunicacao_i2c():
    bus = SMBus(I2C_BUS)
    criar_diretorio_storage()

    print("Aguardando comandos da ESP32 via I2C...")
    while True:
        try:
            # Recebe os dados da ESP32
            read = i2c_msg.read(I2C_ADDRESS, 32)
            bus.i2c_rdwr(read)
            comando = ''.join([chr(byte) for byte in read])
            comando = comando.strip('\x00')  # Remove bytes nulos
            
            print(f"Comando recebido: {comando}")

            # Processa o comando e obtém a resposta
            resposta = processar_comando(comando)
            print(f"Resposta enviada: {resposta}")

            # Envia a resposta para a ESP32
            write = i2c_msg.write(I2C_ADDRESS, resposta.encode('utf-8'))
            bus.i2c_rdwr(write)

        except Exception as e:
            print(f"Erro na comunicação I2C: {e}")
        time.sleep(0.1)

if __name__ == "__main__":
    try:
        iniciar_comunicacao_i2c()
    except KeyboardInterrupt:
        print("\nEncerrando comunicação I2C.")
