/**
 * @file main.c
 * @brief Example Master App to test EtherCAT (on PC)
 * @author Synapticon GmbH <support@synapticon.com>
 */

#define _XOPEN_SOURCE

#include <sncn_ethercat.h>
#include <sncn_slave.h>
#include <ecrt.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/resource.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_UINT16                  0xffff
#define MAX_UINT32                  0xffffffff

#define CMD_NORMAL_OP     0
#define CMD_LIST_SLVAVES  1

/* Index of receiving (in) PDOs */
#define PDO_INDEX_STATUSWORD        0
#define PDO_INDEX_OPMODEDISP        1
#define PDO_INDEX_POSITION_VALUE    2
#define PDO_INDEX_VELOCITY_VALUE    3
#define PDO_INDEX_TORQUE_VALUE      4
#define PDO_INDEX_ADDITIONAL_FEEDBACKSENSOR_VALUE         5
#define PDO_INDEX_TUNING_RESULT         6
#define PDO_INDEX_USER_IN_3         7
#define PDO_INDEX_USER_IN_4         8

/* Index of sending (out) PDOs */
#define PDO_INDEX_CONTROLWORD       0
#define PDO_INDEX_OPMODE            1
#define PDO_INDEX_TORQUE_REQUEST    2
#define PDO_INDEX_POSITION_REQUEST  3
#define PDO_INDEX_VELOCITY_REQUEST  4
#define PDO_INDEX_OFFSET_TORQUE        5
#define PDO_INDEX_TUNING_STATUS        6
#define PDO_INDEX_USER_OUT_3        7
#define PDO_INDEX_USER_OUT_4        8

static int g_running = 1;
static unsigned int sig_alarms  = 0;
static unsigned int user_alarms = 0;

/* set eventually higher priority */


static void set_priority(void)
{
    if (getuid() != 0) {
        fprintf(stderr, "Warning, be root to get higher priority\n");
        return;
    }

    pid_t pid = getpid();
    if (setpriority(PRIO_PROCESS, pid, -19))
        fprintf(stderr, "Warning: Failed to set priority: %s\n",
                strerror(errno));
}

/* Signal handling */

void signal_handler(int signum) {
    switch (signum) {
        case SIGINT:
        case SIGTERM:
            g_running = 0;
            break;
        case SIGALRM:
            sig_alarms++;
            break;
    }
}

static void setup_signal_handler(struct sigaction *sa)
{
    /* setup signal handler */
    sa->sa_handler = signal_handler;
    sigemptyset(&(sa->sa_mask));
    sa->sa_flags = 0;
    if (sigaction(SIGALRM, sa, 0)) {
        fprintf(stderr, "Failed to install signal handler!\n");
        exit(-1);
    }

    if (sigaction(SIGTERM, sa, 0)) {
        fprintf(stderr, "Failed to install signal handler!\n");
        exit(-1);
    }

    if (sigaction(SIGINT, sa, 0)) {
        fprintf(stderr, "Failed to install signal handler!\n");
        exit(-1);
    }
}

static void setup_timer(struct itimerval *tv)
{
    /* setup timer */
    tv->it_interval.tv_sec = 0;
    tv->it_interval.tv_usec = 1000000 / 1000;
    tv->it_value.tv_sec = 0;
    tv->it_value.tv_usec = 1000;
    if (setitimer(ITIMER_REAL, tv, NULL)) {
        fprintf(stderr, "Failed to start timer: %s\n", strerror(errno));
        exit(-1);
    }
}

static void print_usage(char *prog)
{
    char *basename = prog;
    char *c = basename;
    while (*c != '\0') {
        c++;
        if (*c == '/')
            basename = c;
    }

    printf("Usage: %s [-h] [-v] [-n <id>] [-l]\n", basename);
}

static void parse_cmd_line(int argc, char *argv[], int *nodeid, int *command)
{
    const char *optargs = "hVvn:l";

    int opt = 0;

    while ((opt = getopt(argc, argv, optargs)) != -1) {
        switch (opt) {
        case 'n':
            *nodeid = atoi(optarg);
            break;

        case 'l':
            *command = CMD_LIST_SLVAVES;
            break;

        case 'h':
            print_usage(argv[0]);
            exit(0);

        case 'v': /* verbosity */
            /* FIXME set verbosity */

        case 'V': /* version??? */
            /* FIXME print version and exit */
            fprintf(stderr, "Error, option %c not yet implemented - proceeding\n", (char)opt);
            break;

        default:
            fprintf(stderr, "Error, unknown option\n");
            exit(-1);
        }
    }
}

