#include <M5Atom.h>
#include <WiFi.h>
#include <Ticker.h>
#include "KishochoApiController.h"
#include "Config.h"

#ifdef USE_ADDITIONAL_LEDS
#include <FastLED.h>
#endif

Ticker ticker;
bool request_flag = true;
CRGB leds[NUM_LEDS];
uint32_t led_color;

void wifiConfig() {
    // 前回接続時情報で接続する
    connectWifi(30);
    Serial.println("");
    // 未接続の場合にはSmartConfig待受
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.mode(WIFI_STA);
        WiFi.beginSmartConfig();
        Serial.println("Waiting for SmartConfig");
        while (!WiFi.smartConfigDone()) {
            delay(500);
            Serial.print("#");
            // 30秒以上接続できなかったら抜ける
            if (30000 < millis()) {
                Serial.println("");
                Serial.println("Reset");
                ESP.restart();
            }
        }
        // Wi-fi接続
        Serial.println("");
        Serial.println("Waiting for WiFi");
        connectWifi(1);
    }
}

// WiFi接続
bool connectWifi(int retry_count) {
    if ((WiFi.status() != WL_CONNECTED)) {
        WiFi.begin();
        while (WiFi.status() != WL_CONNECTED) {
            retry_count--;
            if (retry_count < 0) {
                Serial.println("");
                return false;
            }
            Serial.print('.');
            delay(500);
        }
        Serial.println();
        Serial.printf("Connected, IP address : ");
        Serial.println(WiFi.localIP());
        return true;
    } else {
        return true;
    }
}

// HOURS_LATORに応じて天気を取得
void requestAPI() {
    // 明日の天気を取得
    int weather_id = KishochoApiController::getTomorrowWeatherId();
    Serial.printf("Weather ID : %d\n", weather_id);

    if (weather_id <= 0) { // 負の場合はエラー
        Serial.println("Error : Failed to get weather.");
    } else {
        // weather_idからLEDの色を取得
        led_color = getColorFromWeatherId(weather_id);
        // weather_idから天気名を取得
        String weather_name = getWeatherFromId(weather_id);

        Serial.printf("Weather : %s\n", weather_name);
    }
}

// 取得したweather_idに対応する天気文字列を取得
String getWeatherFromId(int weather_id) {
    // LED -> GGRRBB
    switch (int(weather_id / 100)) {
        case 1: // Clear
            return "Clear";
        case 2: // Clouds
            return "Clouds";
        case 3: // Rain
            return "Rain";
        case 4: // Snow
            return "Snow";
        case 5: // Clear(Night)
            return "Clear";
        case 6: // Clouds(Night)
            return "Clouds";
        case 7: // Rain(Night)
            return "Rain";
        case 8: // Snow(Night)
            return "Snow";
        default:
            return "Unknown";
    }
}

uint32_t getColorFromWeatherId(int weather_id) {
    switch (int(weather_id / 100)) {
        case 1: // Clear
            return CRGB::White;
        case 2: // Clouds
            return CRGB::Green;
        case 3: // Rain
            return CRGB::Blue;
        case 4: // Snow
            return CRGB::Red;
        case 5: // Clear(Night)
            return CRGB::White;
        case 6: // Clouds(Night)
            return CRGB::Green;
        case 7: // Rain(Night)
            return CRGB::Blue;
        case 8: // Snow(Night)
            return CRGB::Red;
        default:
            return CRGB::Black;
    }
}

void flashMatrix(uint32_t color) {
    for (int i = 0; i < 25; i++) {
        M5.dis.drawpix(i, color);
    }
}

void flashLed(void* param) {
    uint8_t brightness = 0;
    float rad = 0.f;
    while (true) {
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = led_color;
        }

        brightness = 0.5 * (1 + sin(rad)) * 255 * MAX_BRIGHTNESS * 0.01; // 0 - 255
        //Serial.printf("%d\n", brightness);

        // setBrigheness
        M5.dis.setBrightness(0);
        M5.dis.setBrightness(brightness * 100 / 255); // 0 - 100
        flashMatrix(getGRB(led_color)); // ATOM LED -> GRB
        #ifdef USE_ADDITIONAL_LEDS
        FastLED.setBrightness(brightness); // 0 - 255
        FastLED.show();
        #endif
        rad += 0.03;

        delay(10);
    }
}

// RGBをGRBに変換
uint32_t getGRB(uint32_t rgb) {
    uint8_t red = rgb >> 16;
    uint8_t green = rgb >> 8;
    uint8_t blue = rgb;
    uint32_t grb = 0;
    grb += green;
    grb = grb << 8;
    grb += red;
    grb = grb << 8;
    grb += blue;
    return grb;
}



// tickerで設定した時間毎にリクエストフラグを立てる
void timeToRequest() {
    request_flag = true;
}

void setup() {
    M5.begin(true, false, true);

    // FastLED
    #ifdef USE_ADDITIONAL_LEDS
    FastLED.addLeds<P9813, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    //FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    //FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    #endif

    wifiConfig();

    // タスク設定
    xTaskCreatePinnedToCore(flashLed, "flashLed", 4096, NULL, 1, NULL, 1);

    // 定期実行
    ticker.attach(1800, timeToRequest);
}

void loop() {
    if (request_flag) {
        // WiFi接続
        if (connectWifi(30)) { // param : retry count
            Serial.println("WiFi connected.");
            // APIリクエスト
            requestAPI();

            //  WiFi切断
            WiFi.disconnect();
            Serial.println("WiFi disconnect.");
            Serial.println("");
            request_flag = false;
        } else {
            Serial.println("Falied to connect WiFi.");
        }
    }
}