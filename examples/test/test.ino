#include <HomekitRemoteDevice.h>
#include <HomekitRemoteDeviceServerSide.h>
#include <WebSocketsClient.h>
#include <WebSocketsServer.h>

class TestClass : public HomekitRemoteDevice {
public:
  TestClass(WebSocketsClient *ws, const char *deviceID) : HomekitRemoteDevice(ws, deviceID) {}
  void handleHKRCommand(const JsonDocument &doc) {
    Serial.printf("handleHKRCommand: %s", doc["command"].as<const char *>());
  }
};

class TestClass2 : public HomekitRemoteDeviceServerSide {
public:
  TestClass2(WebSocketsServer *ws) : HomekitRemoteDeviceServerSide(ws) {}
  void handleHKRCommand(const JsonDocument &doc) {
    Serial.printf("handleHKRCommand: %s", doc["command"].as<const char *>());
  }
};

void setup() {
  // put your setup code here, to run once:
  new TestClass(new WebSocketsClient(), "TEST_DEVICE");
  new TestClass2(new WebSocketsServer(81));
}

void loop() {
  // put your main code here, to run repeatedly:
}