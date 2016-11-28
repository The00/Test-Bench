#include "Arduino.h"
#include "banc.h"

thrust_data read_thrust()
{
    thrust_data result;
    result.sensor1=analogRead(THRUST_IN1);
    result.sensor2=analogRead(THRUST_IN2);

    return result;
}






