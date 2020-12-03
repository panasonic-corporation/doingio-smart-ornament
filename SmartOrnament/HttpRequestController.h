#pragma once
#include <ArduinoJson.h>
#define BUFFER_SIZE 20000
#define DEBUG false

class HttpRequestController {
private:

public:
    static int getRequest(JsonObject *response_json, const char *url);
};