#include <cmath>
#include <sstream>

#include <boost/property_tree/xml_parser.hpp>

#include <vunirobot/v_unirobot_thread.h>

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

#define POWER_PORT     "/dev/power"
#define UNIROBOT_PORT  "/dev/unirobot"
#define DYNAMIXEL_PORT "/dev/dynamixel"
#define RANGER_PORT    "/dev/hokuyo"
#define RPRANGER_PORT  "/dev/rplidar"
#define MIKROTIK_PORT  "/dev/mikrotik"

#include "VRobotMotionFacadeImpl.h"

#define DIRECT vunirobot::VUnirobotAdapter::DIRECT
#define REVERSE vunirobot::VUnirobotAdapter::REVERSE

// Copy-paste from unirobot project.
#define MAX_COUNTS 5

const long VRobotMotionFacadeImpl::MAX_ANGLE = long(1.0 * 2 * M_PI * 1000 * MAX_COUNTS); //32741;
const long VRobotMotionFacadeImpl::MAX_X = 32000;
const long VRobotMotionFacadeImpl::MAX_Y = 32000;

const long VRobotMotionFacadeImpl::REBOOT_ANGLE_THRESHOLD = long( 1000 * M_PI_4 );
const long VRobotMotionFacadeImpl::REBOOT_COORD_THRESHOLD = 1000;

VRobotMotionFacadeImpl::MotorInfo::MotorInfo() :
    uparams(new vunirobot::DCMotorParameters)
{
}

VRobotMotionFacadeImpl::VRobotMotionFacadeImpl() :
    m_positionOffset( 0, 0 ),
    m_previousPosition( 0, 0 )
{
    m_unirobotThread.reset( new vunirobot::VUnirobotThread( ) );
}

VRobotMotionFacadeImpl::~VRobotMotionFacadeImpl()
{
    m_initUnirobotWorking  = false;
    if(m_initUnirobotThread.get()) {
        m_initUnirobotThread->join();
    }
}

bool VRobotMotionFacadeImpl::readParameters( const std::string& vendorDir,
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
                                             float maxOmega )
{
    MotorInfo leftInfo;
    MotorInfo rightInfo;

    try {
        m_motorLeftId = motor_left;
        m_motorRightId = motor_right;

        leftInfo.reverse = motor_left_reverse;
        leftInfo.mode = motor_left_mode;

        rightInfo.reverse = motor_right_reverse;
        rightInfo.mode = motor_right_mode;

        std::string leftMotorConfig = motor_left_config;
        std::string rightMotorConfig = motor_right_config;

        readDCMotorParameters(vendorDir + "/" + leftMotorConfig, leftInfo);
        readDCMotorParameters(vendorDir + "/" + rightMotorConfig, rightInfo);

        m_distanceBetweenWheels = distance_between_wheels;
        m_wheelRadius = wheel_radius;
        m_maxSpeed = maxSpeed;
        m_maxOmega = maxOmega;
    }
    catch(...) {
        std::cerr << BOOST_CURRENT_FUNCTION << " error" << std::endl;
        return false;
    }

    m_info.insert(std::make_pair(m_motorLeftId, leftInfo));
    m_info.insert(std::make_pair(m_motorRightId, rightInfo));

    vunirobot::task motion_type;
    motion_type.insert(std::make_pair(m_motorLeftId, leftInfo.mode));
    motion_type.insert(std::make_pair(m_motorRightId, rightInfo.mode));
    m_unirobotThread->setMotionType(motion_type);

    vunirobot::dc_motor_parameters_task motor_task;
    motor_task.insert(std::make_pair(m_motorLeftId, *leftInfo.uparams));
    motor_task.insert(std::make_pair(m_motorRightId, *rightInfo.uparams));
    m_unirobotThread->setDCMotorParameters(motor_task);

    m_initUnirobotWorking = true;
    m_initUnirobotThread.reset( new std::thread(&VRobotMotionFacadeImpl::initUnirobot, this));

    m_suspendTime = std::chrono::high_resolution_clock::now( ) - m_suspendPeriod;

    return true;
}

int VRobotMotionFacadeImpl::connectToDevices()
{
    bool result = false;
    try {
        result = m_unirobotThread->connectToPort( UNIROBOT_PORT, 100 );
    }
    catch(...) {
        result = false;
    }
    return result ? 0 : 1;
}

void VRobotMotionFacadeImpl::disconnectFromDevices()
{
    m_unirobotThread->disconnectFromPort();
    std::unique_lock<std::mutex> lock( m_offsetMutex );
    m_previousPositionKnown = false;
    m_positionOffset.x( 0 );
    m_positionOffset.y( 0 );
    m_angleOffset = 0;
}

