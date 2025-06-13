#include <WiFi.h>

// Wi-Fi credentials
const char* ssid = "ESP32-Network";
const char* password = "Esp32-Password";

WiFiServer server(80);
String header;

// Pin definitions
const int motionSensorPin = 14;
const int ledPin = 0;
const int buzzerPin = 22;

// States
bool employeePresent = false;
bool previousPresence = false;
unsigned long lastMotionTime = 0;
unsigned long lastCheckTime = 0;
unsigned long sittingStartTime = 0;
unsigned long totalSittingTime = 0;
unsigned long lastLeftTime = 0;

String stateLED = "off";

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);

  pinMode(motionSensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

  WiFi.softAP(ssid, password);
  Serial.println("Access Point started");
  Serial.println(WiFi.softAPIP());

  server.begin();
}

void loop() {
  unsigned long now = millis();

  // Check for motion every 30 seconds
  if (now - lastCheckTime >= 10000) {
    lastCheckTime = now;

    int motion = digitalRead(motionSensorPin);
    previousPresence = employeePresent;
    employeePresent = (motion == HIGH);

    if (employeePresent) {
      if (!previousPresence) {
        tone(buzzerPin, 1000, 200); // Welcome beep
        sittingStartTime = now;
      }
    } else {
      if (previousPresence) {
        tone(buzzerPin, 1000, 1000); // Left alert
        totalSittingTime += (now - sittingStartTime);
        lastLeftTime = now;
      }
    }

    // LED status update
    digitalWrite(ledPin, employeePresent ? HIGH : LOW);
    stateLED = employeePresent ? "on" : "off";
  }

  // Handle web client
  WiFiClient client = server.available();

  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");
    String currentLine = "";

    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        header += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Web page content
            client.println("<!DOCTYPE html><html>");
            client.println("<head>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<title>Office Cubicle Monitor</title>");
            client.println("<style>");
            client.println("body { font-family: 'Arial', sans-serif; margin: 0; padding: 20px; background-color: #f0f2f5; color: #333; }");
            client.println(".container { max-width: 600px; margin: 0 auto; background-color: #fff; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); padding: 20px; }");
            client.println("h1 { color: #2c3e50; text-align: center; margin-bottom: 30px; }");
            client.println(".status-card { padding: 15px; margin: 15px 0; border-radius: 8px; text-align: center; }");
            client.println(".present { background-color: #d4edda; color: #155724; border: 1px solid #c3e6cb; }");
            client.println(".absent { background-color: #f8d7da; color: #721c24; border: 1px solid #f5c6cb; }");
            client.println(".info-card { background-color: #e2f0fb; border: 1px solid #bee5eb; border-radius: 8px; padding: 15px; margin: 15px 0; }");
            client.println(".info-item { display: flex; justify-content: space-between; padding: 10px 0; border-bottom: 1px solid #eee; }");
            client.println(".info-item:last-child { border-bottom: none; }");
            client.println(".status-indicator { font-size: 24px; font-weight: bold; }");
            client.println(".refresh-button { display: block; width: 100%; background-color: #007bff; color: white; border: none; padding: 12px; border-radius: 5px; font-size: 16px; cursor: pointer; margin-top: 20px; }");
            client.println(".refresh-button:hover { background-color: #0069d9; }");
            client.println("footer { text-align: center; margin-top: 20px; font-size: 12px; color: #6c757d; }");
            client.println("</style>");
            client.println("</head>");
            client.println("<body>");
            client.println("<div class='container'>");
            client.println("<h1>Office Cubicle Monitor</h1>");

            // Status card with color based on presence
            client.println("<div class='status-card " + String(employeePresent ? "present" : "absent") + "'>");
            client.println("<div class='status-indicator'>" + String(employeePresent ? "Employee Present" : "Employee Away") + "</div>");
            client.println("</div>");

            client.println("<div class='info-card'>");
            
            // Sitting time info
            unsigned long totalTime = totalSittingTime;
            if (employeePresent) {
              totalTime += (now - sittingStartTime);
            }
            
            client.println("<div class='info-item'>");
            client.println("<strong>Total Sitting Time:</strong>");
            client.println("<span>" + formatTime(totalTime) + "</span>");
            client.println("</div>");

            // Only show time since left if employee is absent
            if (!employeePresent && lastLeftTime > 0) {
              client.println("<div class='info-item'>");
              client.println("<strong>Left Office:</strong>");
              client.println("<span>" + formatTime(now - lastLeftTime) + " ago</span>");
              client.println("</div>");
            }
            
            client.println("</div>");
            
            // Refresh button
            client.println("<button class='refresh-button' onclick='window.location.reload();'>Refresh Data</button>");
            
            client.println("<footer>Last updated: " + formatTime(millis()) + " uptime</footer>");
            client.println("</div>");
            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    header = "";
    client.stop();
    Serial.println("Client disconnected.");
  }
}

// Helper to convert milliseconds to HH:MM:SS format
String formatTime(unsigned long ms) {
  unsigned long seconds = ms / 1000;
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int secs = seconds % 60;
  char buffer[16];
  sprintf(buffer, "%02d:%02d:%02d", hours, minutes, secs);
  return String(buffer);
}