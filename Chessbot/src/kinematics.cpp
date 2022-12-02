#include <Arduino.h>
#include "kinematics.h"

Kinematics::Kinematics()
{
}

int16_t Kinematics::coordsToVelocityMotor1(float x_coordinate, float y_coordinate)
{
    if (y_coordinate == 0)
    {
        if (x_coordinate > 0)
        {
            return -1000; // quadrant 1
        }

        else
        {
            return 1000; // quadrant 3
        }
    }

    if (x_coordinate == 0)
    {
        if (y_coordinate > 0)
        {
            return -1000; // quadrant 1
        }

        else
        {
            return 1000; // quadrant 3
        }
    }
    
    if (abs(y_coordinate) > abs(x_coordinate)) // above 45 degree angles
    {
        if (y_coordinate / x_coordinate > 0) // Motor 1 Max
        {
            if (x_coordinate > 0) // sector 2
            {
                return -1000;
            }

            else // sector 6
            {
                return 1000;
            }
        }

        else
        {
            if (x_coordinate > 0) // sector 7
            {
                return -1000 * (1 + (y_coordinate / x_coordinate)) / (1 - (y_coordinate / x_coordinate));
            }

            else // sector 3
            {
                return 1000 * (1 + (y_coordinate / x_coordinate)) / (1 - (y_coordinate / x_coordinate));
            }
        }
    }

    if (abs(y_coordinate) < abs(x_coordinate)) // below 45 degree angles
    {
        if (x_coordinate / y_coordinate > 0) // Motor 1 Max
        {
            if (x_coordinate > 0) // sector 1
            {
                return -1000;
            }

            else // sector 5
            {
                return 1000;
            }
        }

        else
        {
            if (x_coordinate > 0) // sector 8
            {
                return 1000 * ((x_coordinate / y_coordinate + 1) / (1 - x_coordinate / y_coordinate));
            }

            else // sector 4
            {
                return -1000 * ((x_coordinate / y_coordinate + 1) / (1 - x_coordinate / y_coordinate));
            }
        }
    }

    if (abs(y_coordinate) == abs(x_coordinate)) // 45 degrees
    {
        if (y_coordinate / x_coordinate > 0)
        {
            if (x_coordinate > 0)
            {
                return -1000; // quadrant 1
            }

            else
            {
                return 1000; // quadrant 3
            }
        }

        else
        {
            return 0;
        }
    }

    
    return 0;
}

int16_t Kinematics::coordsToVelocityMotor2(float x_coordinate, float y_coordinate)
{
    if (y_coordinate == 0)
    {
        if (x_coordinate > 0)
        {
            return -1000; // quadrant 1
        }

        else
        {
            return 1000; // quadrant 3
        }
    }

    if (x_coordinate == 0)
    {
        if (y_coordinate > 0)
        {
            return 1000;
        }

        else
        {
            return -1000;
        }
    }

    if (abs(y_coordinate) > abs(x_coordinate)) // above 45 degree angles
    {
        if (y_coordinate / x_coordinate > 0) // Motor 1 Max
        {
            if (x_coordinate > 0) // sector 2
            {
                return -1000 * (1 - (y_coordinate / x_coordinate)) / (1 + (y_coordinate / x_coordinate));
            }

            else // sector 6
            {
                return 1000 * (1 - (y_coordinate / x_coordinate)) / (1 + (y_coordinate / x_coordinate));
            }
        }

        else // Motor 2 Max
        {
            if (x_coordinate > 0) // sector 7
            {
                return -1000;
            }

            else // sector 3
            {
                return 1000;
            }
        }
    }

    if (abs(y_coordinate) < abs(x_coordinate)) // below 45 degree angles
    {
        if (x_coordinate / y_coordinate > 0) // Motor 1 Max
        {
            if (x_coordinate > 0) // sector 1
            {
                return -1000 * ((x_coordinate / y_coordinate) - 1) / ((1 + (x_coordinate / y_coordinate)));
            }

            else // sector 5
            {
                return 1000 * ((x_coordinate / y_coordinate) - 1) / ((1 + (x_coordinate / y_coordinate)));
            }
        }

        else // Motor 2 Max
        {
            if (x_coordinate > 0) // sector 8
            {
                return -1000;
            }

            else // sector 4
            {
                return 1000;
            }
        }
    }

    if (abs(y_coordinate) == abs(x_coordinate)) // 45 degrees
    {
        if (y_coordinate / x_coordinate < 0)
        {
            if (x_coordinate > 0)
            {
                return -1000; // quadrant 2
            }

            else
            {
                return 1000; // quadrant 4
            }
        }

        else
        {
            return 0;
        }
    }

    

    return 0;
}

