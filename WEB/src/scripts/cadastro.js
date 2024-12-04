let ws;

// Recupera a URL do WebSocket salva no localStorage
const wsUrl = localStorage.getItem('wsUrl');
if (!wsUrl) {
  alert('URL do WebSocket não configurada. Redirecionando para a página inicial.');
  window.location.href = './index.html';
} else {
  console.log('Conectando ao WebSocket:', wsUrl);
  ws = new WebSocket(wsUrl);

  ws.onopen = () => {
    console.log('WebSocket conectado');
  };

  ws.onmessage = (event) => {
    console.log('Mensagem recebida da ESP32:', event.data);

    // Processar a resposta do cadastro
    if (event.data === 'Cadastro bem-sucedido') {
      alert('Cadastro realizado com sucesso!');
      window.location.href = '/login.html'; // Redireciona para a página de login
    } else if (event.data === 'Cadastro falhou') {
      alert('Falha no cadastro. Tente outro nome de usuário.');
    }
  };

  ws.onerror = (error) => {
    console.error('Erro no WebSocket:', error);
  };

  ws.onclose = () => {
    console.log('WebSocket desconectado');
  };
}

function encodeRegisterData(username, password) {
  // Cria um array com espaço suficiente para armazenar os dados
  const usernameBytes = new TextEncoder().encode(username);
  const passwordBytes = new TextEncoder().encode(password);
  const buffer = new Uint8Array(2 + usernameBytes.length + passwordBytes.length);
    
  // Preenche o buffer
  let offset = 0;

  // Adiciona o byte de comando
  buffer[offset++] = 3; // Byte de comando igual a 2
    
  // Adiciona o tamanho do username (1 byte)
  buffer[offset++] = usernameBytes.length;
    
  // Adiciona os bytes do username
  usernameBytes.forEach((byte) => {
    buffer[offset++] = byte;
  });

  // Adiciona o tamanho da password (1 byte)
  buffer[offset++] = passwordBytes.length;

  // Adiciona os bytes da password
  passwordBytes.forEach((byte) => {
    buffer[offset++] = byte;
  });

  return buffer;
}

// Envia os dados de cadastro ao WebSocket
document.getElementById('register-form').addEventListener('submit', function(event) {
  event.preventDefault();
  const username = document.getElementById('username').value;
  const password = document.getElementById('password').value;

  // Codifica os dados no formato binário descrito
  const encodedData = encodeRegisterData(username, password);

  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(encodedData);
    console.log('Dados enviados ao WebSocket:', encodedData);
  } else {
    alert('Erro: WebSocket não conectado.');
  }
});