/**
 * @file state_modes.h
 * @brief Drive mode definitions
 * @author Synapticon GmbH <support@synapticon.com>
*/

#pragma once


/* Manufacturer specific mode -128...-1 optional */

/* Controlword */

//Common for all Modes of Operation (CiA402)

#define SHUTDOWN                0x0006
#define SWITCH_ON               0x000F
#define QUICK_STOP              0x000B
#define CLEAR_FAULT             0x0080

/* Controlword commands (without the leading bits */
#define CMD_SHUTDOWN                 0x0006
#define CMD_SHUTDOWN_MASQ            0x0007
#define CMD_SWITCH_ON                0x0007
#define CMD_SWITCH_ON_MASQ           0x000F
#define CMD_SWITCH_ON_ENABLE         0x000F
#define CMD_SWITCH_ON_ENABLE_MASQ    0x000F
#define CMD_DISABLE_VOLTAGE          0x0000 /* Attention: Bit 1 has to be 0, the others are don't care */
#define CMD_DISABLE_VOLTAGE_MASQ     0x0002
#define CMD_QUICK_STOP               0x0002 /* Bit 2 has to be '0', Bits 0 and 3 are don't care */
#define CMD_QUICK_STOP_MASQ          0x0006 /* Bit 2 has to be '0', Bits 0 and 3 are don't care */
#define CMD_DISABLE_OP               0x0007 /* This (transition 5) is the same as switch on (transition 3) with the same target state */
#define CMD_DISABLE_OP_MASQ          0x000F /* This (transition 5) is the same as switch on (transition 3) with the same target state */
#define CMD_ENABLE_OP                0x000F /* same as switch on + enable */
#define CMD_ENABLE_OP_MASQ           0x000F /* same as switch on + enable */
#define CMD_FAULT_RESET              0x0080
#define CMD_FAULT_RESET_MASQ         0x0080

//Operation Mode specific control words (complies with CiA402)

/* Homing mode */
#define START_HOMING            0x001F
#define HALT_HOMING             0x011F

/* Profile Position Mode */
#define ABSOLUTE_POSITIONING    0x001F
#define RELATIVE_POSITIONING    0x005F   // supported currently
#define STOP_POSITIONING        0x010F

/*Profile Velocity Mode*/
#define HALT_PROFILE_VELOCITY   0x010F

/* Statusword */
//state defined is ORed with current state

#define TARGET_REACHED          0x0400

/* Homing Mode */
#define HOMING_ATTAINED         0x1000

/* Profile Position Mode */
#define SET_POSITION_ACK        0x1000

/* Profile Velocity Mode */
#define TARGET_VELOCITY_REACHED 0x0400

/*Controlword Bits*/
#define SWITCH_ON_CONTROL                 0x1
#define ENABLE_VOLTAGE_CONTROL            0x2
#define QUICK_STOP_CONTROL                0x4
#define ENABLE_OPERATION_CONTROL          0x8
#define OPERATION_MODES_SPECIFIC_CONTROL  0x70  /*3 bits*/
#define FAULT_RESET_CONTROL               0x80
#define HALT_CONTROL                      0x100
#define OPERATION_MODE_SPECIFIC_CONTROL   0x200
#define RESERVED_CONTROL                  0x400
#define MANUFACTURER_SPECIFIC_CONTROL     0xf800

/*Statusword Bits*/
#define READY_TO_SWITCH_ON_STATE          0x1
#define SWITCHED_ON_STATE                 0x2
#define OPERATION_ENABLED_STATE           0x4
#define FAULT_REACTION_ACTIVE_STATE       0x0f
#define FAULT_REACTION_ACTIVE_MASQ        0x4f
#define FAULT_STATE                       0x8
#define FAULT_MASQ                        0x4f
#define VOLTAGE_ENABLED_STATE             0x10
#define QUICK_STOP_STATE                  0x20
#define SWITCH_ON_DISABLED_STATE          0x40
#define WARNING_STATE                     0x80
#define MANUFACTURER_SPECIFIC_STATE       0x100
#define REMOTE_STATE                      0x200
#define TARGET_REACHED_OR_RESERVED_STATE  0x400
#define INTERNAL_LIMIT_ACTIVE_STATE       0x800
#define OPERATION_MODE_SPECIFIC_STATE     0x1000  // 12 CSP/CSV/CST  13
#define MANUFACTURER_SPECIFIC_STATES      0xC000  // 14-15

/* Statusword bit for CSP */
#define SW_CSP_FOLLOWING_ERROR            0x2000
#define SW_CSP_TARGET_POSITION_IGNORED    0x1000
#define SW_FAULT_OVER_CURRENT             0x4000
#define SW_FAULT_UNDER_VOLTAGE            0x8000
#define SW_FAULT_OVER_VOLTAGE             0xC000
#define SW_FAULT_OVER_TEMPERATURE         0x0100

/* List of operation modes, not all modes are supported by this device */
#define OPMODE_NONE    0   /* No operation mode selected */
#define OPMODE_PP      1   /* [O] Profile Position mode */
#define OPMODE_VL      2   /* [O] Velocity mode (frequency converter) */
#define OPMODE_PV      3   /* [O] Profile velocity mode */
#define OPMODE_TQ      4   /* [O] Torque profile mode */
#define OPMODE_HM      6   /* [O] Homing mode */
#define OPMODE_IP      7   /* [O] Interpolated position mode */
#define OPMODE_CSP     8   /* [C] Cyclic synchronous position mode */
#define OPMODE_CSV     9   /* [C] Cyclic synchronous velocity mode */
#define OPMODE_CST     10  /* [C] Cyclic synchronous torque mode */
#define OPMODE_CSTCA   11  /* [O] Cyclic synchronous torque mode with commutation angle */

#define OPMODE_SNCN_TUNING  -128 /* [M] mode for motor tuning stuff */

/* Bitmanipulations */
#define CLEAR_BIT(a, b)     (a & ~b)
#define SET_BIT(a, b)       (a | b)
