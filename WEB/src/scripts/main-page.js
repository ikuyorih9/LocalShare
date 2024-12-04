let ws;
let receivingFile = false;
let receivedFileName = '';
let receivedFileSize = 0;
let receivedFileData = [];
let fileNameSize = 0;
let receivedBytesCount = 0;

const wsUrl = localStorage.getItem('wsUrl');
if (!wsUrl) {
  alert('URL do WebSocket não configurada. Redirecionando para a página inicial.');
  window.location.href = '/index.html';
} else {
  console.log('Conectando ao WebSocket:', wsUrl);
  ws = new WebSocket(wsUrl);

  ws.onopen = () => {
    console.log('WebSocket conectado');
  };

  ws.onmessage = (event) => {
    const data = new Uint8Array(event.data);

    if (!receivingFile) {
      // Primeira parte: comando + tamanho do nome do arquivo
      if (data[0] === 0x00) {
        receivingFile = true;
        fileNameSize = data[1]; // Tamanho do nome do arquivo
        receivedFileName = '';
        receivedFileSize = 0;
        receivedBytesCount = 0;
        receivedFileData = [];
        console.log(`Recebendo arquivo... Tamanho do nome: ${fileNameSize} bytes`);
      } else {
        console.error('Comando inválido recebido!');
      }
    } else {
      if (receivedFileName.length < fileNameSize) {
        // Montando o nome do arquivo
        for (let i = 0; i < data.length; i++) {
          if (receivedFileName.length < fileNameSize) {
            receivedFileName += String.fromCharCode(data[i]);
          } else {
            receivedFileSize = (receivedFileSize << 8) | data[i]; // Monta o tamanho do arquivo em 4 bytes
          }
        }

        console.log(`Nome do arquivo recebido: ${receivedFileName}`);
        console.log(`Tamanho do arquivo: ${receivedFileSize} bytes`);
      } else if (receivedBytesCount < receivedFileSize) {
        // Recebendo os bytes do arquivo
        receivedFileData.push(...data);
        receivedBytesCount += data.length;
        console.log(`Recebido ${receivedBytesCount}/${receivedFileSize} bytes`);

        if (receivedBytesCount >= receivedFileSize) {
          // Finaliza a recepção
          console.log(`Arquivo '${receivedFileName}' recebido com sucesso!`);
          salvarArquivo(receivedFileName, receivedFileData);
          resetFileReception();
        }
      }
    }
  };

  ws.onerror = (error) => {
    console.error('Erro no WebSocket:', error);
  };

  ws.onclose = () => {
    console.log('WebSocket desconectado');
    alert('Conexão perdida com a ESP32. Redirecionando para a página de login.');
    window.location.href = './login.html';
  };
}

function salvarArquivo(fileName, fileData) {
  // Converte os dados recebidos em um Blob
  const blob = new Blob([new Uint8Array(fileData)], { type: 'application/octet-stream' });
  const link = document.createElement('a');
  link.href = URL.createObjectURL(blob);
  link.download = fileName;
  link.click();
}

function resetFileReception() {
  receivingFile = false;
  receivedFileName = '';
  receivedFileSize = 0;
  receivedFileData = [];
  receivedBytesCount = 0;
}


function resetFileReception() {
  receivingFile = false;
  receivedFileName = '';
  receivedFileSize = 0;
  receivedFileData = [];
  fileNameSize = 0;
  receivedBytesCount = 0;
}

