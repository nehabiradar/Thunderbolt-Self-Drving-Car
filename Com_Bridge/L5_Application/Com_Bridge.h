/*
 * Com_Bridge.h
 *
 *  Created on: Nov 27, 2016
 *      Author: Admin
 */

#ifndef L5_APPLICATION_COM_BRIDGE_H_
#define L5_APPLICATION_COM_BRIDGE_H_


#include <stdio.h>
#include <stdint.h>
#include "io.hpp"
#include "_can_dbc/generated_can.h"
#include "can.h"

void init_periodic();

void Heartbeat();

void DummyData(uint32_t count);

void CAN_Receive();

void Handle_MiaMsg();

void MotorDecode(uint32_t count);

void CompassDecode(uint32_t count);

void GPSDecode();

void MasterDecode(uint32_t count);

void StartStopCheckpoint();

void setupBT();

void BT(void *p);

#endif /* L5_APPLICATION_COM_BRIDGE_H_ */