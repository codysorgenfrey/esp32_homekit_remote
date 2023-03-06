#include <HomekitRemoteDevice.h>
#include <WebSocketsClient.h>

class TestClass : public HomekitRemoteDevice {
public:
  TestClass(WebSocketsClient *ws, const char *deviceID) : HomekitRemoteDevice(ws, deviceID) {}
  void handleHKRCommand(const JsonDocument &doc) {
    Serial.printf("handleHKRCommand: %s", doc["command"].as<const char *>());
  }
};

void setup() {
  // put your setup code here, to run once:
  new TestClass(new WebSocketsClient(), "TEST_DEVICE");
}

void loop() {
  // put your main code here, to run repeatedly:
}