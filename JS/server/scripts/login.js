const { ipcRenderer } = require('electron');

document.getElementById('loginForm').addEventListener('submit', (e) => {
  e.preventDefault();
  const ip = document.getElementById('ip').value;
  const port = document.getElementById('port').value;
  const username = document.getElementById('username').value;
  const password = document.getElementById('password').value;
  console.log(`Enviando IP: ${ip} e Porta: ${port} para iniciar o servidor`);
  ipcRenderer.send('start-server', ip, parseInt(port), username, password);
});