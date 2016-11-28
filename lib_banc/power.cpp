#include "Arduino.h"
#include "banc.h"

power_data read_power()
{
    power_data result;
    result.current=analogRead(P_CURRENT);
    result.voltage=analogRead(P_VOLTAGE);

    return result;
}
