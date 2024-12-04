
document.getElementById('connection-form').addEventListener('submit', function(event) {
    event.preventDefault();
    const ngroknurl = document.getElementById('ngroknurl').value.trim();
    const connurl = ngroknurl.replace(/^https?:/, 'wss:') + '/ws';
    console.log('URL do WebSocket:', connurl);

    // Salva a URL do WebSocket no localStorage
    localStorage.setItem('wsUrl', connurl);

    // Redireciona para a página de login após salvar a URL
    window.location.href = './html/login.html';
});
