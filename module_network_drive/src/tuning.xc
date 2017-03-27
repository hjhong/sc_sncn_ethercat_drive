/*
 * tuning.xc
 *
 *  Created on: Jul 13, 2015
 *      Author: Synapticon GmbH
 */
#include <tuning.h>
#include <stdio.h>
#include <ctype.h>
#include <state_modes.h>

#if 0
static void brake_shake(interface MotorControlInterface client i_motorcontrol, int torque) {
    const int period = 50;
    i_motorcontrol.set_brake_status(1);
    for (int i=0 ; i<1 ; i++) {
        i_motorcontrol.set_torque(torque);
        delay_milliseconds(period);
        i_motorcontrol.set_torque(-torque);
        delay_milliseconds(period);
    }
    i_motorcontrol.set_torque(0);
}
#endif

/*
 * Function to call while in tuning opmode
 *
 * Assumption: this function is called every 1ms! (like the standard control does)
 *
 * FIXME
 * - get rid of {Upsream,Downstream}ControlData here, the data exchange should exclusively happen
 *   in the calling ethercat_drive_service.
 */
int tuning_handler(
        /* input */  uint16_t    controlword, uint32_t control_extension,
        /* output */ uint16_t    &statusword, uint32_t &tuning_result,
        TuningStatus             &tuning_status,
        MotorcontrolConfig       &motorcontrol_config,
        MotionControlConfig &motion_ctrl_config,
        PositionFeedbackConfig   &pos_feedback_config_1,
        PositionFeedbackConfig   &pos_feedback_config_2,
        int sensor_commutation,
        int sensor_motion_control,
        UpstreamControlData      &upstream_control_data,
        DownstreamControlData    &downstream_control_data,
        client interface PositionVelocityCtrlInterface i_position_control,
        client interface PositionFeedbackInterface ?i_position_feedback_1,
        client interface PositionFeedbackInterface ?i_position_feedback_2
    )
{
    uint8_t status_mux     = statusword & 0xff;
    uint8_t status_display = (statusword >> 8) & 0xff;

    //mux send offsets and other data in the tuning result pdo using the lower bits of statusword
    status_mux++;
    if (status_mux > 18)
        status_mux = 0;
    switch(status_mux) {
    case 0: //send flags
        //convert polarity flag to 0/1
        int motion_polarity = 0;
        if (motion_ctrl_config.polarity == INVERTED_POLARITY) {
            motion_polarity = 1;
        }
        int sensor_polarity = (int)pos_feedback_config_1.polarity;
        if (sensor_commutation == 2) {
            sensor_polarity = (int)pos_feedback_config_2.polarity;
        }
        if (sensor_polarity == INVERTED_POLARITY) {
            sensor_polarity = 1;
        } else {
            sensor_polarity = 0;
        }
        int brake_release_strategy = 0;
        if (motion_ctrl_config.special_brake_release == 1) {
            brake_release_strategy = 1;
        }
        tuning_result = (tuning_status.motorctrl_status<<3)+(sensor_polarity<<2)+(motion_polarity<<1)+tuning_status.brake_flag;
        break;
    case 1: //send offset
        tuning_result = motorcontrol_config.commutation_angle_offset;
        break;
    case 2: //pole pairs
        tuning_result = motorcontrol_config.pole_pairs;
        break;
    case 3: //target
        switch(tuning_status.motorctrl_status) {
        case TUNING_MOTORCTRL_TORQUE:
            tuning_result = downstream_control_data.torque_cmd;
            break;
        case TUNING_MOTORCTRL_VELOCITY:
            tuning_result = downstream_control_data.velocity_cmd;
            break;
        case TUNING_MOTORCTRL_POSITION:
        case TUNING_MOTORCTRL_POSITION_PROFILER:
            tuning_result = downstream_control_data.position_cmd;
            break;
        }
        break;
    case 4: //position limit min
        tuning_result = motion_ctrl_config.min_pos_range_limit;
        break;
    case 5: //position limit max
        tuning_result = motion_ctrl_config.max_pos_range_limit;
        break;
    case 6: //max speed
        tuning_result = motion_ctrl_config.max_motor_speed;
        break;
    case 7: //max torque
        tuning_result = motion_ctrl_config.max_torque;
        break;
    case 8: //P_pos
        tuning_result = motion_ctrl_config.position_kp;
        break;
    case 9: //I_pos
        tuning_result = motion_ctrl_config.position_ki;
        break;
    case 10: //D_pos
        tuning_result = motion_ctrl_config.position_kd;
        break;
    case 11: //integral_limit_pos
        tuning_result = motion_ctrl_config.position_integral_limit;
        break;
    case 12: //
        tuning_result = motion_ctrl_config.velocity_kp;
        break;
    case 13: //
        tuning_result = motion_ctrl_config.velocity_ki;
        break;
    case 14: //
        tuning_result = motion_ctrl_config.velocity_kd;
        break;
    case 15: //
        tuning_result = motion_ctrl_config.velocity_integral_limit;
        break;
    case 16: //fault code
        tuning_result = upstream_control_data.error_status;
        break;
    case 17: //special_brake_release
        tuning_result = motion_ctrl_config.special_brake_release;
        break;
    default: //sensor error
        tuning_result = upstream_control_data.sensor_error;
        break;
    }

    if ((controlword & 0xff) == 'p') { //cyclic position mode
        downstream_control_data.position_cmd = tuning_status.value;

    } else {//command mode
        tuning_status.mode_1 = 0; //default command do nothing

        //check for new command
        if (controlword == 0) { //no mode
            status_display = 0; //reset status display
        } else if (status_display != (controlword & 0xff)) {//it's a new command
            status_display = (controlword & 0xff); //set controlword display to the master
            tuning_status.mode_1   = controlword         & 0xff;
            tuning_status.mode_2   = (controlword >>  8) & 0xff;
            tuning_status.mode_3   = control_extension   & 0xff;
        }

        /* print command */
        if (tuning_status.mode_1 >=32 && tuning_status.mode_1 <= 126) { //mode is a printable ascii char
            if (tuning_status.mode_2 != 0) {
                if (tuning_status.mode_3 != 0) {
                    printf("%c %c %c %d\n", tuning_status.mode_1, tuning_status.mode_2, tuning_status.mode_3, tuning_status.value);
                } else {
                    printf("%c %c %d\n", tuning_status.mode_1, tuning_status.mode_2, tuning_status.value);
                }
            } else {
                printf("%c %d\n", tuning_status.mode_1, tuning_status.value);
            }
        }

        //execute command
        tuning_command(tuning_status,
                motorcontrol_config, motion_ctrl_config, pos_feedback_config_1, pos_feedback_config_2,
                sensor_commutation, sensor_motion_control,
                upstream_control_data, downstream_control_data,
                i_position_control, i_position_feedback_1, i_position_feedback_2);
    }

    //put status display and status mux in statusword
    statusword = ((status_display & 0xff) << 8) | (status_mux & 0xff);

    return 0;
}



