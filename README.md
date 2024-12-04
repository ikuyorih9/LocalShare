![LocalShare Logo](https://github.com/user-attachments/assets/0b7b7547-2c83-4567-a108-39c2c36037b1)

<p align="center">
<img src="https://img.shields.io/github/license/ikuyorih9/LocalShare"/>
<img src="https://img.shields.io/badge/_-Python-ffd343?logo=Python&logoColor=323330"/>
<img src="https://img.shields.io/badge/_-C/C++-3186a0?logo=cplusplus&logoColor=323330"/>
<img src="https://img.shields.io/badge/_-RaspberryPiOS-E30B5C?logo=raspberrypi&logoColor=323330"/>
<img src="https://img.shields.io/badge/_-HTML5-ec6231?logo=html5&logoColor=323330"/>
<img src="https://img.shields.io/badge/_-CSS3-2965f1?logo=css3&logoColor=323330"/>
<img src="https://img.shields.io/badge/_-JavaScript-f0db40?logo=javascript&logoColor=323330"/>
</p>

*Compartilhe um servidor pessoal de arquivos com um grupo particular!*

Desenvolvido por:

* Gabriel Henrique Brioto
* Hugo Hiroyuki Nakamura
* Isaac Santos Soares

## 📋 Descrição

Um servidor local compartilhado permite que vários usuários cadastrados possam acessar arquivos de uma ou mais unidades de armazenamento comuns. A interação dos usuários com o sistema se dá por meio de uma interface de linha de comando ou por um site (**em breve**), onde é requisitado o endereço TCP/IP da ESP32, um usuário e senha. O armazenamento de arquivos é sigiloso e a requisição por parte de um usuário e senha mostra apenas os arquivos relacionados ao seu nome de usuário.

O site se comunica com a ESP32 utilizando um protocolo TCP/IP. Ela, por sua vez, realiza a conexão I2C com a **Raspberry Pi**, que se responsabilizará em armazenar e servir os arquivos de cada usuário. A **ESP32** utiliza processamento paralelo para permitir um maior fluxo de dados e uma comunicação de duas vias simultâneas, para envio e recebimento de dados.

## 🖥️ Funções das Tecnologias

### 🔹 Raspberry Pi 3B+:

Realiza a logística de armazenamento de arquivos, empregando o sistema operacional Raspberry Pi OS para armazenar e recuperar arquivos separados por usuário.

### 🔹 ESP32:

Estabelece a comunicação TCP/IP com a aplicação e I2C com a Raspberry Pi. Utiliza o sistema operacional FreeRTOS para especializar dois núcleos da ESP32 para requisição e resposta.

## ⚙️ Funcionalidades

1. **Login de Usuário**: O usuário informa suas credenciais (nome de usuário e senha), que são codificadas e enviadas ao servidor via WebSocket. O servidor valida as credenciais e responde com a confirmação de sucesso ou falha.
2. **Cadastro de Usuário**: O novo usuário informa as credenciais que deseja utilizar para acessar o sistema (nome de usuário e senha) e o sistema destina um espaço para armazenar os arquivos do novo usuário.
3. **Envio e Recebimento de Arquivos**: O sistema permite que os arquivos sejam enviados e recebidos por meio de WebSockets. O arquivo é dividido em pacotes de dados binários e transmitido byte a byte.

## 📂 Estrutura de Dados da Comunicação

A comunicação do usuário para com o servidor, e vice-versa, segue a estrutura:
- Header
- Dados Enviados/Recebidos

### Header
O cabeçalho contém dados como o comando a ser executado, nome de usuário e senha seguindo a estrutura:

- 1 byte de comando (`0x01`)
- 1 byte com tamanho do nome do usuário
- Nome do usuário
- 1 byte com o tamanho da senha
- Senha do usuário
- 1 byte com o tamanho do nome do arquivo
- Nome do arquivo (caracteres `char` byte a byte)

### Dados Enviados/Recebidos

Os dados enviados/recebidos devem ser transmitidos seguindo a estrutura:
- 4 bytes para tamanho do Dado
- Dado a ser transmitido

## 📂 Documentação dos Arquivos da ESP32

### main.ino

Este arquivo contém a configuração inicial e o loop principal do ESP32.

- **setup()**: Configura o WiFi, inicializa o servidor WebSocket, e cria as filas e tarefas.
- **loop()**: Mantém a conexão com o servidor e permite a execução de outras tarefas.
- **onWebSocketEvent()**: Manipula eventos do WebSocket.

### i2c.hpp

Define os pinos e funções para comunicação I2C.

- **onRequest()**: Função chamada quando o ESP32 recebe uma solicitação I2C.
- **onReceive(int numBytes)**: Função chamada quando o ESP32 recebe dados via I2C.

### i2c.cpp

Implementa as funções definidas em `i2c.hpp`.

- **onRequest()**: Envia dados da fila `DataToSend` via I2C.
- **onReceive(int numBytes)**: Recebe dados via I2C e os coloca na fila `DataToReceive`.

### frtos.hpp

Define constantes e declara funções para tarefas FreeRTOS.

- **Tarefa1(void *pvParameters)**: Lê dados do cliente e envia para a fila `DataToSend`.
- **Tarefa2(void *pvParameters)**: Lê dados da fila `DataToReceive` e envia para o cliente.
- **byte4_to_int(uint8_t * byte)**: Converte um array de 4 bytes em um inteiro.
- **sendStringToQueue(char * message, int tam)**: Envia uma string para a fila `DataToSend`.
- **sendBytesToQueue(uint8_t * bytes, int tam)**: Envia um array de bytes para a fila `DataToSend`.

### frtos.cpp

Implementa as funções definidas em `frtos.hpp`.

- **Tarefa1(void *pvParameters)**: Lê dados do cliente e envia para a fila `DataToSend`.
- **Tarefa2(void *pvParameters)**: Lê dados da fila `DataToReceive` e envia para o cliente.
- **byte4_to_int(uint8_t * byte)**: Converte um array de 4 bytes em um inteiro.
- **sendStringToQueue(char * message, int tam)**: Envia uma string para a fila `DataToSend`.
- **sendBytesToQueue(uint8_t * bytes, int tam)**: Envia um array de bytes para a fila `DataToSend`.

### Dependências

- Arduino.h
- Wire.h
- WiFi.h
- ESPAsyncWebServer.h
- AsyncTCP.h
- ArduinoJson.h

## 📂 Documentação dos Arquivos da Raspberry Pi

### main.py

Este é o ponto de entrada do projeto. Ele inicializa o diretório de armazenamento e entra em um loop infinito para processar comandos recebidos via I2C.

#### Funções Principais

- `criar_diretorio_storage()`: Cria o diretório de armazenamento se ele não existir.
- `read_header()`: Lê o comando, nome de usuário e senha do barramento I2C.
- `verificar_login(username, password)`: Verifica se o usuário existe e se a senha está correta.
- `cadastrar_usuario(username, password)`: Cadastra um novo usuário se ele não existir.
- `store_file(username)`: Armazena um arquivo enviado pelo usuário.
- `load_file(username)`: Carrega um arquivo solicitado pelo usuário.
- `list_files(username)`: Lista os arquivos disponíveis para o usuário.

### i2c_functions.py

Contém funções para comunicação via I2C, incluindo leitura e escrita de bytes.

#### Funções Principais

- `readBytes(n)`: Lê `n` bytes do barramento I2C.
- `sendTransmissionCode()`: Envia um código de transmissão específico via I2C.
- `convertByteToChar(byte)`: Converte um byte em um caractere.
- `convertBytesToString(bytes)`: Converte uma lista de bytes em uma string.

### commands.py

Define funções para ler comandos, armazenar arquivos, carregar arquivos e listar arquivos.

#### Funções Principais

- `read_header()`: Lê o comando, nome de usuário e senha do barramento I2C.
- `store_file(username)`: Armazena um arquivo enviado pelo usuário.
- `load_file(username)`: Carrega um arquivo solicitado pelo usuário.
- `list_files(username)`: Lista os arquivos disponíveis para o usuário.

### authenticate.py

Contém funções para autenticação de usuários, incluindo criação de diretórios, cadastro de usuários e verificação de login.

#### Funções Principais

- `obtem_diretorio_user(username)`: Retorna o caminho do diretório do usuário.
- `obtem_diretorio_arquivo(username, filename)`: Retorna o caminho completo de um arquivo do usuário.
- `criar_diretorio_storage()`: Cria o diretório de armazenamento se ele não existir.
- `cadastrar_usuario(username, password)`: Cadastra um novo usuário se ele não existir.
- `verificar_login(username, password)`: Verifica se o usuário existe e se a senha está correta.
- `manipular_diretorio_usuario(caminho_usuario)`: Permite manipular o diretório do usuário após o login.

### Dependências

- smbus
- time
- os
- hashlib

## 📂 Documentação dos Arquivos do Server

### Descrição
Este servidor TCP aceita conexões de clientes, recebe credenciais de usuário e realiza operações como envio e recebimento de arquivos.

### Configuração
- `ip`: Endereço IP do servidor.
- `porta`: Porta do servidor.
- `DELAY_BUFFER`: Buffer de atraso para envio de dados.

### Comandos
- `command = 0`: Envia um arquivo para o cliente.
- `command = 1`: Recebe um arquivo do cliente.
- `command = 2`: Lista arquivos no servidor.
- `command = 3`: Cadastra um novo usuário.

### Funções
#### `cria_servidor(IP, PORTA)`
Cria um servidor socket TCP.

#### `envia_arquivo(client, filepath)`
Envia um arquivo para o cliente conectado.

#### `main.py`
Este script principal configura o servidor, aceita conexões de clientes e executa comandos com base na entrada do usuário.

##### Fluxo de Execução
1. Solicita `username` e `password` do usuário.
2. Cria o servidor e aguarda conexões.
3. Envia credenciais e executa comandos baseados no valor de `command`.

##### Comandos Detalhados
- **command = 0**: Envia um arquivo especificado para o cliente.
- **command = 1**: Recebe um arquivo do cliente e salva localmente.
- **command = 2**: Lista arquivos disponíveis no servidor.
- **command = 3**: Cadastra um novo usuário no sistema.

#### `functions/server.py`
Contém funções auxiliares para criar o servidor e enviar arquivos.

##### Funções
- `cria_servidor(IP, PORTA)`: Cria e configura um servidor TCP.
- `envia_arquivo(client, filepath)`: Envia um arquivo binário para o cliente conectado.]

