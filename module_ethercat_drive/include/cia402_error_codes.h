/**
 * @file cia402_error_codes.h
 * @brief Error codes defined by IEC65800-7-201
 * @author Synapticon GmbH <support@synapticon.com>
*/

#pragma once

#include <stdint.h>

#define ERROR_CODE_DC_LINK_OVER_VOLTAGE             0x3210
#define ERROR_CODE_DC_LINK_UNDER_VOLTAGE            0x3220

#define ERROR_CODE_PHASE_FAILURE_L1                 0x3131
#define ERROR_CODE_PHASE_FAILURE_L2                 0x3132
#define ERROR_CODE_PHASE_FAILURE_L3                 0x3133

#define ERROR_CODE_EXCESS_TEMPERATURE_DEVICE        0x4210

#define ERROR_CODE_SENSOR                           0x7300
#define ERROR_CODE_MOTOR_COMMUTATION                0x7122

#define ERROR_CODE_MOTOR_BLOCKED                    0x7121

/* for all error in this control which could not further specified */
#define ERROR_CODE_CONTROL                          0x8A00

#define ERROR_CODE_COMMUNICATION                    0x7500