bool VRobotMotionFacadeImpl::setSpeed(int velocity, int omega)
{
    bool commandIgnored = false;
    if( m_suspendMotion && !m_ignoreOdometryErrors ) {
        commandIgnored = velocity || omega;
        velocity = omega = 0;
    }

    m_unirobotThread->sleep( false );

    std::unique_lock<std::mutex> lock( m_speedMutex );

    // convert from percent
    vreal realVelocity = static_cast<vreal>(velocity) * m_maxSpeed / 100;
    vreal realOmega = static_cast<vreal>(omega) * m_maxOmega / 100;

    vreal leftOmega  = TRANSFORM_TO_LEFT_OMEGA( realVelocity, realOmega, m_distanceBetweenWheels / 2, m_wheelRadius );
    vreal rightOmega = TRANSFORM_TO_RIGHT_OMEGA( realVelocity, realOmega, m_distanceBetweenWheels / 2, m_wheelRadius );

    vreal leftOmegaMax = 1.0 * m_info[m_motorLeftId].motorNominalOmega * 2 * M_PI / m_info[m_motorLeftId].gearRatio / m_info[m_motorLeftId].reductionFactor / 60;
    vreal rightOmegaMax = 1.0 * m_info[m_motorRightId].motorNominalOmega * 2 * M_PI / m_info[m_motorRightId].gearRatio / m_info[m_motorRightId].reductionFactor / 60;

    if(leftOmega > leftOmegaMax) {
        leftOmega = leftOmegaMax;
    }
    else if(leftOmega < -leftOmegaMax) {
        leftOmega = -leftOmegaMax;
    }

    if(rightOmega > rightOmegaMax) {
        rightOmega = rightOmegaMax;
    }
    else if(rightOmega < -rightOmegaMax) {
        rightOmega = -rightOmegaMax;
    }

    int16_t leftPowerInt = convertToUnirobotOmega( leftOmega, m_motorLeftId );
    int16_t rightPowerInt = convertToUnirobotOmega( rightOmega, m_motorRightId );

    vunirobot::task directionTask;
    directionTask.insert(std::make_pair(m_motorLeftId, m_info[m_motorLeftId].reverse ? REVERSE : DIRECT ));
    directionTask.insert(std::make_pair(m_motorRightId, m_info[m_motorRightId].reverse ? REVERSE : DIRECT ));
    m_unirobotThread->setMotorDirection(directionTask);

    vunirobot::DCMotorParameters lparam(*m_info[m_motorLeftId].uparams);
    vunirobot::DCMotorParameters rparam(*m_info[m_motorRightId].uparams);

    if(std::abs(velocity) < 3 && std::abs(omega) < 3) {
        lparam.saturation = 0;
        lparam.saturationDifferential = 0;
        lparam.saturationIntegrated = 0;
        lparam.saturationProportional = 0;

        rparam.saturation = 0;
        rparam.saturationDifferential = 0;
        rparam.saturationIntegrated = 0;
        rparam.saturationProportional = 0;
    }

    vunirobot::dc_motor_parameters_task motor_task;
    motor_task.insert(std::make_pair(m_motorLeftId, lparam));
    motor_task.insert(std::make_pair(m_motorRightId, rparam));
    m_unirobotThread->setDCMotorParameters(motor_task);

    vunirobot::register_type registerLeftOmega = 0;
    vunirobot::register_type registerRightOmega = 0;

    memcpy(&registerLeftOmega, &leftPowerInt, sizeof(vunirobot::register_type));
    memcpy(&registerRightOmega, &rightPowerInt, sizeof(vunirobot::register_type));

    vunirobot::task speedTask;
    speedTask.insert(std::make_pair(m_motorLeftId, registerLeftOmega));
    speedTask.insert(std::make_pair(m_motorRightId, registerRightOmega));
    bool res = m_unirobotThread->setMotorSpeed(speedTask);

    return res && !commandIgnored;
}

int16_t VRobotMotionFacadeImpl::convertToUnirobotOmega( vreal omega, int motorId ) const
{
    return static_cast<int16_t>((omega * m_info.at(motorId).gearRatio * m_info.at(motorId).numberOfTicksPerRevolutionEncoder) / ( 2 * M_PI ));
}

vreal VRobotMotionFacadeImpl::convertFromUnirobotOmega( int omega, int motorId ) const
{
    return omega * ( 2 * M_PI ) / (m_info.at(motorId).gearRatio * m_info.at(motorId).numberOfTicksPerRevolutionEncoder);
}

