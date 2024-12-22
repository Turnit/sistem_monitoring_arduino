#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Pin yang terhubung ke pin DATA DS18B20
#define ONE_WIRE_BUS D4  // GPIO4
#define RELAY1_PIN D5
#define RELAY2_PIN D1
#define RELAY3_PIN D6

// Data SSID dan password WiFi 
const char* ssid     = "Realme10";
const char* password = "1234567899";      

// Variabel untuk menyimpan status WiFi
int wifiStatus; 

// Inisialisasi komunikasi 1-Wire untuk DS18B20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Deklarasi Sensor pH
const int ph_Pin = A0; // Pin analog untuk membaca sensor pH
float nilaipH = 0;
float PH_step;
int nilai_analog_PH;
double TeganganPh;
float PH4 = 3.1;
float PH7 = 2.7;
float pHValue; // Menampung nilai pH

// untuk set pembacaan program asyncronus
unsigned long previousMillis = 0;
const long interval = 6000;


void setup() {
  Serial.begin(115200);
  delay(200);

  // Menampilkan informasi koneksi WiFi
  Serial.println();
  Serial.print("Mencoba terhubung ke WiFi: ");
  Serial.println(ssid);
    
  WiFi.begin(ssid, password);
  
  // Ketika WiFi.status tidak sama dengan WL_CONNECTED
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Setelah terkoneksi
  Serial.println("");
  Serial.println("ESP8266 sudah terhubung ke WiFi!");
  Serial.print("IP address ESP8266: ");
  Serial.println(WiFi.localIP());
  
  // Memulai sensor suhu DS18B20
  sensors.begin();

  pinMode(ph_Pin, INPUT); // Inisialisasi pH sebagai Input  


  // Atur pin relay sebagai output
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);

  // Pastikan relay dalam kondisi off saat awal
  digitalWrite(RELAY1_PIN, HIGH); // Relay aktif rendah (LOW untuk ON, HIGH 
  digitalWrite(RELAY2_PIN, HIGH); // Relay aktif rendah (LOW untuk ON, HIGH 
  digitalWrite(RELAY3_PIN, HIGH); // Relay aktif rendah (LOW untuk ON, HIGH 
}   

void loop() {
  wifiStatus = WiFi.status();
  pHValue = takepH(); 
  unsigned long currentMillis = millis();

  if (wifiStatus == WL_CONNECTED) {
    Serial.println("WiFi terkoneksi.");
  } else {
    Serial.println("WiFi tidak terkoneksi.");
  }

  // Tampilkan nilai pH
  Serial.print("Nilai pH: ");
  Serial.println(pHValue);


  // Meminta sensor untuk membaca suhu
  sensors.requestTemperatures();

  // Mendapatkan suhu dari sensor pertama (jika ada lebih dari satu sensor, sesuaikan ID)
  float suhuCelsius = sensors.getTempCByIndex(0);
  float suhuFahrenheit = sensors.toFahrenheit(suhuCelsius);

  // Menampilkan suhu ke Serial Monitor
  Serial.print("Suhu: ");
  Serial.print(suhuCelsius);
  Serial.print(" °C / ");
  Serial.print(suhuFahrenheit);
  Serial.println(" °F");


  if (pHValue <= 6.5 || pHValue >= 7.5) {
    if (currentMillis - previousMillis >= interval){
      previousMillis = currentMillis;
      digitalWrite(RELAY3_PIN, LOW);
      Serial.print("ph tidak normal");
    }
  } else {
    Serial.print("ph normal");
    digitalWrite(RELAY3_PIN, HIGH);
  } 

if (suhuCelsius < 18) {
    Serial.print("Suhu: ");
    Serial.print(suhuCelsius);
    Serial.println("°C - Status: Air Dingin");
    digitalWrite(RELAY1_PIN, HIGH);
    digitalWrite(RELAY2_PIN, LOW);
    delay(1000);
  } else if (suhuCelsius > 25) {
    Serial.print("Suhu: ");
    Serial.print(suhuCelsius);
    Serial.println("°C - Status: Air Panas");
    digitalWrite(RELAY2_PIN, HIGH);
    digitalWrite(RELAY1_PIN, LOW);
    delay(1000);
  } else {
    Serial.print("Suhu: ");
    Serial.print(suhuCelsius);
    Serial.println("°C - Status: Baik");
    digitalWrite(RELAY1_PIN, HIGH); // Relay aktif rendah (LOW untuk ON, HIGH 
    digitalWrite(RELAY2_PIN, HIGH); // Relay aktif rendah (LOW untuk ON, HI
  } 

  delay(1000); // Tunggu 1 detik sebelum pembacaan berikutnya
  
}


float takepH()
{                                       // Fungsi pengambilan pH dari sensor
  nilai_analog_PH = analogRead(ph_Pin); // Baca nilai analog dari sensor pH
  TeganganPh = 3.3 / 1024.0 * nilai_analog_PH;
  PH_step = (PH4 - PH7) / 3;
  nilaipH = 7.00 + ((PH7 - TeganganPh) / PH_step);
  return nilaipH;
}