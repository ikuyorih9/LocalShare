#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>

// Configurações da rede Wi-Fi
const char* ssid = "LabMicros"; // Substitua pelo nome da sua rede Wi-Fi
const char* password = "seluspeesc@"; // Substitua pela senha da sua rede Wi-Fi

// Configuração do I2C
#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_ADDRESS 0x08

// Instancia o servidor web
AsyncWebServer server(80);

// Declaração das funções
void webServerTask(void *param);
void i2cTask(void *param);
String generateFileOptions();
void handleLogin(AsyncWebServerRequest *request);
void handleCadastro(AsyncWebServerRequest *request);
void handleDownload(AsyncWebServerRequest *request);
void handleUpload(AsyncWebServerRequest *request);

// Lista estática de arquivos para download
const char* arquivos[] = {"arquivo1.txt", "arquivo2.pdf", "arquivo3.c"};
const int num_arquivos = sizeof(arquivos) / sizeof(arquivos[0]);

// Página HTML para Login
const char login_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Login</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; }
    .container { margin: auto; padding: 20px; width: 50%; }
    input[type="text"], input[type="password"], input[type="submit"] { margin: 10px; padding: 10px; width: 80%; }
    a { text-decoration: none; color: blue; }
  </style>
</head>
<body>
  <div class="container">
    <h2>Login</h2>
    <form action="/login" method="post">
      <input type="text" name="username" placeholder="Nome de usuário" required><br>
      <input type="password" name="password" placeholder="Senha" required><br>
      <input type="submit" value="Entrar">
    </form>
    <a href="/cadastro">Inscreva-se</a>
  </div>
</body>
</html>
)rawliteral";

// Página HTML para Cadastro
const char cadastro_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Cadastro</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; }
    .container { margin: auto; padding: 20px; width: 50%; }
    input[type="text"], input[type="password"], input[type="submit"] { margin: 10px; padding: 10px; width: 80%; }
    a { text-decoration: none; color: blue; }
  </style>
</head>
<body>
  <div class="container">
    <h2>Cadastro</h2>
    <form action="/register" method="post">
      <input type="text" name="username" placeholder="Nome de usuário" required><br>
      <input type="password" name="password" placeholder="Senha" required><br>
      <input type="submit" value="Cadastrar">
    </form>
    <a href="/">Voltar para Login</a>
  </div>
</body>
</html>
)rawliteral";

// Página HTML para o Dashboard com Upload e Download
const char dashboard_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Dashboard</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; }
    .container { margin: auto; padding: 20px; width: 50%; }
    .section { display: none; }
    .active { display: block; }
    button { padding: 10px 20px; margin: 5px; }
    select, input[type="file"] { margin: 10px; padding: 10px; }
  </style>
  <script>
    function toggleSection(section) {
      document.getElementById('download-section').classList.remove('active');
      document.getElementById('upload-section').classList.remove('active');
      document.getElementById(section).classList.add('active');
    }
  </script>
</head>
<body>
  <div class="container">
    <h2>Bem-vindo ao Dashboard!</h2>
    <button onclick="toggleSection('download-section')">Download</button>
    <button onclick="toggleSection('upload-section')">Upload</button>

    <div id="download-section" class="section active">
      <h3>Download de Arquivos</h3>
      <form action="/download" method="post">
        <label for="arquivo">Escolha um arquivo:</label>
        <select name="arquivo" id="arquivo">
          %OPTIONS%
        </select><br>
        <input type="submit" value="Baixar">
      </form>
    </div>

    <div id="upload-section" class="section">
      <h3>Upload de Arquivos</h3>
      <form action="/upload" method="post" enctype="multipart/form-data">
        <label for="file">Selecione um arquivo para enviar:</label>
        <input type="file" name="file" id="file"><br>
        <input type="submit" value="Enviar">
      </form>
    </div>
  </div>
</body>
</html>
)rawliteral";

// Função para gerar opções dinâmicas
String generateFileOptions() {
  const char* arquivos[] = {"arquivo1.txt", "arquivo2.pdf", "arquivo3.c"};
  const int num_arquivos = sizeof(arquivos) / sizeof(arquivos[0]);
  
  String options = "";
  for (int i = 0; i < num_arquivos; i++) {
    options += "<option value='" + String(arquivos[i]) + "'>" + String(arquivos[i]) + "</option>";
  }
  return options;
}

