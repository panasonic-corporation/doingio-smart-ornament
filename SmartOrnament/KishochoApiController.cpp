#include <M5Atom.h>
#include "HttpRequestController.h"
#include "KishochoApiController.h"

// 翌日の天気を取得
int KishochoApiController::getTomorrowWeatherId() {
    Serial.println("getTomorrowtWeatherId");
    JsonObject response_json;
    char url[64];
    sprintf(url, KISHOCHO_BASE_URL "%d.json", KISHOCHO_PLACE_CODE);
    Serial.printf("Request URL : %s\n", url);
    int result = HttpRequestController::getRequest(&response_json, url);
    if (result) {
        return response_json["timeSeries"][0]["areas"][0]["weatherCodes"][1].as<int>();
    } else {
        Serial.println("error");
        return result;
    }
    return 0;
}
