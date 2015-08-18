#ifndef _V_ROBOT_FACADE_IMPL_H_
#define _V_ROBOT_FACADE_IMPL_H_

#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <boost/signals2/signal.hpp>
#include <boost/property_tree/ptree.hpp>
#include <vcore/v_core_global.h>
#include <vcore/types.h>
#include <vcore/containers.h>

namespace vunirobot {
class VUnirobotThread;
class DCMotorParameters;
}

class VRobotMotionFacadeImpl
{
public:
    VRobotMotionFacadeImpl();
    ~VRobotMotionFacadeImpl();

    bool readParameters( const std::string& vendorDir,
                         int motor_left,
                         int motor_right,
                         int motor_left_reverse,
                         int motor_left_mode,
                         int motor_right_reverse,
                         int motor_right_mode,
                         std::string motor_left_config,
                         std::string motor_right_config,
                         float distance_between_wheels,
                         float wheel_radius,
                         float maxSpeed,
                         float maxOmega );

    int connectToDevices();
    void disconnectFromDevices();

    bool setSpeed(int velocity, int omega);
    bool getSpeed(vreal &velocity, vreal &omega);
    bool getEngineSpeed(vreal &velocity, vreal &omega);
    bool getPosition(vpointf &point, vreal &phi);

    void setStartPosition(const vpointf& point, vreal phi);

    void getDeviceState(DevicesState &deviceState);
    void getDeviceName(std::string &deviceName, int deviceId);
    void resetErrors();

    void ignoreOdometryErrors( bool ignore );

    std::shared_ptr<vunirobot::VUnirobotThread> getUnirobotThread( ) {
        return m_unirobotThread;
    }

    enum
    {
        UNIROBOT
    };

    enum unirobot_state_t {
        OK = 0,
        REBOOT = 1,
        OVERLOAD = 2,
        CRASH = 3,
        UNCERTAIN = 4
    };

private:
    struct MotorInfo
    {
        MotorInfo();

        int mode = -1;
        std::shared_ptr<vunirobot::DCMotorParameters> uparams;
        bool reverse = false;

        int gearRatio = -1;
        int numberOfTicksPerRevolutionEncoder = -1;
        int motorNominalOmega = -1;
        vreal reductionFactor = -1;
    };

    void initUnirobot();
    int16_t convertToUnirobotOmega( vreal omega, int motorId ) const;
    vreal convertFromUnirobotOmega( int omega, int motorId ) const;
    void readDCMotorParameters(const std::string& config, VRobotMotionFacadeImpl::MotorInfo& info);

    static bool rebooted( unirobot_state_t x, unirobot_state_t y, unirobot_state_t angle );

    DevicesState m_state;
    std::shared_ptr<vunirobot::VUnirobotThread> m_unirobotThread;
    std::mutex m_speedMutex;

    std::shared_ptr<std::thread> m_initUnirobotThread;
    bool m_initUnirobotWorking = false;

    int m_motorLeftId = -1;
    int m_motorRightId = -1;

    std::map<int, MotorInfo> m_info;

    vreal m_distanceBetweenWheels = 0;
    vreal m_wheelRadius = 0;

    vreal m_maxSpeed = 0;
    vreal m_maxOmega = 0;

    std::mutex m_offsetMutex;
    boost::geometry::model::d2::point_xy<long, boost::geometry::cs::cartesian> m_positionOffset;
    boost::geometry::model::d2::point_xy<long, boost::geometry::cs::cartesian> m_previousPosition;
    long m_angleOffset = 0;
    long m_previousAngle = 0;
    bool m_previousPositionKnown = false;
    bool m_motionResume = true;

    std::chrono::high_resolution_clock::time_point m_suspendTime;
    std::chrono::milliseconds m_suspendPeriod = std::chrono::milliseconds( 3000 );
    volatile bool m_suspendMotion = false;
    bool m_ignoreOdometryErrors = true;

    static const long MAX_ANGLE;
    static const long MAX_X;
    static const long MAX_Y;

    static const long REBOOT_ANGLE_THRESHOLD;
    static const long REBOOT_COORD_THRESHOLD;
};

#endif // _V_ROBOT_FACADE_IMPL_H_
