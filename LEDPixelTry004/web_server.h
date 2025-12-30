#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WiFiS3.h>
#include <NuSock.h>

extern NuSockServer wsServer;

void initWebServer();
void handleWebServer();
void onWSEvent(NuClient *client, NuServerEvent event, const uint8_t *payload, size_t len);

#endif // WEB_SERVER_H