void tuning_command(
        TuningStatus             &tuning_status,
        MotorcontrolConfig       &motorcontrol_config,
        MotionControlConfig &motion_ctrl_config,
        PositionFeedbackConfig   &pos_feedback_config_1,
        PositionFeedbackConfig   &pos_feedback_config_2,
        int sensor_commutation,
        int sensor_motion_control,
        UpstreamControlData      &upstream_control_data,
        DownstreamControlData    &downstream_control_data,
        client interface PositionVelocityCtrlInterface i_position_control,
        client interface PositionFeedbackInterface ?i_position_feedback_1,
        client interface PositionFeedbackInterface ?i_position_feedback_2
    )
{

    //repeat
    const int tolerance = 1000;
    if (tuning_status.repeat_flag == 1) {
        if (upstream_control_data.position < (downstream_control_data.position_cmd+tolerance) &&
            upstream_control_data.position > (downstream_control_data.position_cmd-tolerance)) {
            downstream_control_data.position_cmd = -downstream_control_data.position_cmd;
        }
    }

    /* execute command */
    switch(tuning_status.mode_1) {
    //position commands
    case 'p':
        downstream_control_data.offset_torque = 0;
        downstream_control_data.position_cmd = tuning_status.value;
        motion_ctrl_config = i_position_control.get_position_velocity_control_config();
        switch(tuning_status.mode_2)
        {
        //direct command with profile
        case 'p':
                //bug: the first time after one p# command p0 doesn't use the profile; only the way back to zero
                motion_ctrl_config.enable_profiler = 1;
                i_position_control.set_position_velocity_control_config(motion_ctrl_config);
                printf("Go to %d with profile\n", tuning_status.value);
                upstream_control_data = i_position_control.update_control_data(downstream_control_data);
                break;
        //step command (forward and backward)
        case 's':
                switch(tuning_status.mode_3)
                {
                //with profile
                case 'p':
                        motion_ctrl_config.enable_profiler = 1;
                        printf("position cmd: %d to %d with profile\n", tuning_status.value, -tuning_status.value);
                        break;
                //without profile
                default:
                        motion_ctrl_config.enable_profiler = 0;
                        printf("position cmd: %d to %d\n", tuning_status.value, -tuning_status.value);
                        break;
                }
                i_position_control.set_position_velocity_control_config(motion_ctrl_config);
                downstream_control_data.offset_torque = 0;
                downstream_control_data.position_cmd = tuning_status.value;
                upstream_control_data = i_position_control.update_control_data(downstream_control_data);
                delay_milliseconds(1500);
                downstream_control_data.position_cmd = -tuning_status.value;
                upstream_control_data = i_position_control.update_control_data(downstream_control_data);
                delay_milliseconds(1500);
                downstream_control_data.position_cmd = 0;
                upstream_control_data = i_position_control.update_control_data(downstream_control_data);
                break;
        //direct command
        default:
                motion_ctrl_config.enable_profiler = 0;
                i_position_control.set_position_velocity_control_config(motion_ctrl_config);
                printf("Go to %d\n", tuning_status.value);
                upstream_control_data = i_position_control.update_control_data(downstream_control_data);
                break;
        }
        break;

    //repeat
    case 'R':
        if (tuning_status.value) {
            downstream_control_data.position_cmd = upstream_control_data.position+tuning_status.value;
            tuning_status.repeat_flag = 1;
        } else {
            tuning_status.repeat_flag = 0;
        }
        break;

    //set velocity
    case 'v':
        downstream_control_data.velocity_cmd = tuning_status.value;
        upstream_control_data = i_position_control.update_control_data(downstream_control_data);
        printf("set velocity %d\n", downstream_control_data.velocity_cmd);
        break;

    //change pid coefficients
    case 'k':
        motion_ctrl_config = i_position_control.get_position_velocity_control_config();
        switch(tuning_status.mode_2) {
        case 'p': //position
            switch(tuning_status.mode_3) {
            case 'p':
                motion_ctrl_config.position_kp = tuning_status.value;
                break;
            case 'i':
                motion_ctrl_config.position_ki = tuning_status.value;
                break;
            case 'd':
                motion_ctrl_config.position_kd = tuning_status.value;
                break;
            case 'l':
                motion_ctrl_config.position_integral_limit = tuning_status.value;
                break;
            case 'j':
                motion_ctrl_config.moment_of_inertia = tuning_status.value;
                break;
            default:
                printf("Pp:%d Pi:%d Pd:%d Pi lim:%d j:%d\n", motion_ctrl_config.position_kp, motion_ctrl_config.position_ki, motion_ctrl_config.position_kd,
                        motion_ctrl_config.position_integral_limit, motion_ctrl_config.moment_of_inertia);
                break;
            }
            break;
            case 'v': //velocity
                switch(tuning_status.mode_3) {
                case 'p':
                    motion_ctrl_config.velocity_kp = tuning_status.value;
                    break;
                case 'i':
                    motion_ctrl_config.velocity_ki = tuning_status.value;
                    break;
                case 'd':
                    motion_ctrl_config.velocity_kd = tuning_status.value;
                    break;
                case 'l':
                    motion_ctrl_config.velocity_integral_limit = tuning_status.value;
                    break;
                default:
                    printf("Kp:%d Ki:%d Kd:%d\n", motion_ctrl_config.velocity_kp, motion_ctrl_config.velocity_ki, motion_ctrl_config.velocity_kd);
                    break;
                }
                break;
        } /* end mode_2 */
        i_position_control.set_position_velocity_control_config(motion_ctrl_config);
        break;

    //limits
    case 'L':
        motion_ctrl_config = i_position_control.get_position_velocity_control_config();
        switch(tuning_status.mode_2) {
            //max torque
            case 't':
                motion_ctrl_config.max_torque = tuning_status.value;
                motorcontrol_config.max_torque = tuning_status.value;
                i_position_control.set_motorcontrol_config(motorcontrol_config);
                tuning_status.brake_flag = 0;
                tuning_status.motorctrl_status = TUNING_MOTORCTRL_OFF;
                break;
            //max speed
            case 's':
            case 'v':
                motion_ctrl_config.max_motor_speed = tuning_status.value;
                break;
            //max position
            case 'p':
                switch(tuning_status.mode_3) {
                case 'u':
                    motion_ctrl_config.max_pos_range_limit = tuning_status.value;
                    break;
                case 'l':
                    motion_ctrl_config.min_pos_range_limit = tuning_status.value;
                    break;
                default:
                    motion_ctrl_config.max_pos_range_limit = tuning_status.value;
                    motion_ctrl_config.min_pos_range_limit = -tuning_status.value;
                    break;
                }
                break;
        } /* end mode_2 */
        i_position_control.set_position_velocity_control_config(motion_ctrl_config);
        break;

    //enable position control
    case 'e':
        if (tuning_status.value > 0) {
            tuning_status.brake_flag = 1;
            switch(tuning_status.mode_2) {
            case 'p':
                tuning_status.motorctrl_status = TUNING_MOTORCTRL_POSITION;
                upstream_control_data = i_position_control.update_control_data(downstream_control_data);
                downstream_control_data.position_cmd = upstream_control_data.position;
                upstream_control_data = i_position_control.update_control_data(downstream_control_data);
                printf("start position %d\n", downstream_control_data.position_cmd);

                //select profiler
                motion_ctrl_config = i_position_control.get_position_velocity_control_config();
                if (tuning_status.mode_3 == 'p') {
                    motion_ctrl_config.enable_profiler = 1;
                    tuning_status.motorctrl_status = TUNING_MOTORCTRL_POSITION_PROFILER;
                } else {
                    motion_ctrl_config.enable_profiler = 0;
                }
                i_position_control.set_position_velocity_control_config(motion_ctrl_config);

                //select control mode
                switch(tuning_status.value) {
                case 1:
                    i_position_control.enable_position_ctrl(POS_PID_CONTROLLER);
                    printf("simpe PID pos ctrl enabled\n");
                    break;
                case 2:
                    i_position_control.enable_position_ctrl(POS_PID_VELOCITY_CASCADED_CONTROLLER);
                    printf("vel.-cascaded pos ctrl enabled\n");
                    break;
                case 3:
                    i_position_control.enable_position_ctrl(NL_POSITION_CONTROLLER);
                    printf("Nonlinear pos ctrl enabled\n");
                    break;
                default:
                    i_position_control.enable_position_ctrl(motion_ctrl_config.position_control_strategy);
                    printf("%d pos ctrl enabled\n", motion_ctrl_config.position_control_strategy);
                    break;
                }
                break;
            case 'v':
                tuning_status.motorctrl_status = TUNING_MOTORCTRL_VELOCITY;
                downstream_control_data.velocity_cmd = 0;
                i_position_control.enable_velocity_ctrl();
                printf("velocity ctrl enabled\n");
                break;
            case 't':
                tuning_status.motorctrl_status = TUNING_MOTORCTRL_TORQUE;
                downstream_control_data.torque_cmd = 0;
                i_position_control.enable_torque_ctrl();
                printf("torque ctrl enabled\n");
                break;
            }
        } else {
            tuning_status.brake_flag = 0;
            tuning_status.repeat_flag = 0;
            tuning_status.motorctrl_status = TUNING_MOTORCTRL_OFF;
            i_position_control.disable();
            printf("position ctrl disabled\n");
        }
        break;

    //pole pairs
    case 'P':
        if (sensor_commutation == 2) {
            if (!isnull(i_position_feedback_2)) {
                pos_feedback_config_2.pole_pairs = tuning_status.value;
                i_position_feedback_2.set_config(pos_feedback_config_2);
            }
        } else {
            if (!isnull(i_position_feedback_1)) {
                pos_feedback_config_1.pole_pairs = tuning_status.value;
                i_position_feedback_1.set_config(pos_feedback_config_1);
            }
        }
        motorcontrol_config.pole_pairs = tuning_status.value;
        tuning_status.brake_flag = 0;
        tuning_status.motorctrl_status = TUNING_MOTORCTRL_OFF;
        i_position_control.set_motorcontrol_config(motorcontrol_config);
        break;

    //direction
    case 'd':
        motion_ctrl_config = i_position_control.get_position_velocity_control_config();
        if (motion_ctrl_config.polarity == INVERTED_POLARITY) {
            motion_ctrl_config.polarity = NORMAL_POLARITY;
        } else {
            motion_ctrl_config.polarity = INVERTED_POLARITY;
        }
        i_position_control.set_position_velocity_control_config(motion_ctrl_config);
        break;

    //sensor polarity
    case 's':
        if (sensor_commutation == 2) {
            if (!isnull(i_position_feedback_2)) {
                if (pos_feedback_config_2.polarity == NORMAL_POLARITY) {
                    pos_feedback_config_2.polarity = INVERTED_POLARITY;
                } else {
                    pos_feedback_config_2.polarity = NORMAL_POLARITY;
                }
                i_position_feedback_2.set_config(pos_feedback_config_2);
            }
        } else {
            if (!isnull(i_position_feedback_1)) {
                if (pos_feedback_config_1.polarity == NORMAL_POLARITY) {
                    pos_feedback_config_1.polarity = INVERTED_POLARITY;
                } else {
                    pos_feedback_config_1.polarity = NORMAL_POLARITY;
                }
                i_position_feedback_1.set_config(pos_feedback_config_1);
            }
        }
        break;

    //auto offset tuning
    case 'a':
        tuning_status.motorctrl_status = TUNING_MOTORCTRL_OFF;
        tuning_status.brake_flag = 0;
        motorcontrol_config = i_position_control.set_offset_detection_enabled();
        break;

    //set offset
    case 'o':
        tuning_status.brake_flag = 0;
        tuning_status.motorctrl_status = TUNING_MOTORCTRL_OFF;
        motorcontrol_config.commutation_angle_offset = tuning_status.value;
        i_position_control.set_motorcontrol_config(motorcontrol_config);
        printf("set offset to %d\n", tuning_status.value);
        break;

    //set brake
    case 'b':
        switch(tuning_status.mode_2) {
        case 's': //toggle special brake release
            motion_ctrl_config = i_position_control.get_position_velocity_control_config();
            motion_ctrl_config.special_brake_release = tuning_status.value;
            i_position_control.set_position_velocity_control_config(motion_ctrl_config);
            break;
        default:
            if (tuning_status.brake_flag == 0 || tuning_status.value == 1) {
                tuning_status.brake_flag = 1;
                printf("Brake released\n");
            } else {
                tuning_status.brake_flag = 0;
                printf("Brake blocking\n");
            }
            i_position_control.set_brake_status(tuning_status.brake_flag);
            break;
        } /* end mode_2 */
        break;

    //set zero position
    case 'z':
        if (sensor_motion_control == 2) {
            if (!isnull(i_position_feedback_2)) {
                switch(tuning_status.mode_2) {
                case 'z':
                    i_position_feedback_2.send_command(REM_16MT_CONF_NULL, 0, 0);
                    break;
                default:
                    i_position_feedback_2.send_command(REM_16MT_CONF_MTPRESET, tuning_status.value, 16);
                    break;
                }
                i_position_feedback_2.send_command(REM_16MT_CTRL_SAVE, 0, 0);
                i_position_feedback_2.send_command(REM_16MT_CTRL_RESET, 0, 0);
            }
        } else {
            if (!isnull(i_position_feedback_1)) {
                switch(tuning_status.mode_2) {
                case 'z':
                    i_position_feedback_1.send_command(REM_16MT_CONF_NULL, 0, 0);
                    break;
                default:
                    i_position_feedback_1.send_command(REM_16MT_CONF_MTPRESET, tuning_status.value, 16);
                    break;
                }
                i_position_feedback_1.send_command(REM_16MT_CTRL_SAVE, 0, 0);
                i_position_feedback_1.send_command(REM_16MT_CTRL_RESET, 0, 0);
            }
        }
        break;

    //reverse torque
    case 'r':
        switch(tuning_status.motorctrl_status) {
        case TUNING_MOTORCTRL_TORQUE:
            downstream_control_data.torque_cmd = -downstream_control_data.torque_cmd;
            printf("Torque %d\n", downstream_control_data.torque_cmd);
            break;
        case TUNING_MOTORCTRL_VELOCITY:
            downstream_control_data.velocity_cmd = -downstream_control_data.velocity_cmd;
            printf("Velocity %d\n", downstream_control_data.velocity_cmd);
            break;
        }
        break;

    //set torque
    case '@':
        //switch to torque control mode
        if (tuning_status.motorctrl_status != TUNING_MOTORCTRL_TORQUE) {
            tuning_status.brake_flag = 1;
            tuning_status.repeat_flag = 0;
            tuning_status.motorctrl_status = TUNING_MOTORCTRL_TORQUE;
            i_position_control.enable_torque_ctrl();
            printf("switch to torque control mode\n");
        }
        //release the brake
        if (tuning_status.brake_flag == 0) {
            tuning_status.brake_flag = 1;
            i_position_control.set_brake_status(tuning_status.brake_flag);
        }
        downstream_control_data.torque_cmd = tuning_status.value;
        upstream_control_data = i_position_control.update_control_data(downstream_control_data);
        break;
    } /* main switch */
}
