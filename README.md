
*Compartilhe um servidor pessoal entre um grupo particular!*

Desenvolvido por:

* Gabriel Henrique Brioto
* Hugo Hiroyuki Nakamura
* Isaac Santos Soares

## 📋 Descrição
Um servidor local compartilhado permite que vários usuários cadastrados possam acessar arquivos de uma ou mais unidades de armazenamento comuns. Uma aplicação, baseada no projeto **LiveChat**, possibilita um terminal compartilhado para a comunicação com o servidor. O armazenamento de arquivos é sigiloso e a requisição por parte de um usuário deve mostrar apenas os arquivos relacionados ao seu nome.

A aplicação se comunica ao servidor através de um protocolo TCP/IP que se conecta a uma **ESP32**. Ela, por sua vez, realiza a conexão serial com a **Raspberry Pi** que se responsabilizará em separar os arquivos em diferentes unidades de armazenamento. Tanto a **ESP32** quanto a **Raspberry Pi** funcionarão com processamento paralelo, para permitir um maior fluxo de dados e uma comunicação de duas vias simultâneas, para envio e recebimento de dados. 
  
## 🖥️ Funções das tecnologias
### 🔹 Raspberry Pi 3B+: 
Realiza a logística de distribuição de arquivos, empregando o sistema operacional RaspOS para armazenar e recuperar arquivos separados por usuário; 
### 🔹 ESP32: 
Estabelece a comunicação TCP/IP com a aplicação e serial com a Raspberry Pi. Utiliza o sistema operacional FreeRTOS para especializar dois núcleos da ESP32 para requisição e resposta.
