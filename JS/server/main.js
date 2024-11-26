const { app, BrowserWindow, ipcMain } = require('electron');
const WebSocket = require('ws');
const path = require('path');
const fs = require('fs');

let mainWindow;
let wsServer;
let connectedClients = [];
let userCredentials = {};

const createWindow = () => {
  mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      preload: path.join(__dirname, './scripts/preload.js'),
      nodeIntegration: true,
      contextIsolation: false
    }
  });

  mainWindow.loadFile('./html/index.html');
};

app.on('ready', createWindow);

ipcMain.on('start-server', (event, ip, port, username, password) => {
  console.log(`Iniciando servidor em ${ip}:${port}`);
  console.log(`Username: ${username}, Password: ${password}`);

  // Armazenar credenciais de usuário
  userCredentials = { username, password };

  wsServer = new WebSocket.Server({ port: port, host: ip });

  wsServer.on('connection', (socket) => {
    console.log('Cliente conectado');
    connectedClients.push(socket);
    mainWindow.webContents.send('client-connected', connectedClients.length);

    socket.on('close', () => {
      console.log('Cliente desconectado');
      connectedClients = connectedClients.filter(client => client !== socket);
      mainWindow.webContents.send('client-disconnected', connectedClients.length);
    });
  });

  wsServer.on('listening', () => {
    console.log(`Servidor WebSocket está rodando em ${ip}:${port}`);
    mainWindow.loadFile('./html/server.html');
  });

  wsServer.on('error', (error) => {
    console.error('Erro no servidor WebSocket:', error);
  });
});

ipcMain.on('send-file', (event, filepath) => {
  console.log(`Recebido caminho do arquivo para envio: ${filepath}`); // Log para verificar o valor do caminho do arquivo
  if (!filepath || !fs.existsSync(filepath)) {
    console.error('Arquivo não encontrado ou caminho inválido');
    return;
  }

  if (connectedClients.length === 0) {
    console.log('Nenhum cliente conectado.');
    return;
  }

  const client = connectedClients[0]; // Enviar para o primeiro cliente conectado

  try {
    const { username, password } = userCredentials;

    // Enviar metadados do arquivo
    const filename = path.basename(filepath);
    const fileSize = fs.statSync(filepath).size;
    const metadata = `${username.length},${username},${password.length},${password},${filename.length},${filename},${fileSize}`;

    console.log(`Enviando metadados: ${metadata}`);
    client.send(metadata);

    // Enviar o arquivo
    const stream = fs.createReadStream(filepath);
    stream.on('data', (chunk) => {
      console.log('Enviando chunk de dados...');
      client.send(chunk);
    });
    stream.on('end', () => {
      console.log('Envio do arquivo concluído.');
      client.send(Buffer.from([0])); // Indicar o final da transferência
    });
  } catch (error) {
    console.error('Erro ao enviar arquivo:', error);
  }
});
