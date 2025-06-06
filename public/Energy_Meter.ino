#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>  
#include <ESP_Mail_Client.h>

// WiFi & MQTT Credentials
#define SMTP_HOST "smtp.gmail.com"
#define AUTHOR_EMAIL "espdev32@gmail.com"
#define AUTHOR_PASSWORD "jwytkigbsbfobyno"
#define RECIPIENT_EMAIL "hprasanth430@gmail.com"
#define SMTP_PORT 465

#define ANALOG_IN_PIN 35 
#define CURRENT_PIN 0

#define FAN1 14
#define FAN2 16
#define LIGHT1 18
#define LIGHT2 22

// Sensor 1
#define TRIG1_PIN 5
#define ECHO1_PIN 4
#define LED1_PIN 2

// Sensor 2
#define TRIG2_PIN 17
#define ECHO2_PIN 19
#define LED2_PIN 15

#define REF_VOLTAGE    3.3
#define ADC_RESOLUTION 4096.0

#define Voltage_R1 30000.0
#define Voltage_R2 7500.0

#define Current_R1 6800.0
#define Current_R2 12000.0

const char* ssid = "iQOO Neo 10R";
const char* password = "Hari2003";

const char* mqtt_server = "1fae4ab464e64fe9be19c16c1101c1be.s1.eu.hivemq.cloud";
const char* mqtt_user = "Check";
const char* mqtt_password = "2025Black";

const char* mqtt_Subtopic[] = {"Led1", "Led2", "Led3", "Led4", "seet_value"};
const char* mqtt_pubtopic[] = {"Switch", "Switch2", "Switch3", "Alert1", "Alert2"};

bool state = false;
int room1 = 1, room2 = 1;

WiFiClientSecure espClient;
PubSubClient client(espClient);
SMTPSession smtp;

void smtpCallback(SMTP_Status status) {
  Serial.println("Email Status:");
  Serial.println(status.info());

  if (status.success()) {
    Serial.println("✅ Email sent successfully!");
  } else {
    Serial.println("❌ Failed to send email! Error: " + smtp.errorReason());
  }
}

