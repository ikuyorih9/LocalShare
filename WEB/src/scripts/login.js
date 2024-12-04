let ws;
let username;
let password;

//Recupera a URL do WebSocket salva no localStorage
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
        console.log('Mensagem recebida da ESP32:', event.data);

        if (event.data.startsWith('Login bem-sucedido')) {
            // Separa a mensagem recebida
            const [status, arquivos] = event.data.split(':');
            
            // Salva o nome de usuário e senha no localStorage
            localStorage.setItem('username', username);
            localStorage.setItem('password', password);

            // Salva a lista de arquivos no localStorage
            if (arquivos) {
                const listaArquivos = arquivos.split('|').filter(Boolean); // Filtra valores vazios
                localStorage.setItem('fileList', JSON.stringify(listaArquivos));
                console.log('Lista de arquivos recebida e salva:', listaArquivos);
            }

            // Redireciona para a próxima página
            window.location.href = './main-page.html';
        } else if (event.data === 'Login falhou') {
            alert('Credenciais inválidas. Tente novamente.');
        }
    };

    ws.onerror = (error) => {
        console.error('Erro no WebSocket:', error);
    };

    ws.onclose = () => {
        console.log('WebSocket desconectado');
    };
}

// //testando com python
// const wsUrl = 'ws://127.0.0.1:8080'; // Substitua pelo IP e porta locais do servidor WebSocket
// const ws = new WebSocket(wsUrl);

// ws.onopen = () => {
//   console.log('WebSocket conectado');
// };

// ws.onmessage = (event) => {
//   console.log('Mensagem recebida do servidor:', event.data);

//   if (event.data === 'Login bem-sucedido') {
//     window.location.href = './main-page.html';
//   } else if (event.data === 'Login falhou') {
//     alert('Credenciais inválidas. Tente novamente.');
//   }
// };

// ws.onerror = (error) => {
//   console.error('Erro no WebSocket:', error);
// };

// ws.onclose = () => {
//   console.log('WebSocket desconectado');
// };


function encodeLoginData(username, password) {
    // Codifica o username e a password em bytes
    const usernameBytes = new TextEncoder().encode(username);
    const passwordBytes = new TextEncoder().encode(password);

    // Cria um buffer com espaço adicional para o byte de comando
    const buffer = new Uint8Array(3 + usernameBytes.length + passwordBytes.length);

    // Preenche o buffer
    let offset = 0;

    // Adiciona o byte de comando
    buffer[offset++] = 2; // Byte de comando igual a 3

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

// Envia os dados de login ao WebSocket
document.getElementById('login-form').addEventListener('submit', function (event) {
    event.preventDefault();
    username = document.getElementById('username').value;
    password = document.getElementById('password').value;

    // Codifica os dados no formato binário descrito
    const encodedData = encodeLoginData(username, password);

    if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(encodedData);
        console.log('Dados enviados ao WebSocket:', encodedData);
    } else {
        alert('Erro: WebSocket não conectado.');
    }
});