#include <M5Atom.h>
#include "HttpRequestController.h"
#include "OpenWeatherMapApiController.h"

// 現在の天気取得
int OpenWeatherMapApiController::getCurrentWeatherId() {
    Serial.println("getCurrentWeatherId");
    JsonObject response_json;
    char *url = API_BASE_URL "?lat=" API_PARAM_LAT "&lon=" API_PARAM_LON "&exclude=hourlyl,daily,alerts&appid=" API_TOKEN;
    Serial.printf("Request URL : %s\n", url);
    int result = HttpRequestController::getRequest(&response_json, url);
    if (result) {
        //Serial.println(response_json["current"]["dt"].as<int>());
        return response_json["current"]["weather"][0]["id"].as<int>();
    } else {
        Serial.println("error");
        return result;
    }
}

// 指定時間後の天気取得
int OpenWeatherMapApiController::getHoursLatorWeatherId() {
    Serial.println("getHoursLatorWeatherId");
    JsonObject response_json;
    char *url = API_BASE_URL "?lat=" API_PARAM_LAT "&lon=" API_PARAM_LON "&exclude=hourlyl,daily,alerts&appid=" API_TOKEN;
    Serial.printf("Request URL : %s\n", url);
    int result = HttpRequestController::getRequest(&response_json, url);
    if (result) {
        //Serial.println(response_json["hourly"][HOURS_LATOR]["dt"].as<int>());
        return response_json["hourly"][HOURS_LATOR]["weather"][0]["id"].as<int>();
    } else {
        Serial.println("error");
        return result;
    }
}

// 指定日数後の天気取得
int OpenWeatherMapApiController::getDaysLatorWeatherId() {
    Serial.println("getDaysLatorWeatherId");
    JsonObject response_json;
    char *url = API_BASE_URL "?lat=" API_PARAM_LAT "&lon=" API_PARAM_LON "&exclude=hourly,minutely,alerts&appid=" API_TOKEN;
    Serial.printf("Request URL : %s\n", url);
    int result = HttpRequestController::getRequest(&response_json, url);
    if (result) {
        //Serial.println(response_json["daily"][DAYS_LATOR]["dt"].as<int>());
        return response_json["daily"][DAYS_LATOR]["weather"][0]["id"].as<int>();
    } else {
        Serial.println("error");
        return result;
    }
}