#include <WiFi.h>
#include <WebServer.h>

/* Configuração de pinos para controle de motores */
#define IN1 26
#define IN2 27
#define IN3 14
#define IN4 13
#define ENA 25
#define ENB 33

/* Configuração de pinos de saída */
#define OUT_PIN1 18
#define OUT_PIN2 19
#define OUT_PIN3 21

/* Configuração de PWM */
#define PWM_FREQ 1000
#define PWM_RESOLUTION 8
#define PWM_CHANNEL_A 0
#define PWM_CHANNEL_B 1

// Credenciais de Wi-Fi
const char* ssid = "iPhone de Delian";
const char* password = "delian2023";

// Inicialização do servidor web
WebServer server(80);

// Variáveis de controle
bool motoresLigados = false;
bool virandoEsquerda = false;
bool virandoDireita = false;
bool saidasAtivadas = false;

// Função para parar os motores
void pararMotores() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  ledcWrite(PWM_CHANNEL_A, 0);
  ledcWrite(PWM_CHANNEL_B, 0);
}

// Função para acionar os motores
void andarFrente() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(PWM_CHANNEL_A, 255);
  ledcWrite(PWM_CHANNEL_B, 255);
}

void virarEsquerda() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(PWM_CHANNEL_A, 255);
  ledcWrite(PWM_CHANNEL_B, 255);
  delay(1000);
  pararMotores();
}

void virarDireita() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  ledcWrite(PWM_CHANNEL_A, 255);
  ledcWrite(PWM_CHANNEL_B, 255);
  delay(1000);
  pararMotores();
}

// Funções para ativar/desativar saídas
void ativarSaidas() {
  digitalWrite(OUT_PIN1, LOW);
  digitalWrite(OUT_PIN2, LOW);
  digitalWrite(OUT_PIN3, LOW);
  saidasAtivadas = true;
}

void desativarSaidas() {
  digitalWrite(OUT_PIN1, HIGH);
  digitalWrite(OUT_PIN2, HIGH);
  digitalWrite(OUT_PIN3, HIGH);
  saidasAtivadas = false;
}

// Página HTML para controle
String paginaControle() {
  String html = "<html><head><title>Controle do Robo</title></head><body>";
  html += "<h1>Controle do Robo</h1>";
  html += "<button onclick=\"location.href='/ligarMotores'\">Ligar Motores</button>";
  html += "<button onclick=\"location.href='/desligarMotores'\">Desligar Motores</button><br><br>";
  html += "<button onclick=\"location.href='/virarEsquerda'\">Virar Esquerda</button>";
  html += "<button onclick=\"location.href='/virarDireita'\">Virar Direita</button><br><br>";
  html += "<button onclick=\"location.href='/ativarSaidas'\">Ativar Saidas</button>";
  html += "<button onclick=\"location.href='/desativarSaidas'\">Desativar Saidas</button>";
  html += "</body></html>";
  return html;
}

// Configurações de rotas do servidor
void setupServidor() {
  server.on("/", []() {
    server.send(200, "text/html", paginaControle());
  });

  server.on("/ligarMotores", []() {
    andarFrente();
    motoresLigados = true;
    server.send(200, "text/html", paginaControle());
  });

  server.on("/desligarMotores", []() {
    pararMotores();
    motoresLigados = false;
    server.send(200, "text/html", paginaControle());
  });

  server.on("/virarEsquerda", []() {
    virarEsquerda();
    server.send(200, "text/html", paginaControle());
  });

  server.on("/virarDireita", []() {
    virarDireita();
    server.send(200, "text/html", paginaControle());
  });

  server.on("/ativarSaidas", []() {
    ativarSaidas();
    server.send(200, "text/html", paginaControle());
  });

  server.on("/desativarSaidas", []() {
    desativarSaidas();
    server.send(200, "text/html", paginaControle());
  });

  server.begin();
}

// Função de inicialização
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi!");
  Serial.println(WiFi.localIP());

  // Configuração dos pinos
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(OUT_PIN1, OUTPUT);
  pinMode(OUT_PIN2, OUTPUT);
  pinMode(OUT_PIN3, OUTPUT);
  ledcSetup(PWM_CHANNEL_A, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_B, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(ENA, PWM_CHANNEL_A);
  ledcAttachPin(ENB, PWM_CHANNEL_B);

  setupServidor();
}

void loop() {
  server.handleClient();
}
