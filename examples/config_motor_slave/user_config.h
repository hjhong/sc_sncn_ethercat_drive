/**
 * @file bldc_motor_config.h
 * @brief Motor Control config file (define your motor specifications here)
 * @author Synapticon GmbH <support@synapticon.com>
 */
#pragma once

#include <qei_service.h>
#include <hall_service.h>
#include <motorcontrol_service.h>

/**************************************************
 *********      USER CONFIGURATION       **********
 **************************************************/

/////////////////////////////////////////////
//////  APPLICATION CONFIGURATIONS
////////////////////////////////////////////

#define VDC             48

/////////////////////////////////////////////
//////  GENERAL MOTOR CONFIGURATION
////////////////////////////////////////////

// MOTOR TYPE [BLDC_MOTOR, BDC_MOTOR]
#define MOTOR_TYPE  BLDC_MOTOR

// NUMBER OF POLE PAIRS (if applicable)
#define POLE_PAIRS  2

// WINDING TYPE (if applicable) [STAR_WINDING, DELTA_WINDING]
#define BLDC_WINDING_TYPE   STAR_WINDING

// MOTOR POLARITY [NORMAL_POLARITY, INVERTED_POLARITY]
#define MOTOR_POLARITY      NORMAL_POLARITY

/////////////////////////////////////////////
//////  MOTOR SENSORS CONFIGURATION
///////////////////////////////////////////

// SENSOR USED FOR COMMUTATION (if applicable) [HALL_SENSOR]
#define MOTOR_COMMUTATION_SENSOR   BISS_SENSOR

// SENSOR USED FOR CONTROL FEEDBACK [HALL_SENSOR, QEI_SENSOR, BISS_SENSOR]
#define MOTOR_FEEDBACK_SENSOR      BISS_SENSOR

// TYPE OF INCREMENTAL ENCODER (if applicable) [QEI_WITH_INDEX, QEI_WITH_NO_INDEX]
#define QEI_SENSOR_INDEX_TYPE      QEI_WITH_INDEX

// TYPE OF SIGNAL FOR INCREMENTAL ENCODER (if applicable) [QEI_RS422_SIGNAL, QEI_TTL_SIGNAL]
#define QEI_SENSOR_SIGNAL_TYPE     QEI_RS422_SIGNAL

// RESOLUTION OF YOUR INCREMENTAL ENCODER (if applicable)
#define QEI_SENSOR_RESOLUTION      4000

// POLARITY OF YOUR INCREMENTAL ENCODER (if applicable) [1, -1]
#define QEI_SENSOR_POLARITY         1

//////////////////////////////////////////////
//////  PROTECTION CONFIGURATION
////////////////////////////////////////////
#define I_MAX           60      //maximum tolerable value of phase current (under abnormal conditions)
#define V_DC_MAX        50      //maximum tolerable value of dc-bus voltage (under abnormal conditions)
#define V_DC_MIN        10      //minimum tolerable value of dc-bus voltave (under abnormal conditions)
#define TEMP_BOARD_MAX  100     //maximum tolerable value of board temperature (optional)

//////////////////////////////////////////////
//////  MOTOR COMMUTATION CONFIGURATION
////////////////////////////////////////////

// COMMUTATION LOOP PERIOD (if applicable) [us]
#define COMMUTATION_LOOP_PERIOD     66

// COMMUTATION CW SPIN OFFSET (if applicable) [0:4095]
#define COMMUTATION_OFFSET_CLK      3450

// COMMUTATION CCW SPIN OFFSET (if applicable) [0:4095]
#define COMMUTATION_OFFSET_CCLK     0

///////////////////////////////////////////////
//////  MOTOR CONTROL CONFIGURATION
/////////////////////////////////////////////

// CONTROL LOOP PERIOD [us]
#define CONTROL_LOOP_PERIOD     1000

// PID FOR POSITION CONTROL (if applicable) [will be divided by 10000]
#define POSITION_Kp       100
#define POSITION_Ki       1
#define POSITION_Kd       0

// PID FOR VELOCITY CONTROL (if applicable) [will be divided by 10000]
#define VELOCITY_Kp       667
#define VELOCITY_Ki       200
#define VELOCITY_Kd       0

// PID FOR TORQUE CONTROL (if applicable) [will be divided by 10000]
#define TORQUE_Kp         1000
#define TORQUE_Ki         91
#define TORQUE_Kd         0

/////////////////////////////////////////////////
//////  PROFILES AND LIMITS CONFIGURATION
///////////////////////////////////////////////

// POLARITY OF THE MOVEMENT OF YOUR MOTOR [1,-1]
#define POLARITY           1

// DEFAULT PROFILER SETTINGS FOR PROFILE ETHERCAT DRIVE
#define PROFILE_VELOCITY        1000        // rpm
#define PROFILE_ACCELERATION    2000        // rpm/s
#define PROFILE_DECELERATION    2000        // rpm/s
#define PROFILE_TORQUE_SLOPE    400         // adc_ticks

// PROFILER LIMITIS
#define MAX_POSITION_LIMIT      0x7fffffff        // ticks (max range: 2^30, limited for safe operation)
#define MIN_POSITION_LIMIT      -0x7fffffff       // ticks (min range: -2^30, limited for safe operation)
#define MAX_VELOCITY            7000            // rpm
#define MAX_ACCELERATION        7000            // rpm/s
#define MAX_DECELERATION        7000            // rpm/s
#define MAX_CURRENT_VARIATION   800             // adc_ticks/s
#define MAX_CURRENT             800             // adc_ticks

/*
 *  used for the advanced FOC
 *
 * FIXME Check this new parameters if they have to be updated or if they should be relocated
 */

#define MAX_TORQUE              63  // Nm at 1000 A
#define MAXIMUM_TORQUE          MAX_TORQUE
#define PHASE_RESISTANCE        350000 // uOhm
#define PHASE_INDUCTANCE        1000    // uH

/*
 *  Motor specific offst settings
 */

#define REGEN_V_HIGH          58
#define REGEN_V_LOW           38

#define REGEN_I_HIGH          60
#define REGEN_I_LOW           0

#define PERCENT_CHARGING_FACTOR_MAX  52
#define PERCENT_CHARGING_FACTOR_MIN  1

/*
 * Set the Motor offset and position of the hall state sensors.
 * Unless for adding a new motor or fixing motor settings do not touch this.
 */
#define MOTOR_OFFSET           COMMUTATION_OFFSET_CLK

#define MOTOR_HALL_STATE_1     0
#define MOTOR_HALL_STATE_2     0
#define MOTOR_HALL_STATE_3     0
#define MOTOR_HALL_STATE_4     0
#define MOTOR_HALL_STATE_5     0
#define MOTOR_HALL_STATE_6     0
#define POWER_FACTOR           0
#define RATED_POWER            140       // W
#define RATED_TORQUE           270       // mNm
#define RATED_CURRENT          5         // Amp
#define PEAK_SPEED             9000      // rpm