static void list_slaves(SNCN_Master_t *master)
{
    int slave_count = sncn_master_slave_count(master);

    for (int i = 0; i < slave_count; i++) {
        SNCN_Slave_t *slave = sncn_slave_get(master, i);

        SNCN_Slave_Info_t info; 
        sncn_slave_get_info(slave, &info);

        printf("%d: VID: 0x%.4x PID: 0x%.x \"%s\"\n", i, info.vendor_id, info.product_code, info.name);
    }
}

int main(int argc, char *argv[])
{
	int slaveid = 0;
    int command = CMD_NORMAL_OP;

    struct sigaction sa;
    struct itimerval tv;

    /* FIXME use getopt(1) with -h, -v etc. * /
    if (argc > 1) {
        slaveid =  atoi(argv[1]);
    }
    */
    parse_cmd_line(argc, argv, &slaveid, &command);

    FILE *ecatlog = fopen("./ecat.log", "w");

	/* Initialize EtherCAT Master */
    SNCN_Master_t *master = sncn_master_init(0, ecatlog);
    if (master == NULL) {
        fprintf(stderr, "Error, could not initialize master\n");
        return -1;
    }

    if (command == CMD_LIST_SLVAVES) {
        list_slaves(master);
        sncn_master_release(master);
        fclose(ecatlog);
        return 0;
    }

    size_t slave_count = sncn_master_slave_count(master);

    if (slave_count < ((unsigned int)slaveid + 1)) {
        fprintf(stderr, "Error only %lu slaves present, but requested slave index is %d\n",
                slave_count, slaveid);
        return -1;
    }

    if (master == NULL) {
        fprintf(stderr, "Error could not initialize master\n");
        return -1;
    }

    /* only talk to slave 0 */
    SNCN_Slave_t *slave = sncn_slave_get(master, slaveid);
    if (slave == NULL) {
        fprintf(stderr, "Error could not retrieve slave %d", slaveid);
        return -1;
    }

    sncn_master_start(master);
	printf("starting Master application\n");

    set_priority();
    setup_signal_handler(&sa);
    setup_timer(&tv);

    uint32_t     position = 0;
    uint32_t     velocity = 0;
    uint16_t     torque   = 0;
    uint16_t     status   = 0;
    unsigned int received = 0;

    uint32_t     user_1   = 0;
    uint32_t     user_2   = 0;
    uint32_t     user_3   = 0;
    uint32_t     user_4   = 0;

	while(g_running) {
        pause();

        while (sig_alarms != user_alarms) {
            user_alarms++;

            sncn_master_cyclic_function(master);

            received = (unsigned int)sncn_slave_get_in_value(slave, PDO_INDEX_STATUSWORD);
            if (received == status) {
                status = (status >= MAX_UINT16) ? 0 : status + 1;
                sncn_slave_set_out_value(slave, PDO_INDEX_CONTROLWORD, status);
            }

            received = (unsigned int)sncn_slave_get_in_value(slave, PDO_INDEX_TORQUE_VALUE);
            if (received == torque) {
                torque = (torque >= MAX_UINT16) ? 0 : torque + 1;
                sncn_slave_set_out_value(slave, PDO_INDEX_TORQUE_REQUEST, torque);
            }

            received = (unsigned int)sncn_slave_get_in_value(slave, PDO_INDEX_POSITION_VALUE);
            if (received == position) {
                position = (position >= MAX_UINT32) ? 0 : position + 1;
                sncn_slave_set_out_value(slave, PDO_INDEX_POSITION_REQUEST, position);
            }

            received = (unsigned int)sncn_slave_get_in_value(slave, PDO_INDEX_VELOCITY_VALUE);
            if (received == velocity) {
                velocity = (velocity >= MAX_UINT32) ? 0 : velocity + 1;
                sncn_slave_set_out_value(slave, PDO_INDEX_VELOCITY_REQUEST, velocity);
            }

            received = (unsigned int)sncn_slave_get_in_value(slave, PDO_INDEX_ADDITIONAL_FEEDBACKSENSOR_VALUE);
            if (received == user_1) {
                user_1 = (user_1 >= MAX_UINT32) ? 0 : user_1 + 1;
                sncn_slave_set_out_value(slave, PDO_INDEX_OFFSET_TORQUE, user_1);
            }

            received = (unsigned int)sncn_slave_get_in_value(slave, PDO_INDEX_TUNING_RESULT);
            if (received == user_2) {
                user_2 = (user_2 >= MAX_UINT32) ? 0 : user_2 + 1;
                sncn_slave_set_out_value(slave, PDO_INDEX_TUNING_STATUS, user_2);
            }

        }
	}

    sncn_master_release(master);
    fclose(ecatlog);

	return 0;
}


