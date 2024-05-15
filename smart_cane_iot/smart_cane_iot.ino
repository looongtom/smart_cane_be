#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <OneButton.h>
#include <HTTPClient.h>

int nutnhan = 5;
int gtnutnhan;

OneButton button(nutnhan, true);

const char* ssid = "Tomdeptrai";
const char* password = "Lkjhgfdsa";
const char* serverName = "http://192.168.1.10:8080/event";

const int TRIGGER_PIN = 25;  //ultrasonic
const int ECHO_PIN = 26;     //ultrasonic
const int buzzer = 13;
const int MOTOR_PIN = 27;

// Threshold distances
const int ALERT_DISTANCE = 10;  // Alert if object is within 30 cm

// Timing constants
const int BUZZER_ON_TIME = 500;  // Buzzer on time in milliseconds
const int MOTOR_ON_TIME = 200;   // Haptic motor on time in milliseconds
const int LOOP_DELAY = 500;      // Main loop delay in milliseconds
void setup() {
  Serial.begin(9600);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);

  button.attachDoubleClick(nhan_double);  //Kích hoạt lệnh khi nhấn liên tục 2 lần
  button.attachClick(nhan_don);           //Kích hoạt lệnh khi nhấn 1 lần rồi nhả
  button.attachLongPressStart(nhan_giu);  //Kích hoạt lệnh khi nhấn giữ 1s

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  // Print the ESP32's IP address
  Serial.print("ESP32 Web Server's IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  gtnutnhan = digitalRead(nutnhan);
  //Serial.print("Nút nhấn: "); Serial.println(gtnutnhan);
  button.tick();  //Kiểm tra trạng thái nút nhấn

  // Send ultrasonic pulse
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  // Measure pulse duration
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate distance in cm
  int distance = duration / 58;

  // Debug output
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Check for alerts
  if (distance <= ALERT_DISTANCE) {
    // Turn on buzzer
    tone(buzzer, 1000, 1000);  // 1KHz
    digitalWrite(MOTOR_PIN, HIGH);
    delay(BUZZER_ON_TIME);
    digitalWrite(MOTOR_PIN, LOW);
    noTone(buzzer);  // 1KHz
  }
  noTone(buzzer);  // 1KHz

  // Wait before taking another reading
  delay(LOOP_DELAY);
}

void sendToServer(String message) {
  HTTPClient http;

  // Your server URL
  http.begin(serverName);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  // Prepare your message data
  String postData = "message=" + message;

  // Send HTTP POST request
  int httpResponseCode = http.POST(postData);

  // If the request was successful (HTTP status code 200)
  if (httpResponseCode > 0) {
    Serial.print("Message sent to server successfully. Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error sending message to server. HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
}

void nhan_don() {
  Serial.print("Bấm thường:");
  sendToServer("Bấm thường: Gửi vị trí hiện tại");
}

void nhan_double() {
  Serial.print("Bấm double:");
  sendToServer("Bấm double: Gửi tin nhắn Ét O Ét");
}

void nhan_giu() {
  Serial.print("Bấm giữ: ");
  sendToServer("Bấm giữ: Gọi điện thoại");
}
