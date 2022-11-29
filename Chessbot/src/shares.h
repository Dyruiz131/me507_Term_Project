/** @file shares.h
 *  This file contains extern declarations of shares and queues which are used
 *  in more than one file of a demonstration project.
 *
 *  @author JR Ridgely
 *  @date   2021-Oct-23 Original file
 *  @copyright (c) 2021 by JR Ridgely, released under the LGPL 3.0.
 */

#ifndef _SHARES_H_
#define _SHARES_H_

#include "taskqueue.h"
#include "taskshare.h"

extern Share<bool> stopMotor1;
extern Share<bool> stopMotor2;
extern Share<bool> beginMove;
extern Queue<float> directionsQueue;
extern Share<uint16_t> steps1;
extern Share<uint16_t> steps2;
extern Share<float> aVel1;
extern Share<float> aVel2;
extern Share<bool> Piece_Detect;
extern Share<bool> scanBoard;
extern Share<bool> startMotor1;
extern Share<bool> startMotor2;
extern Share<bool> startMaxMotor1;
extern Share<bool> startMaxMotor2;
extern Share<int8_t> dirMotor1;
extern Share<int8_t> dirMotor2;
#endif // _SHARES_H_
