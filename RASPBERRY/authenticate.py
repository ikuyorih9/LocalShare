import os
import hashlib

def obtem_diretorio_user(username):
	caminho_storage = os.path.join(os.path.expanduser("~"), "storage")
	return os.path.join(caminho_storage, username)

def obtem_diretorio_arquivo(username, filename):
    caminho_storage = os.path.join(os.path.expanduser("~"), "storage")
    caminho_usuario = os.path.join(caminho_storage, username)
    return os.path.join(caminho_usuario, filename)

# Função para criar o diretório "storage" na pasta home
def criar_diretorio_storage():
    caminho_storage = os.path.join(os.path.expanduser("~"), "storage")
    if not os.path.exists(caminho_storage):
        os.makedirs(caminho_storage)
        print(f"Diretório 'storage' criado em: {caminho_storage}")
    else:
        print(f"O diretório 'storage' já existe em: {caminho_storage}")

# Função para cadastrar um usuário, criando diretório e salvando o hash da senha
def cadastrar_usuario(username, password):
    caminho_storage = os.path.join(os.path.expanduser("~"), "storage")
    caminho_usuario = os.path.join(caminho_storage, username)

    # Verifica se o nome de usuário já existe
    if os.path.exists(caminho_usuario):
        return "Nome de usuário já existe."

    # Cria o diretório do usuário
    os.makedirs(caminho_usuario)
    
    # Calcula o hash da senha
    senha_hash = hashlib.sha256(password.encode()).hexdigest()

    # Define o nome do arquivo de senha
    arquivo_senha = ".senha.bin"  # Arquivo oculto no Linux/macOS
    caminho_senha = os.path.join(caminho_usuario, arquivo_senha)

    # Salva o hash em um arquivo binário
    with open(caminho_senha, "wb") as f:
        f.write(senha_hash.encode())

    # Tenta ocultar o arquivo (apenas em sistemas compatíveis)
    try:
        if os.name == 'nt':  # Windows
            import ctypes
            ctypes.windll.kernel32.SetFileAttributesW(caminho_senha, 2)  # 2 = FILE_ATTRIBUTE_HIDDEN
    except Exception as e:
        print(f"Não foi possível ocultar o arquivo: {e}")

    return "Usuário cadastrado com sucesso."

# Função para verificar o login do usuário
def verificar_login(username, password):
    caminho_storage = os.path.join(os.path.expanduser("~"), "storage")
    caminho_usuario = os.path.join(caminho_storage, username)
    senha_hash = hashlib.sha256(password.encode()).hexdigest()

    # Verifica se o diretório do usuário e o arquivo de senha existem
    arquivo_senha = ".senha.bin"  # Arquivo oculto no Linux/macOS
    caminho_senha = os.path.join(caminho_usuario, arquivo_senha)
    
    if os.path.exists(caminho_usuario) and os.path.isfile(caminho_senha):
        with open(caminho_senha, "rb") as f:
            senha_armazenada = f.read().decode()
            if senha_hash == senha_armazenada:
                return caminho_usuario  # Retorna o caminho do diretório do usuário
    return False

# Função para manipular o diretório do usuário após login
def manipular_diretorio_usuario(caminho_usuario):
    while True:
        print("\nOpções disponíveis:")
        print("1. imprimir - Listar arquivos e imprimir o conteúdo de um arquivo")
        print("2. criar - Criar um novo arquivo")
        print("3. logout - Retornar ao menu principal")
        
        operacao = input("Escolha uma opção: ").lower()

        if operacao == "imprimir":
            arquivos = [f for f in os.listdir(caminho_usuario) if f != ".senha.bin"]
            if not arquivos:
                print("Nenhum arquivo disponível.")
                continue

            print("Arquivos disponíveis:")
            for arquivo in arquivos:
                print(f"- {arquivo}")

            nome_arquivo = input("Digite o nome do arquivo a ser impresso: ")
            caminho_arquivo = os.path.join(caminho_usuario, nome_arquivo)

            if os.path.isfile(caminho_arquivo):
                with open(caminho_arquivo, "r") as f:
                    print("\nConteúdo do arquivo:")
                    print(f.read())
            else:
                print("Arquivo não encontrado.")

        elif operacao == "criar":
            nome_arquivo = input("Digite o nome do novo arquivo: ")
            caminho_arquivo = os.path.join(caminho_usuario, nome_arquivo)

            try:
                with open(caminho_arquivo, "w") as f:
                    f.write("")  # Cria um arquivo vazio
                print(f"Arquivo '{nome_arquivo}' criado com sucesso.")
            except Exception as e:
                print(f"Erro ao criar o arquivo: {e}")

        elif operacao == "logout":
            print("Logout realizado.")
            break

        else:
            print("Opção inválida.")