// Função para processar login
void handleLogin(AsyncWebServerRequest *request) {
  String username = "";
  String password = "";

  if (request->hasParam("username", true)) {
    username = request->getParam("username", true)->value();
  }
  if (request->hasParam("password", true)) {
    password = request->getParam("password", true)->value();
  }

  Serial.println("Tentativa de Login:");
  Serial.println("Username: " + username);
  Serial.println("Password: " + password);

  // Validação fictícia - substitua pela lógica real
  if (username == "admin" && password == "1234") {
    request->redirect("/dashboard");
  } else {
    request->send(401, "text/plain", "Login inválido. Tente novamente.");
  }
}

// Função para processar cadastro
void handleCadastro(AsyncWebServerRequest *request) {
  String username = "";
  String password = "";

  if (request->hasParam("username", true)) {
    username = request->getParam("username", true)->value();
  }
  if (request->hasParam("password", true)) {
    password = request->getParam("password", true)->value();
  }

  Serial.println("Novo Cadastro:");
  Serial.println("Username: " + username);
  Serial.println("Password: " + password);

  request->redirect("/");
}

// Função para processar download
void handleDownload(AsyncWebServerRequest *request) {
  if (request->hasParam("arquivo", true)) {
    String arquivo = request->getParam("arquivo", true)->value();
    Serial.println("Fazendo download do arquivo: " + arquivo);
    request->send(200, "text/plain", "Download do arquivo " + arquivo + " iniciado.");
  } else {
    request->send(400, "text/plain", "Nenhum arquivo selecionado.");
  }
}

// Função para processar upload
void handleUpload(AsyncWebServerRequest *request) {
  Serial.println("Fazendo upload do arquivo (simulado).");
  request->send(200, "text/plain", "Upload do arquivo recebido.");
}

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(115200);

  // Inicializa as tarefas FreeRTOS
  xTaskCreatePinnedToCore(
    webServerTask,    // Função da tarefa
    "WebServerTask",  // Nome da tarefa
    10000,            // Tamanho da pilha
    NULL,             // Parâmetro
    1,                // Prioridade
    NULL,             // Handle
    1                 // Núcleo 1
  );

  xTaskCreatePinnedToCore(
    i2cTask,          // Função da tarefa
    "I2CTask",        // Nome da tarefa
    10000,            // Tamanho da pilha
    NULL,             // Parâmetro
    1,                // Prioridade
    NULL,             // Handle
    0                 // Núcleo 0
  );
}

void loop() {
  // Nada a ser feito aqui, tudo está nas tarefas
}

// Função da tarefa do servidor web
void webServerTask(void *param) {
  // Conecta ao Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Conectando ao Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConectado ao Wi-Fi!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Configuração do servidor web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", login_html);
  });

  server.on("/cadastro", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", cadastro_html);
  });

  server.on("/dashboard", HTTP_GET, [](AsyncWebServerRequest *request) {
    String page = String(dashboard_html);
    page.replace("%OPTIONS%", generateFileOptions());
    request->send(200, "text/html", page);
  });

  server.on("/login", HTTP_POST, handleLogin);
  server.on("/register", HTTP_POST, handleCadastro);
  server.on("/download", HTTP_POST, handleDownload);
  server.on("/upload", HTTP_POST, handleUpload);

  server.begin();
  Serial.println("Servidor web iniciado no Núcleo 1");

  while (true) {
    delay(1000);
  }
}

// Função da tarefa do I2C
void i2cTask(void *param) {
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.println("I2C inicializado no Núcleo 0");

  while (true) {
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write(reinterpret_cast<const uint8_t*>("Comando do Núcleo 0"), strlen("Comando do Núcleo 0"));
    Wire.endTransmission();

    Wire.requestFrom(I2C_ADDRESS, 10);
    while (Wire.available()) {
      char c = Wire.read();
      Serial.print(c);
    }
    Serial.println();

    delay(1000);
  }
}
