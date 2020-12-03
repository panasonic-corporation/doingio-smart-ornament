#define NUM_LEDS 6 // LEDの数
#define MAX_BRIGHTNESS 70 // 0 - 100% 
#define DATA_PIN 26
#define CLOCK_PIN 32

#define WIFI_SSID     ""      // WiFiのSSID
#define WIFI_PASSWORD ""  // WiFiのパスワード

#define API_BASE_URL  "http://api.openweathermap.org/data/2.5/onecall" // APIのBASE URL
#define API_TOKEN     "" // Open Weather Map APIのaccess token
#define API_PARAM_LAT "35.68"  // 緯度(小数点第2位まで)
#define API_PARAM_LON "139.77" // 経度(小数点第2位まで)

// いつの天気予報を取得するか
// - 現在の天気予報を取得する場合は HOURS_LATOR, DAYS_LATOR 共に0としてください
// - 指定時間後の天気予報を取得する場合は DAYS_LATOR を0としてください
// - 指定日数後の天気予報を取得する場合は HOURS_LATOR の値を0としてください
// - どちらも0でない場合は DAYS_LATOR の値を優先します
#define HOURS_LATOR 0 // 何時間後の天気を取得するか 1 - 47 時間
#define DAYS_LATOR  1 // 何時間後の天気を取得するか 1 - 7 日