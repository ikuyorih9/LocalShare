const { ipcRenderer } = require('electron');

ipcRenderer.on('client-connected', (event, count) => {
  document.getElementById('clientStatus').textContent = `Clientes conectados: ${count}`;
});

ipcRenderer.on('client-disconnected', (event, count) => {
  document.getElementById('clientStatus').textContent = `Clientes conectados: ${count}`;
});

document.getElementById('sendButton').addEventListener('click', () => {
  const fileInput = document.getElementById('fileInput');
  if (fileInput.files.length > 0) {
    const filePath = fileInput.files[0].path;
    console.log(`Selecionado para envio: ${filePath}`); // Adicionar log para ver o valor do caminho do arquivo
    ipcRenderer.send('send-file', filePath);
  } else {
    alert('Por favor, selecione um arquivo primeiro.');
  }
});