uint16_t Kinematics::coordsToStepsMotor1(float x_coordinate, float y_coordinate)
{
    float stepLength = 0.1; //  mm/step

    if (abs(y_coordinate) == abs(x_coordinate)) // 45 degrees
    {
        if (y_coordinate / x_coordinate > 0)
        {
            return abs(2 * x_coordinate);
        }

        else
        {
            return 0;
        }
    }

    if (y_coordinate == 0)
    {
        return abs(x_coordinate / stepLength);
    }

    if (x_coordinate == 0)
    {
        return abs(y_coordinate / stepLength);
    }
   

    if (abs(y_coordinate) > abs(x_coordinate)) // above 45 degree angles
    {
        if (y_coordinate / x_coordinate > 0) // Motor 1 Max
        {
            return abs(((-2 * y_coordinate) / stepLength) / (1 - ((1 - y_coordinate / x_coordinate) / (1 + y_coordinate / x_coordinate))));
        }

        else
        {
            return abs(((-2 * x_coordinate / stepLength) * (1 + y_coordinate / x_coordinate) / (1 - y_coordinate / x_coordinate)) / (1 + ((1 + y_coordinate / x_coordinate) / (1 - y_coordinate / x_coordinate))));
        }
    }

    if (abs(y_coordinate) < abs(x_coordinate)) // below 45 degree angles
    {
        if (x_coordinate / y_coordinate > 0) // Motor 1 Max
        {
            return abs((-2 * x_coordinate / stepLength) / (1 + (x_coordinate / y_coordinate - 1) / (1 + x_coordinate / y_coordinate)));
        }

        else
        {
            return abs(((-2 * y_coordinate / stepLength) * ((x_coordinate / y_coordinate + 1) / (1 - x_coordinate / y_coordinate))) / (1 - (x_coordinate / y_coordinate + 1) / (1 - x_coordinate / y_coordinate)));
        }
    }
    return 0;

   
}

uint16_t Kinematics::coordsToStepsMotor2(float x_coordinate, float y_coordinate)
{
    float stepLength = 0.1; //  mm/step

    if (abs(y_coordinate) == abs(x_coordinate)) // 45 degrees
    {
        if (y_coordinate / x_coordinate < 0)
        {
            return abs(2 * x_coordinate);
        }

        else
        {
            return 0;
        }
    }

    if (y_coordinate == 0)
    {
        return abs(x_coordinate / stepLength);
    }

    if (x_coordinate == 0)
    {
        return abs(y_coordinate / stepLength);
    }

    if (abs(y_coordinate) > abs(x_coordinate)) // above 45 degree angles
    {
        if (y_coordinate / x_coordinate > 0) // Motor 1 Max
        {
            return abs((((-2 * y_coordinate) / stepLength) * (1 - (y_coordinate / x_coordinate) / (1 + y_coordinate / x_coordinate))) / (1 - ((1 - y_coordinate / x_coordinate) / (1 + y_coordinate / x_coordinate))));
        }

        else
        {
            return abs((-2 * x_coordinate / stepLength) / (1 + ((1 + y_coordinate / x_coordinate) / (1 - y_coordinate / x_coordinate))));
        }
    }

    if (abs(y_coordinate) < abs(x_coordinate)) // below 45 degree angles
    {
        if (x_coordinate / y_coordinate > 0) // Motor 1 Max
        {
            return abs((-2 * x_coordinate / stepLength) * ((x_coordinate / y_coordinate - 1) / (1 + x_coordinate / y_coordinate)) / (1 + (x_coordinate / y_coordinate - 1) / (1 + x_coordinate / y_coordinate)));
        }

        else
        {
            return abs(((-2 * y_coordinate / stepLength) / (1 - x_coordinate / y_coordinate))) / (1 - (x_coordinate / y_coordinate + 1) / (1 - x_coordinate / y_coordinate));
        }
    }

    
    return 0;
}