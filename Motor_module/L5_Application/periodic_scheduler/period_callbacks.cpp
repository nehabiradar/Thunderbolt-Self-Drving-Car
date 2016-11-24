#include "Motor_Servo_Control.h"
#include "periodic_callback.h"
#include <stdint.h>
#include "io.hpp"
#include "can.h"
#include <stdio.h>
#include "utilities.h"
#include "eint.h"
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

const uint32_t PERIOD_DISPATCHER_TASK_STACK_SIZE_BYTES = (512 * 3);

MASTER_DRIVING_CAR_t rcv_car;
MOTOR_HEARTBEAT_t motor_heartbeat = {0};
can_msg_t msg={ 0 };
dbc_msg_hdr_t msg_hdr;
const uint32_t      MASTER_DRIVING_CAR__MIA_MS = 3000;
const MASTER_DRIVING_CAR_t    MASTER_DRIVING_CAR__MIA_MSG = {STOP,CENTER,MEDIUM};
int stop_count;
int wheel_rotation_count=0;
double RPM_Speed=0;

void RPMSpeed_Func()
{
 wheel_rotation_count++;
}


bool period_init(void)
{
	eint3_enable_port2(5,eint_rising_edge,RPMSpeed_Func);
    CAN_init(can1,100,10,10,NULL, NULL);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(can1);

    rcv_car.MASTER_DRIVE_ENUM =STOP;
    rcv_car.MASTER_SPEED_ENUM =MEDIUM;
    rcv_car.MASTER_STEER_ENUM =CENTER;

    return true;
}

bool period_reg_tlm(void)
{
    return true;
}

void period_1Hz(uint32_t count)
{
    if(CAN_is_bus_off(can1))
    {
        CAN_reset_bus(can1);
    }
    msg_hdr = dbc_encode_MOTOR_HEARTBEAT(msg.data.bytes,&motor_heartbeat);
    msg.msg_id = msg_hdr.mid;
    msg.frame_fields.data_len = msg_hdr.dlc;
    msg.data.qword = msg_hdr.mid;
    CAN_tx(can1, &msg, 0);


    //RPM speed Logic
      RPM_Speed=wheel_rotation_count * 7.5;
      wheel_rotation_count=0;

}
void period_10Hz(uint32_t count)
{
	 if(count<12)
		   {
		       rcv_car.MASTER_DRIVE_ENUM = STOP;
		   }
	 Motor_Servo_Set(rcv_car,RPM_Speed);
}

void period_100Hz(uint32_t count)
{
    while(CAN_rx(can1,&msg,0))
               {
                   dbc_msg_hdr_t msg_header;
                   msg_header.mid=msg.msg_id;
                   msg_header.dlc=msg.frame_fields.data_len;
                   if(msg_header.mid == MASTER_DRIVING_CAR_HDR.mid)
                   {
                       dbc_decode_MASTER_DRIVING_CAR(&rcv_car,msg.data.bytes,&msg_header);
                   }
               }

    if(dbc_handle_mia_MASTER_DRIVING_CAR(&rcv_car,100))
               {
                   LE.setAll(15);
               }
               else
               {
                   LE.setAll(0);
               }
}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
    LE.toggle(4);
}