void setup_wifi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32_Client", mqtt_user, mqtt_password, "status", 0, false, "offline")) {
      Serial.println(" ✅ Connected!");
      for (int i = 0; i < 5; i++) {
        client.subscribe(mqtt_Subtopic[i]);
      }
    } else {
      Serial.print(" ❌ Failed, Error Code: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("\n📩 Received Message from Topic: ");
  Serial.print(topic);
  Serial.print(" -> ");

  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  message.trim();

  // Control logic based on topic
  if (strcmp(topic, mqtt_Subtopic[0]) == 0) { // Led1 -> FAN1
    digitalWrite(FAN1, message == "ON" ? LOW : HIGH);
  } else if (strcmp(topic, mqtt_Subtopic[1]) == 0) { // Led2 -> FAN2
    digitalWrite(FAN2, message == "ON" ? LOW : HIGH);
  } else if (strcmp(topic, mqtt_Subtopic[2]) == 0) { // Led3 -> LIGHT1
    digitalWrite(LIGHT1, message == "ON" ? LOW : HIGH);
  } else if (strcmp(topic, mqtt_Subtopic[3]) == 0) { // Led4 -> LIGHT2
    digitalWrite(LIGHT2, message == "ON" ? LOW : HIGH);
  } else if (strcmp(topic, mqtt_Subtopic[4]) == 0) { // see_value
    Serial.println("The see value received.");
  }
}

void sendEmailNotification() {
  Serial.println("Sending email...");
  smtp.debug(1);
  smtp.callback(smtpCallback);

  ESP_Mail_Session session;
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;

  SMTP_Message message;
  message.sender.name = "ESP32 Sensor";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "SENSOR ALERT";
  message.addRecipient("User", RECIPIENT_EMAIL);

  String htmlMsg = "<div style=\"color:#2f4468;\"><h1>⚠ PERSON DETECTED!</h1><p>- Sent from ESP32</p></div>";
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  if (!smtp.connect(&session)) return;
  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("❌ Error sending Email: " + smtp.errorReason());
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  analogSetAttenuation(ADC_11db);
  espClient.setInsecure(); 
  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  pinMode(FAN1, OUTPUT);
  pinMode(LIGHT1, OUTPUT);
  pinMode(FAN2, OUTPUT);
  pinMode(LIGHT2, OUTPUT);

  pinMode(TRIG1_PIN, OUTPUT);
  pinMode(ECHO1_PIN, INPUT);
  pinMode(LED1_PIN, OUTPUT);

  pinMode(TRIG2_PIN, OUTPUT);
  pinMode(ECHO2_PIN, INPUT);
  pinMode(LED2_PIN, OUTPUT);

  Serial.println("\n✅ MQTT Ready. Waiting for commands...");
}

void loop() {
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop();

  checkDistance(TRIG1_PIN, ECHO1_PIN, LED1_PIN, "Sensor 1");
  delay(100);
  checkDistance(TRIG2_PIN, ECHO2_PIN, LED2_PIN, "Sensor 2");
  delay(100);

  if (room1 == 0) {
    Serial.println("⚠ PERSON Detected in Room 1!");
    room1 = 1;
    sendEmailNotification();
  }

  if (room2 == 0) {
    Serial.println("⚠ PERSON Detected in Room 2!");
    room2 = 1;
    sendEmailNotification();
  }

  float voltage = voltage_calculator();
  float current = current_calculator();
  float power = voltage * current;

  String messages[] = {
    "{\"voltage\":" + String(voltage) + ",\"Vunit\":\"V\",\"Current\":" + String(current) + ",\"Cunit\":\"A\"}",
    "{\"voltage\":" + String(current) + ",\"Vunit\":\"A\",\"Power\":" + String(power) + ",\"Punit\":\"W\"}",
    "{\"voltage\":" + String(power * 0.1) + ",\"Vunit\":\"₹\",\"Current\":2.00,\"Cunit\":\"A\"}",
    "{\"voltage\":" + String(room1) + "}",
    "{\"voltage\":" + String(room2) + "}"
  };

  if (!state) {
    client.publish(mqtt_pubtopic[3], messages[3].c_str());
    client.publish(mqtt_pubtopic[4], messages[4].c_str());
  }

  for (int i = 0; i < 3; i++) {
    client.publish(mqtt_pubtopic[i], messages[i].c_str());
    state = true;
  }

  delay(1000);
}

float voltage_calculator() {
  int adc_value = analogRead(ANALOG_IN_PIN);
  float voltage_adc = ((float)adc_value * REF_VOLTAGE) / ADC_RESOLUTION;
  float voltage_in = voltage_adc * (Voltage_R1 + Voltage_R2) / Voltage_R2;
  Serial.print("Measured Voltage = ");
  Serial.println(voltage_in, 2);
  return voltage_in;
}

float current_calculator() {
  int adc = analogRead(CURRENT_PIN);
  float adc_voltage = adc * (REF_VOLTAGE / ADC_RESOLUTION);
  float current_voltage = adc_voltage * (Current_R1 + Current_R2) / Current_R2;
  float current = (current_voltage - 2.5) / 0.100; // ACS712 calibration
  Serial.print("Current Value: ");
  Serial.println(current);
  return current;
}

void checkDistance(int trigPin, int echoPin, int ledPin, String label) {
  long duration;
  float distance_cm;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) {
    Serial.print(label);
    Serial.println(" Timeout");
    return;
  }

  distance_cm = duration * 0.0343 / 2.0;

  Serial.print(label);
  Serial.print(" Distance: ");
  Serial.print(distance_cm, 2);
  Serial.println(" cm");

  if (distance_cm < 3.0) {
    digitalWrite(ledPin, HIGH);
    if (label == "Sensor 1") room1 = 0;
    else if (label == "Sensor 2") room2 = 0;
  } else {
    if (label == "Sensor 1") room1 = 1;
    else if (label == "Sensor 2") room2 = 1;
    digitalWrite(ledPin, LOW);
  }
}