void VRobotMotionFacadeImpl::readDCMotorParameters(const std::string& config, VRobotMotionFacadeImpl::MotorInfo& info)
{
    boost::property_tree::ptree pt;
    boost::property_tree::xml_parser::read_xml(config, pt);

    auto parameters_node = pt.get_child( "parameters.<xmlattr>" );

    info.uparams->differential = parameters_node.get<int>( "differential" );
    info.uparams->integrated = parameters_node.get<int>( "integrated" );
    info.uparams->proportional = parameters_node.get<int>( "proportional" );
    info.uparams->saturation = parameters_node.get<int>( "saturation" );
    info.uparams->saturationProportional = parameters_node.get<int>( "saturationProportional" );
    info.uparams->saturationIntegrated = parameters_node.get<int>( "saturationIntegrated" );
    info.uparams->saturationDifferential = parameters_node.get<int>( "saturationDifferential" );
    info.uparams->sampling = parameters_node.get<int>( "sampling" );

    info.gearRatio = parameters_node.get<int>( "gear_ratio" );
    info.numberOfTicksPerRevolutionEncoder = parameters_node.get<int>( "number_of_ticks_per_revolution_encoder" );
    info.motorNominalOmega = parameters_node.get<int>( "motor_nominal_omega" );
    info.reductionFactor = parameters_node.get<vreal>( "reduction_factor" );
}

bool VRobotMotionFacadeImpl::getSpeed( vreal& velocity, vreal& omega )
{
    vunirobot::motor_speed speed;
    if( m_unirobotThread->getMotorSpeed(speed) ) {
        vreal leftOmega = convertFromUnirobotOmega( speed[ m_motorLeftId ], m_motorLeftId );
        vreal rightOmega = convertFromUnirobotOmega( speed[ m_motorRightId ], m_motorRightId );
        omega = m_wheelRadius / m_distanceBetweenWheels * ( -rightOmega - leftOmega );
        velocity = m_wheelRadius  * ( -rightOmega + leftOmega ) / 2;
        return true;
    }
    return false;
}

bool VRobotMotionFacadeImpl::getEngineSpeed(vreal &velocity, vreal &omega)
{
    return getSpeed( velocity, omega );
}

bool VRobotMotionFacadeImpl::getPosition(vpointf &point, vreal &phi)
{
    vunirobot::RobotCoordinate coordinate;
    bool result = m_unirobotThread->getRobotCoordinate( coordinate );

    if( result ) {
        std::unique_lock<std::mutex> lock( m_offsetMutex );
        if( m_previousPositionKnown ) {
            auto currentTime = std::chrono::high_resolution_clock::now( );
            if( currentTime - m_suspendTime > m_suspendPeriod && !m_motionResume ) {
                m_suspendMotion = false;
                m_motionResume = true;
            }

            unirobot_state_t stateX = OK;
            long diff = coordinate.coordinate_x + m_positionOffset.x( ) - m_previousPosition.x( );
            long absDiff = std::abs( diff );
            if( absDiff > REBOOT_COORD_THRESHOLD ) {
                if( absDiff <= 2 * MAX_X + REBOOT_COORD_THRESHOLD && absDiff >= 2 * MAX_X - REBOOT_COORD_THRESHOLD ) {
                    stateX = OVERLOAD;
                    m_positionOffset.x( m_positionOffset.x( ) + ( diff < 0 ? 2 * MAX_X : - 2 * MAX_X ) );
                } else {
                    m_positionOffset.x( m_previousPosition.x( ) - coordinate.coordinate_x );
                    if( std::abs( coordinate.coordinate_x ) <= REBOOT_COORD_THRESHOLD ) {
                        stateX = REBOOT;
                    } else {
                        stateX = CRASH;
                    }
                }
            } else if( std::abs( m_previousPosition.x( ) + m_positionOffset.x( ) ) < REBOOT_COORD_THRESHOLD ) {
                stateX = UNCERTAIN;
            }

            unirobot_state_t stateY = OK;
            diff = coordinate.coordinate_y + m_positionOffset.y( ) - m_previousPosition.y( );
            absDiff = std::abs( diff );
            if( absDiff > REBOOT_COORD_THRESHOLD ) {
                if( absDiff <= 2 * MAX_Y + REBOOT_COORD_THRESHOLD && absDiff >= 2 * MAX_Y - REBOOT_COORD_THRESHOLD ) {
                    stateY = OVERLOAD;
                    m_positionOffset.y( m_positionOffset.y( ) + ( diff < 0 ? 2 * MAX_Y : - 2 * MAX_Y ) );
                } else {
                    m_positionOffset.y( m_previousPosition.y( ) - coordinate.coordinate_y );
                    if( std::abs( coordinate.coordinate_y ) <= REBOOT_COORD_THRESHOLD ) {
                        stateY = REBOOT;
                    } else {
                        stateY = CRASH;
                    }
                }
            } else if( std::abs( m_previousPosition.y( ) + m_positionOffset.y( ) ) < REBOOT_COORD_THRESHOLD ) {
                stateY = UNCERTAIN;
            }

            unirobot_state_t stateA = OK;
            diff = coordinate.coordinate_angle + m_angleOffset - m_previousAngle;
            absDiff = std::abs( diff );
            if( absDiff > REBOOT_ANGLE_THRESHOLD ) {
                if( absDiff <= 2 * MAX_ANGLE + REBOOT_ANGLE_THRESHOLD && absDiff >= 2 * MAX_ANGLE - REBOOT_ANGLE_THRESHOLD ) {
                    stateA = OVERLOAD;
                    m_angleOffset += ( diff < 0 ? 2 * MAX_ANGLE : - 2 * MAX_ANGLE );
                } else {
                    m_angleOffset = m_previousAngle - coordinate.coordinate_angle;
                    if( std::abs( coordinate.coordinate_angle ) <= REBOOT_ANGLE_THRESHOLD ) {
                        stateA = REBOOT;
                    } else {
                        stateA = CRASH;
                    }
                }
            } else if( std::abs( m_previousAngle + m_angleOffset ) < REBOOT_ANGLE_THRESHOLD ) {
                stateA = UNCERTAIN;
            }

            result = !rebooted( stateX, stateY, stateA );
            if( !result ) {
                if( currentTime - m_suspendTime > m_suspendPeriod && m_motionResume ) {
                    m_suspendMotion = true;
                }
                m_suspendTime = currentTime;
                m_motionResume = false;
            }

            if( stateX != REBOOT && stateX != CRASH && m_motionResume )
                 m_previousPosition.x( coordinate.coordinate_x + m_positionOffset.x( ) );

            if( stateY != REBOOT && stateY != CRASH && m_motionResume )
                 m_previousPosition.y( coordinate.coordinate_y + m_positionOffset.y( ) );

            if( stateA != REBOOT && stateA != CRASH && m_motionResume )
                 m_previousAngle = coordinate.coordinate_angle + m_angleOffset;

        } else {
            m_previousPosition.x( coordinate.coordinate_x );
            m_previousPosition.y( coordinate.coordinate_y );
            m_previousAngle = coordinate.coordinate_angle;
            m_previousPositionKnown = true;
        }
        phi = m_previousAngle / 1000.0;
        point.x( m_previousPosition.x( ) / 1000.0 );
        point.y( m_previousPosition.y( ) / 1000.0 );
    }

    return result;
}