async function enviarArquivo() {
  const arquivoInput = document.getElementById('file');
  if (!arquivoInput.files.length) {
    alert('Nenhum arquivo selecionado!');
    return;
  }

  const arquivo = arquivoInput.files[0];
  const tamanhoArquivo = arquivo.size;
  const nomeArquivo = arquivo.name;
  const tamanhoNomeArquivo = nomeArquivo.length;

  // Verifica se o arquivo excede 4 GB
  if (tamanhoArquivo > 0xFFFFFFFF) {
    alert('Erro: O arquivo é muito grande (limite de 4 GB).');
    return;
  }

  // Verifica se o tamanho do nome do arquivo cabe em 1 byte
  if (tamanhoNomeArquivo > 0xFF) {
    alert('Erro: Nome do arquivo muito grande (limite de 255 caracteres).');
    return;
  }

  // Prepara o cabeçalho:
  // 1 byte para o tamanho do username
  /// N bytes para o username
  // 1 byte para o tamanho da password
  // N bytes para a password
  // 1 byte para o comando (0x00)
  // 1 byte para o tamanho do nome do arquivo
  // N bytes para o nome do arquivo
  // 4 bytes para o tamanho do arquivo
  const username = localStorage.getItem('username');
  const password = localStorage.getItem('password');

  if (!username || !password) {
    alert('Credenciais de login não encontradas. Por favor, faça o login novamente.');
    window.location.href = '/login.html';
    return;
  }

  if (!nomeArquivo) {
    alert('Por favor, selecione um arquivo.');
    return;
  }

  // Codifica as informações no formato solicitado
  const usernameBytes = new TextEncoder().encode(username);
  const passwordBytes = new TextEncoder().encode(password);

  const header = new Uint8Array(
    5 + usernameBytes.length + passwordBytes.length + arquivoBytes.length
  );

  let offset = 0;

  // Adiciona o tamanho do username e o username
  header[offset++] = usernameBytes.length;
  usernameBytes.forEach((byte) => (header[offset++] = byte));

  // Adiciona o tamanho da password e a password
  header[offset++] = passwordBytes.length;
  passwordBytes.forEach((byte) => (header[offset++] = byte));

  // Byte de comando
  header[offset++] = 0x00;

  // Tamanho do nome do arquivo
  header[offset++] = tamanhoNomeArquivo;

  // Nome do arquivo (char por char)
  for (let i = 0; i < tamanhoNomeArquivo; i++) {
    header[offset++] = nomeArquivo.charCodeAt(i);
  }

  // Tamanho do arquivo (4 bytes)
  header[offset++] = (tamanhoArquivo >> 24) & 0xFF;
  header[offset++] = (tamanhoArquivo >> 16) & 0xFF;
  header[offset++] = (tamanhoArquivo >> 8) & 0xFF;
  header[offset++] = tamanhoArquivo & 0xFF;

  // Envia o cabeçalho
  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(header);
    console.log('Cabeçalho enviado:', header);

    // Transmite o arquivo byte a byte
    const reader = new FileReader();
    reader.onload = async (event) => {
      const fileBytes = new Uint8Array(event.target.result);
      for (let i = 0; i < fileBytes.length; i++) {
        ws.send(fileBytes[i]);
        await delay(1); // Pequeno atraso para evitar sobrecarregar a conexão
      }
      console.log('Arquivo enviado com sucesso!');
    };

    reader.onerror = () => {
      console.error('Erro ao ler o arquivo.');
    };

    reader.readAsArrayBuffer(arquivo); // Lê o arquivo como um buffer de bytes
  } else {
    alert('Erro: WebSocket não conectado.');
  }
}

function solicitarDownload() {
  const username = localStorage.getItem('username');
  const password = localStorage.getItem('password');
  const arquivoSelect = document.getElementById('arquivo');
  const nomeArquivo = arquivoSelect.value;

  if (!username || !password) {
    alert('Credenciais de login não encontradas. Por favor, faça o login novamente.');
    window.location.href = '/login.html';
    return;
  }

  if (!nomeArquivo) {
    alert('Por favor, selecione um arquivo.');
    return;
  }

  // Codifica as informações no formato solicitado
  const usernameBytes = new TextEncoder().encode(username);
  const passwordBytes = new TextEncoder().encode(password);
  const arquivoBytes = new TextEncoder().encode(nomeArquivo);

  const buffer = new Uint8Array(
    5 + usernameBytes.length + passwordBytes.length + arquivoBytes.length
  );

  let offset = 0;

  // Adiciona o tamanho do username e o username
  buffer[offset++] = usernameBytes.length;
  usernameBytes.forEach((byte) => (buffer[offset++] = byte));

  // Adiciona o tamanho da password e a password
  buffer[offset++] = passwordBytes.length;
  passwordBytes.forEach((byte) => (buffer[offset++] = byte));

  // Adiciona o comando (0x01 para solicitar download)
  buffer[offset++] = 0x01;

  // Adiciona o tamanho do nome do arquivo e o nome do arquivo
  buffer[offset++] = arquivoBytes.length;
  arquivoBytes.forEach((byte) => (buffer[offset++] = byte));

  // Envia os dados pelo WebSocket
  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(buffer);
    console.log('Solicitação de download enviada:', buffer);
  } else {
    alert('Erro: WebSocket não conectado.');
  }
}

// Função auxiliar para adicionar um atraso
function delay(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}