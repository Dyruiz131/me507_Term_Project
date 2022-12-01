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

extern Share<bool> Stop_Motor1;
extern Share<bool> Stop_Motor2;
extern Share<bool> Begin_Move;
extern Queue<float> Directions_Queue;
extern Share<uint16_t> Steps1;
extern Share<uint16_t> Steps2;
extern Share<float> Avel1;
extern Share<float> Avel2;
extern Share<bool> Piece_Detect;
extern Share<bool> Scan_Board;
extern Share<bool> Motor1_Start;
extern Share<bool> Motor2_Start;
extern Share<bool> Motor1Max_Start;
extern Share<bool> Motor2Max_Start;
extern Share<int8_t> Motor1_dir;
extern Share<int8_t> Motor2_dir;
extern Share<uint32_t> steps_traveled_x;
#endif // _SHARES_H_
