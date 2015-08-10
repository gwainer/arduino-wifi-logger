#include <SoftwareSerial.h>
#include <DHT.h>
#define DHTPIN 13
#define DHTTYPE DHT22
#define SSID "<YOUR_SSID>"
#define PASS "<PASS>"
#define THINGSPEAK_HOST "184.106.153.149"

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial console(8,9); //RX, TX 
String GET = "GET /update?key=<THINGSPEAK_KEY>";


/*
 Hey WairDuino! What do you feel?
*/
void setup() {
  Serial.begin(9600);
  console.begin(9600);  
  dht.begin();
  send_data("AT");
  delay(5000);
  if (Serial.find("OK")) {
    console.println("Alive and kicking.");
    connect_wifi(SSID, PASS);
  } else { 
    console.println("NO CARRIER");
  }
}

void loop() {
collect_send_env_data();
delay(60000);
}

void connect_wifi(String ssid, String pwd) {
  send_data("AT+CWJAP=\"" + ssid + "\",\"" + pwd +"\"");
  delay(10000);
  if (Serial.find("OK")) {
    console.println("Connected on " + ssid);
  } else {
    console.println("CATASTROPHIC FAILURE ON WIFI CONNECTION");
  }
}

void collect_send_env_data() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    console.println("Failed to read from DHT");
  } else {
    console.print("Humidity: "); 
    console.print(h);
    console.print(" %\t");
    console.print("Temperature: "); 
    console.print(t);
    console.println(" *C");
    update_env_data(t, h);
  }
}

void update_env_data(float temp, float humidity) {
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += THINGSPEAK_HOST;
  cmd += "\",80";
  Serial.println(cmd);
  delay(2000);
  console.println(Serial.read());
  if (Serial.find("Error")) {
    console.println("Error establishing ThingSpeak connection");
    connect_wifi(SSID, PASS);
    return;
  }
  cmd = GET;
  cmd += "&field1=" + String(temp);
  cmd += "&field2=" + String(humidity);
  cmd += "\r\n";
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if (Serial.find(">")) {
    console.print(cmd);
    Serial.print(cmd);
  } else {
    Serial.println("AT+CIPCLOSE");
  }
}

void send_data(String cmd) {
  console.print("SEND: ");
  console.println(cmd);
  Serial.println(cmd);
}
