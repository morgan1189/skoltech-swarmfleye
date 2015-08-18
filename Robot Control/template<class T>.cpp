template<class T>
T TRANSFORM_TO_RIGHT_OMEGA(T v, T w, T r, T wR)
{
    return (((v) + (w) * (r)) / (wR));
}

template<class T>
T TRANSFORM_TO_LEFT_OMEGA (T v, T w, T r, T wR)
{
    return (((v) - (w) * (r)) / (wR));
}

2015-08-17 18:28 GMT+03:00 Dmitry Suvorov <d.suvorov90@gmail.com>:
#ifndef _PORT_CONFIG_H_
#define _PORT_CONFIG_H_

#define POWER_PORT     "/dev/power"
#define UNIROBOT_PORT  "/dev/unirobot"
#define DYNAMIXEL_PORT "/dev/dynamixel"
#define RANGER_PORT    "/dev/hokuyo"
#define RPRANGER_PORT  "/dev/rplidar"
#define MIKROTIK_PORT  "/dev/mikrotik"

#endif // _PORT_CONFIG_H_