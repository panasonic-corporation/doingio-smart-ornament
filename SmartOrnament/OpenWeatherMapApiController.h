#pragma once
#include "HttpRequestController.h"
#include "Config.h"

class OpenWeatherMapApiController {
private:

public:
    static int getCurrentWeatherId();
    static int getHoursLatorWeatherId();
    static int getDaysLatorWeatherId();
};