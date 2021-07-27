#include <M5Atom.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "HttpRequestController.h"

// Getリクエスト
/**
 * response_json : JSONレスポンス
 * url : URL
 * return :  1  成功
 *          -1 desirializeJson失敗
 *          -2 エラーレスポンス
 *          -3 WiFi接続失敗
 */
int HttpRequestController::getRequest(JsonObject *response_json, const char *url) {
    if ((WiFi.status() == WL_CONNECTED)) {
        HTTPClient http_client;
        http_client.begin(url);
        // リクエスト
        int http_code = http_client.GET();
        Serial.print("Status Code : ");
        Serial.println(http_code);
        if (http_code == 200) {
            // response取得
            String json = http_client.getString();
            if (DEBUG) Serial.println(json);
            DynamicJsonDocument json_buffer(BUFFER_SIZE);
            auto error = deserializeJson(json_buffer, json);
            if (error) {
                Serial.print("Failed to deserializeJson()");
                Serial.println(error.c_str());
                http_client.end();
                return -1;
            } else {
                *response_json = json_buffer[0].as<JsonObject>();
                http_client.end();
                return 1;
            }
        } else {
            // エラーレスポンス
            Serial.println("Error on http request");
            http_client.end();
            return -2;
        }
    } else {
        Serial.println("Failed to connect WiFi");
        return -3;
    }
}

