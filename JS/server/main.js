const { app, BrowserWindow, ipcMain } = require('electron');
const WebSocket = require('ws');
const path = require('path');

let mainWindow;
let wsServer;
let connectedClients = 0;

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
}

app.on('ready', createWindow);

ipcMain.on('start-server', (event, ip, port, user, passwor) => {
  console.log(`Iniciando servidor em ${ip}:${port}`);
  wsServer = new WebSocket.Server({ port: port, host: ip });

  wsServer.on('connection', (socket) => {
    console.log('Cliente conectado');
    connectedClients++;
    mainWindow.webContents.send('client-connected', connectedClients);
    socket.on('message', (message) => {
      console.log('Mensagem recebida: ' + message);
      socket.send('Mensagem recebida: ' + message);
    });
    socket.on('close', () => {
      console.log('Cliente desconectado');
      connectedClients--;
      mainWindow.webContents.send('client-disconnected', connectedClients);
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
  // Lógica para enviar arquivo para o cliente conectado
  console.log(`Enviando arquivo: ${filepath}`);
});