bool VRobotMotionFacadeImpl::rebooted( unirobot_state_t stateX, unirobot_state_t stateY, unirobot_state_t stateA )
{
    if( ( stateX == REBOOT || stateY == REBOOT || stateA == REBOOT ) ||
        ( stateX == CRASH || stateY == CRASH || stateA == CRASH ) ) {
        return true;
    } else
        return false;
}

void VRobotMotionFacadeImpl::setStartPosition(const vpointf& point, vreal phi)
{
    (void) point;
    (void) phi;

    //Not imlemented.
    assert( false );
}

void VRobotMotionFacadeImpl::ignoreOdometryErrors( bool ignore )
{
    m_ignoreOdometryErrors = ignore;
}

void VRobotMotionFacadeImpl::getDeviceName(std::string &deviceName, int deviceId)
{
    if( deviceId == UNIROBOT ) {
        deviceName = "unirobot";
    }
    else {
        deviceName.clear( );
    }
}

void VRobotMotionFacadeImpl::initUnirobot()
{
    vunirobot::RobotParameters w_params;
    w_params.distanceBetweenWheels = m_distanceBetweenWheels * 1000;
    w_params.encoderTicksPerRotation = m_info.at(m_motorLeftId).numberOfTicksPerRevolutionEncoder;
    w_params.reductionRatio = m_info.at(m_motorLeftId).gearRatio;
    w_params.wheelRadius = m_wheelRadius * 1000;

    vunirobot::RobotParameters r_params;
    bool save = false;
    while(m_initUnirobotWorking) {
        if(save) {
            bool save = m_unirobotThread->saveRobotConfig();
            if(save) {
                std::cout << "Save new robot parameters" << std::endl;
                break;
            }
        }
        bool read = m_unirobotThread->getRobotParameters(r_params);
        if(read) {
            bool comp = (memcmp(&w_params, &r_params, sizeof(vunirobot::RobotParameters)) == 0);
            if(comp) {
                std::cout << "Using old robot parameters" << std::endl;
                break;
            }
            else {
                save = m_unirobotThread->setRobotParameters(w_params);
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
