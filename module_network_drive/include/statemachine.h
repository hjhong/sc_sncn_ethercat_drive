/**
 * @file statemachine.h
 * @brief Motor Drive defines and configurations
 * @author Synapticon GmbH <support@synapticon.com>
*/

#pragma once

#include <stdint.h>
#include <motion_control_service.h>

#define CTRL_QUICK_STOP_INIT           0x01
#define CTRL_QUICK_STOP_FINISHED       0x02
#define CTRL_COMMUNICATION_TIMEOUT     0x8000
#define CTRL_FAULT_REACTION_FINISHED   0x4000

typedef int bool;
#define true 1
#define false 0

//#define S_NOT_READY_TO_SWITCH_ON    1
//#define S_SWITCH_ON_DISABLED        2
//#define S_READY_TO_SWITCH_ON        7
//#define S_SWITCH_ON                 3
//#define S_OPERATION_ENABLE          4
//#define S_FAULT                     5
//#define S_QUICK_STOP                6

typedef struct S_Check_list {
    bool fault;
} check_list;

typedef enum e_States {
    S_NOT_READY_TO_SWITCH_ON = 1,
    S_SWITCH_ON_DISABLED = 2,
    S_READY_TO_SWITCH_ON = 3,
    S_SWITCH_ON = 4,
    S_OPERATION_ENABLE = 5,
    S_QUICK_STOP_ACTIVE = 6,
    S_FAULT_REACTION_ACTIVE = 8,
    S_FAULT = 7
} states;

typedef states DriveState_t;

bool __check_bdc_init(chanend c_signal);

int init_state(void);

/**
 * @brief Initialize checklist params
 *
 * @return check_list initialized checklist parameters
 */
check_list init_checklist(void);

/**
 * @brief Update Checklist
 *
 * @param check_list_param Check List to be updated
 * @param mode sets mode of operation
 * @param fault sets fault
 */

void update_checklist(check_list &check_list_param, int mode, int fault);

int16_t update_statusword(int current_status, DriveState_t state_reached, int ack, int q_active, int shutdown_ack);

int get_next_state(int in_state, check_list &checklist, int controlword, int localcontrol);

int read_controlword_switch_on(int control_word);

int read_controlword_quick_stop(int control_word);

int read_controlword_enable_op(int control_word);

int read_controlword_fault_reset(int control_word);

