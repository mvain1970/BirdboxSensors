#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>

namespace {
Preferences preferences;
constexpr unsigned long kConnectTimeoutMs = 30000;

String readSecretLine() {
  String value;
  while (true) {
    while (Serial.available()) {
      const char ch = static_cast<char>(Serial.read());
      if (ch == '\n') {
        value.trim();
        return value;
      }
      if (ch != '\r') {
        value += ch;
      }
    }
    delay(10);
  }
}

bool connectToWifi(const String &ssid, const String &password) {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid.c_str(), password.c_str());

  Serial.printf("Connecting to %s", ssid.c_str());
  const unsigned long startedAt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startedAt < kConnectTimeoutMs) {
    Serial.print('.');
    delay(500);
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.printf("Wi-Fi connection failed (status %d).\n", WiFi.status());
    return false;
  }

  Serial.printf("Wi-Fi connected. IP address: %s\n", WiFi.localIP().toString().c_str());
  return true;
}

void provisionOverUsb() {
  Serial.println("PROVISIONING_REQUIRED");
  Serial.println("Send the SSID on one line and the password on the next line.");
  const String ssid = readSecretLine();
  const String password = readSecretLine();

  if (ssid.isEmpty() || password.isEmpty()) {
    Serial.println("Provisioning rejected: SSID and password must not be empty.");
    return;
  }

  if (!connectToWifi(ssid, password)) {
    Serial.println("Provisioning failed; credentials were not saved.");
    return;
  }

  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  Serial.println("PROVISIONING_COMPLETE");
}
}  // namespace

void setup() {
  Serial.begin(115200);
  const unsigned long serialWaitStartedAt = millis();
  while (!Serial && millis() - serialWaitStartedAt < 5000) {
    delay(10);
  }

  Serial.println("Birdbox Sensors starting");
  preferences.begin("birdbox-wifi", false);

  const String ssid = preferences.getString("ssid", "");
  const String password = preferences.getString("password", "");
  if (ssid.isEmpty() || password.isEmpty() || !connectToWifi(ssid, password)) {
    provisionOverUsb();
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected; reconnecting.");
    WiFi.reconnect();
  } else {
    Serial.printf("Birdbox Sensors online at %s\n", WiFi.localIP().toString().c_str());
  }
  delay(30000);
}