### Dependências
Para executar o servidor, você precisará das seguintes bibliotecas Python:
- `socket`: Biblioteca padrão para comunicação de rede.
- `time`: Biblioteca padrão para manipulação de tempo.
- `os`: Biblioteca padrão para interagir com o sistema operacional.

## 📦 Instalação

1. Clone o repositório (na Raspberry Pi e no seu PC).
   
3. Instale o [Arduino IDE](https://www.arduino.cc/en/software)

4. Instale o [Python](https://www.python.org/downloads/)

5. Instale as dependências para a ESP32 e Raspberry Pi conforme listadas nos tópicos anteriores.

## 🚀 Uso

1. Compile/Upload do [main.ino](./ESP32/main/main.ino) para ESP32. 

3. Inicie código de armazenamento da Raspberry Pi executando [main.py](./RASPBERRY/main.py).

4. Inicie o servidor no site ou na aplicação de interface de comando (no seu PC).
   - O servidor de interface de comnado é inicado ao executar [main.py](./SERVER/python/main.py).
   - Para o site basta acessar o [link](https://local-share-web-page.vercel.app/).
     
6. Envie e receba arquivos conforme necessário.

### Esquemático de Ligação I2C

<img alt="Esquemático Ligação I2C" src="https://github.com/user-attachments/assets/a6a376ad-6cdf-4e98-b5a8-319fb951b6fe" width="60%">


## 🤝 Contribuições

Você pode contribuir para o nosso projeto, fazendo um fork do nosso repositório e fazendo pull requests...

### Melhorias Faltantes
   1. Fazer o site funcionar 100%
   2. Fazer uma aplicação com Interface gráfica para o cliente rodar de maneira local e amigável
   3. Adicionar criptografia dos arquivos de cada usuário usando o hash da sua senha

## 📄 Licença

Este projeto está licenciado sob a licença MIT. Veja o arquivo [LICENSE](./LICENSE) para mais detalhes.
