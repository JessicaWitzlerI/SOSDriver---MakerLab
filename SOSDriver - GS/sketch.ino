#include <Wire.h>
#include <DHTesp.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>


const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";
const char* HOST = "unrigged-progeny-enroll.ngrok-free.dev";


const bool USAR_HTTPS = false;
const unsigned long INTERVALO_ENVIO = 8000;
unsigned long ultimoEnvio = 0;


#define LED_PIN       2
#define DHT_PIN       15
#define MQ2_PIN       34
#define TRIG_PIN      19
#define ECHO_PIN      18
#define OLED_SDA      21
#define OLED_SCL      22
#define BUZZER_PIN    4
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

DHTesp dht;

const int MQ2_ALERT = 2500;
const float TEMP_ALERT = 45.0;
const float TEMP_CRITICAL = 60.0;
const int DISTANCE_ALERT = 30;

 

void conectarWiFi() {

  Serial.print("Conectando ao Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASS, 6);
  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" conectado!");
  } else {
    Serial.println(" falhou (seguindo sem rede).");
  }
}

 

void enviarParaBackend(float temperatura, float umidade, int gas, float distancia) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Sem Wi-Fi, envio ignorado.");
    return;
  }

  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("DHT22 sem leitura, envio ignorado.");
    return;
  }


  float distEnvio = (distancia < 0) ? 400.0 : distancia;

  String json = "{";

  json += "\"temperatura\":" + String(temperatura, 1) + ",";
  json += "\"umidade\":" + String(umidade, 1) + ",";
  json += "\"gas\":" + String(gas) + ",";
  json += "\"distancia\":" + String(distEnvio, 1); 
  json += "}";

  IPAddress ip;
  bool dnsOk = WiFi.hostByName(HOST, ip);
  Serial.print("DNS: ");
  Serial.println(dnsOk ? ip.toString() : "falhou");

 

  String url = String(USAR_HTTPS ? "https://" : "http://") + HOST + "/sensores";
  HTTPClient http;
  http.setTimeout(20000);


  WiFiClientSecure clientSeguro;
  WiFiClient clientComum;


  if (USAR_HTTPS) {
    clientSeguro.setInsecure(); 
    http.begin(clientSeguro, url);
  } else {
    http.begin(clientComum, url);
  }

 
  http.addHeader("Content-Type", "application/json");
  http.addHeader("ngrok-skip-browser-warning", "true");

 
  Serial.println("URL: " + url);
  Serial.println("Enviando: " + json);
  int codigo = http.POST(json);


  if (codigo > 0) {
    Serial.print("Resposta HTTP ");
    Serial.println(codigo);
    Serial.println(http.getString());
  } else {
    Serial.print("Erro no envio: ");
    Serial.println(http.errorToString(codigo));
  }
  http.end();
}

 

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);


  dht.setup(DHT_PIN, DHTesp::DHT22);
  Wire.begin(OLED_SDA, OLED_SCL);

 

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Erro ao iniciar o OLED!");
    while (true) delay(100);
  }

 

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("SOSDriver SmartBot");
  display.println("Conectando Wi-Fi...");
  display.display();

  conectarWiFi();
  Serial.println("SOSDriver SmartBot iniciado!");

}

 

float lerDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duracao == 0) return -1;
  return duracao * 0.0343 / 2;
}

 

void loop() {
  TempAndHumidity dados = dht.getTempAndHumidity();
  float temperatura = dados.temperature;
  float umidade = dados.humidity;
  int gas = analogRead(MQ2_PIN);
  float distancia = lerDistancia();

  bool alertaTemperatura =
    !isnan(temperatura) && temperatura >= TEMP_ALERT;
  bool alertaGas = gas >= MQ2_ALERT;
  bool alertaObstaculo =
    distancia > 0 && distancia <= DISTANCE_ALERT;

  bool alerta = alertaTemperatura || alertaGas || alertaObstaculo;

 
  bool critico =
    (!isnan(temperatura) && temperatura >= TEMP_CRITICAL) ||
    gas >= 2600;
  digitalWrite(LED_PIN, alerta ? HIGH : LOW);

 
  if (critico) tone(BUZZER_PIN, 2000);
  else noTone(BUZZER_PIN);


  Serial.println("===== SOSDRIVER SMARTBOT =====");
  Serial.print("Temperatura: "); Serial.print(temperatura); Serial.println(" C");
  Serial.print("Umidade: "); Serial.print(umidade); Serial.println(" %");
  Serial.print("Nivel MQ-2: "); Serial.println(gas);
  Serial.print("Distancia: ");

  if (distancia < 0) Serial.println("Sem leitura");
  else { Serial.print(distancia); Serial.println(" cm"); }
  Serial.print("Situacao: "); Serial.println(alerta ? "ALERTA" : "NORMAL");


  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("SOSDriver SmartBot");
  display.println("----------------");
  display.print("Temp: ");

  if (isnan(temperatura)) display.println("--");
  else { display.print(temperatura, 1); display.println(" C"); }
  display.print("Umidade: ");
  if (isnan(umidade)) display.println("--");
  else { display.print(umidade, 1); display.println(" %"); }
  display.print("Gas MQ2: "); display.println(gas);
  display.print("Distancia: ");
  if (distancia < 0) display.println("--");
  else { display.print(distancia, 1); display.println(" cm"); }
  display.println("----------------");
  if (critico) display.println("RISCO CRITICO!");
  else if (alerta) display.println("ATENCAO: ALERTA");
  else display.println("Situacao: NORMAL");
  display.display();


  if (millis() - ultimoEnvio >= INTERVALO_ENVIO) {
    ultimoEnvio = millis();
    enviarParaBackend(temperatura, umidade, gas, distancia);
  }
  delay(1500);
}