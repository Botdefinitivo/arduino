#include <ESP8266WiFi.h>
const char* ssid     = "xxxxxx";                    // Nombre de la red Wifi
const char* password = "xxxxxx";                    // Password
const char* host = "192.168.1.110";                 // Direccion IP de nuestro servidor
const int   port = 80;                              // Puerto, 80
const int   watchdog = 5000;                        // Frecuencia del watchdog, valor por defecto
unsigned long previousMillis = millis(); 

#include "DHT.h"                                    // Se incluye la libreria del sensor 
#define DHTTYPE DHT11                               // DHT 11

#define dht_dpin 0                                  //Definimos el pin al cual se conecta 
DHT dht(dht_dpin, DHTTYPE); 
void setup(void)
{
   WiFi.begin(ssid, password);                      
  while (WiFi.status() != WL_CONNECTED) {          //While mientras conecta
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");                 //Avisa si conecta el wifi
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
    dht.begin();
  Serial.begin(9600);
  Serial.println("Humedad y Temperatura\n\n");
  delay(700);
  
}
void loop() {                                       //Lectura del sensor en loop
    float h = dht.readHumidity();
    float t = dht.readTemperature();         
    Serial.print("Humedad = ");
    Serial.print(h);
    Serial.print("%  ");
    Serial.print("Temperatura = ");
    Serial.print(t); 
    Serial.println("°C  ");

    unsigned long currentMillis = millis();          //Enviamos al servidor de aqui en adelante
    
  if ( currentMillis - previousMillis > watchdog ) {
    previousMillis = currentMillis;
    WiFiClient client;
  
    if (!client.connect(host, port)) {
      Serial.println("Fallo al conectar");
      return;
    }

    String url = "/ESP8266DHT11/index.php?temp=";    //construimos nuestra URL
    url += t;                                        //Concatenamos para enviar por GET
    url += "&hum=";
    url += h;
    
       // Enviamos petición al servidor
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
  
    // Leemos la respuesta del servidor
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }
  
   delay(800);// tiempo en el que ocurre la lectura
}
