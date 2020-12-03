#include <M5Atom.h>
#include <WiFi.h>
#include <Ticker.h>
#include <FastLED.h>
#include "OpenWeatherMapApiController.h"
#include "Config.h"

Ticker ticker;
bool request_flag = true;
CRGB leds[NUM_LEDS];
uint32_t led_color;

// WiFi接続
bool connectWifi(int retry_count) {
    if ((WiFi.status() != WL_CONNECTED)) {
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
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
    int weather_id = 0;
    if (HOURS_LATOR == 0 && DAYS_LATOR == 0) {
        // 現在の天気を取得
        weather_id = OpenWeatherMapApiController::getCurrentWeatherId();
        Serial.printf("Weather ID : %d\n", weather_id);
    } else if (DAYS_LATOR == 0) {
        // 指定時間後の天気を取得
        weather_id = OpenWeatherMapApiController::getHoursLatorWeatherId();
        Serial.printf("Weather ID : %d\n", weather_id);
    } else {
        // 指定日数後の天気を取得
        weather_id = OpenWeatherMapApiController::getDaysLatorWeatherId();
        Serial.printf("Weather ID : %d\n", weather_id);
    }

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

uint32_t getColorFromWeatherId(int weather_id) {
    switch (int(weather_id / 100)) {
        case 2: // Thunderstorm
            return CRGB::Yellow;
        case 3: // Drizzle
            return CRGB::Cyan;
        case 5: // Rain
            return CRGB::Blue;
        case 6: // Snow
            return CRGB::Red;
        case 7: // Atmosphere
            return CRGB::Purple;
        case 8:
            if (weather_id < 803) { // Clear
                return CRGB::White;
            } else { // Clouds
                return CRGB::Green;
            }
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
    Serial.printf("%d\n", brightness);
    float rad = 0.f;
    while (true) {
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = led_color;
        }

        brightness = 0.5 * (1 + sin(rad)) * 255 * MAX_BRIGHTNESS * 0.01; // 0 - 255

        // setBrigheness
        M5.dis.setBrightness(brightness * 100 / 255); // 0 - 100
        FastLED.setBrightness(brightness); // 0 - 255

        // Flash
        FastLED.show();
        flashMatrix(getGRB(led_color)); // ATOM LED -> GRB
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

// 取得したweather_idに対応する天気文字列を取得
String getWeatherFromId(int weather_id) {
    // LED -> GGRRBB
    switch (int(weather_id / 100)) {
        case 2: // Thunderstorm
            return "Thunderstorm";
        case 3: // Drizzle
            return "Drizzle";
        case 5: // Rain
            return "Rain";
        case 6: // Snow
            return "Snow";
        case 7: // Atmosphere
            return "Atmosphere";
        case 8:
            if (weather_id < 803) { // Clear
                return "Clear";
            } else { // Clouds
                return "Clouds";
            }
        default:
            return "Unknown";
    }
}

// tickerで設定した時間毎にリクエストフラグを立てる
void timeToRequest() {
    request_flag = true;
}

void setup() {
    M5.begin(true, false, true);

    // FastLED
    FastLED.addLeds<P9813, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    //FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);


    // タスク設定
    xTaskCreatePinnedToCore(flashLed, "flashLed", 4096, NULL, 1, NULL, 1);

    // 定期実行
    ticker.attach(300, timeToRequest);
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