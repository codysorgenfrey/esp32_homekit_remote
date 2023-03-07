# esp32_homekit_remote
Websocket protocol for custom homekit hub

Set up by inheriting ```HomekitRemoteDevice``` or ```HomekitRemoteDeviceServerSide```.
- Override ```handleHKRCommand```
- Override ```handleHKRError```
- Call ```listenForHKRResponse``` in ```loop()```