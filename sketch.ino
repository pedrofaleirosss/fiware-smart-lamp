// -----------------------------------------------------------------------------
// Projeto: PoC Smart Lamp - FIWARE + ESP32 + LDR (Simulação Wokwi)
// Autores:
//   Pedro Alves Faleiros - 562523
//   Luan Felix - 565541
//   João Lopes - 565737
//   Leandro Farias - 566488
// -----------------------------------------------------------------------------
// Descrição: 
// Este código conecta o ESP32 a uma rede Wi-Fi, estabelece comunicação com um 
// broker MQTT (integrado ao FIWARE), lê valores de luminosidade através de um 
// sensor LDR (simulado no pino 34) e envia essas leituras para a nuvem. 
// Além disso, recebe comandos via MQTT para ligar/desligar 
// o LED onboard do ESP32.
// -----------------------------------------------------------------------------

#include <WiFi.h>
#include <PubSubClient.h>

// --------------------------- CONFIGURAÇÕES -----------------------------------
// Rede Wi-Fi
const char* default_SSID = "Wokwi-GUEST"; // Nome da rede Wi-Fi
const char* default_PASSWORD = "";        // Senha da rede Wi-Fi

// Broker MQTT (IP do FIWARE no Azure)
const char* default_BROKER_MQTT = "102.37.156.74"; // IP do Broker MQTT
const int default_BROKER_PORT = 1883;             // Porta padrão MQTT

// Tópicos MQTT
const char* default_TOPICO_SUBSCRIBE = "/TEF/lamp001/cmd";     // Para ouvir comandos (ex.: ligar/desligar LED)
const char* default_TOPICO_PUBLISH_1 = "/TEF/lamp001/attrs";   // Para enviar estado do LED
const char* default_TOPICO_PUBLISH_2 = "/TEF/lamp001/attrs/l"; // Para enviar valores de luminosidade

// Identificação
const char* default_ID_MQTT = "fiware_001"; // ID do cliente MQTT
const int default_D4 = 2;                   // LED onboard do ESP32 (GPIO 2)

// Prefixo usado na formatação das mensagens de comando
const char* topicPrefix = "lamp001";

// ---------------------- VARIÁVEIS EDITÁVEIS EM TEMPO DE EXECUÇÃO -------------
char* SSID = const_cast<char*>(default_SSID);
char* PASSWORD = const_cast<char*>(default_PASSWORD);
char* BROKER_MQTT = const_cast<char*>(default_BROKER_MQTT);
int BROKER_PORT = default_BROKER_PORT;
char* TOPICO_SUBSCRIBE = const_cast<char*>(default_TOPICO_SUBSCRIBE);
char* TOPICO_PUBLISH_1 = const_cast<char*>(default_TOPICO_PUBLISH_1);
char* TOPICO_PUBLISH_2 = const_cast<char*>(default_TOPICO_PUBLISH_2);
char* ID_MQTT = const_cast<char*>(default_ID_MQTT);
int D4 = default_D4;

// Objetos do Wi-Fi e MQTT
WiFiClient espClient;
PubSubClient MQTT(espClient);

// Estado atual do LED
char EstadoSaida = '0';

// --------------------------- FUNÇÕES AUXILIARES ------------------------------
void initSerial() {
    Serial.begin(115200); // Inicia a comunicação serial para debug
}

void initWiFi() {
    delay(10);
    Serial.println("------ Conexao Wi-Fi ------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    reconectWiFi(); // Conecta ao Wi-Fi
}

void initMQTT() {
    MQTT.setServer(BROKER_MQTT, BROKER_PORT); // Define servidor e porta
    MQTT.setCallback(mqtt_callback);          // Define função callback para receber mensagens
}

// --------------------------- SETUP -------------------------------------------
void setup() {
    InitOutput();   // Inicializa LED onboard piscando
    initSerial();   // Inicializa serial
    initWiFi();     // Conecta ao Wi-Fi
    initMQTT();     // Conecta ao broker MQTT
    delay(5000);

    // Publica mensagem inicial (LED ligado como status de vida do device)
    MQTT.publish(TOPICO_PUBLISH_1, "s|on");
}

// --------------------------- LOOP PRINCIPAL ----------------------------------
void loop() {
    VerificaConexoesWiFIEMQTT(); // Garante conexões ativas
    EnviaEstadoOutputMQTT();     // Envia estado atual do LED
    handleLuminosity();          // Lê e envia dados de luminosidade
    MQTT.loop();                 // Mantém cliente MQTT rodando
}

// --------------------------- GERENCIAMENTO WI-FI -----------------------------
void reconectWiFi() {
    if (WiFi.status() == WL_CONNECTED) return;

    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    Serial.println("\nConectado ao Wi-Fi!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    // Garante que o LED onboard inicie desligado
    digitalWrite(D4, LOW);
}

// --------------------------- CALLBACK MQTT -----------------------------------
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (int i = 0; i < length; i++) {
        msg += (char)payload[i]; // Monta string recebida
    }

    Serial.print("- Mensagem recebida: ");
    Serial.println(msg);

    // Formata padrões esperados para ligar/desligar LED
    String onTopic = String(topicPrefix) + "@on|";
    String offTopic = String(topicPrefix) + "@off|";

    // Se for comando de ligar
    if (msg.equals(onTopic)) {
        digitalWrite(D4, HIGH);
        EstadoSaida = '1';
    }

    // Se for comando de desligar
    if (msg.equals(offTopic)) {
        digitalWrite(D4, LOW);
        EstadoSaida = '0';
    }
}

// --------------------------- VERIFICA CONEXÕES -------------------------------
void VerificaConexoesWiFIEMQTT() {
    if (!MQTT.connected()) reconnectMQTT();
    reconectWiFi();
}

// --------------------------- ENVIO DE ESTADO ---------------------------------
void EnviaEstadoOutputMQTT() {
    if (EstadoSaida == '1') {
        MQTT.publish(TOPICO_PUBLISH_1, "s|on");
        Serial.println("- Led Ligado");
    } else {
        MQTT.publish(TOPICO_PUBLISH_1, "s|off");
        Serial.println("- Led Desligado");
    }
    Serial.println("- Estado do LED enviado ao broker!");
    delay(1000);
}

// --------------------------- INICIALIZA LED ----------------------------------
void InitOutput() {
    pinMode(D4, OUTPUT);

    // Pisca 10 vezes no início para indicar boot
    boolean toggle = false;
    for (int i = 0; i <= 10; i++) {
        toggle = !toggle;
        digitalWrite(D4, toggle);
        delay(200);
    }
    digitalWrite(D4, LOW); // Garante que termine desligado
}

// --------------------------- RECONEXÃO MQTT ----------------------------------
void reconnectMQTT() {
    while (!MQTT.connected()) {
        Serial.print("* Tentando conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);

        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); // Inscreve no tópico de comandos
        } else {
            Serial.println("Falha ao conectar. Nova tentativa em 2s...");
            delay(2000);
        }
    }
}

// --------------------------- LEITURA LUMINOSIDADE ----------------------------
void handleLuminosity() {
    const int potPin = 34; // Pino analógico do LDR
    int sensorValue = analogRead(potPin); 
    int luminosity = map(sensorValue, 0, 4095, 0, 100); // Converte valor para escala 0-100%

    // Converte para string e envia via MQTT
    String mensagem = String(luminosity);
    Serial.print("Valor da luminosidade: ");
    Serial.println(mensagem.c_str());
    MQTT.publish(TOPICO_PUBLISH_2, mensagem.c_str());
}
