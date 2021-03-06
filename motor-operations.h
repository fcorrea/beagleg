/* -*- mode: c; c-basic-offset: 2; indent-tabs-mode: nil; -*-
 * (c) 2013, 2014 Henner Zeller <h.zeller@acm.org>
 *
 * This file is part of BeagleG. http://github.com/hzeller/beagleg
 *
 * BeagleG is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BeagleG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BeagleG.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _BEAGLEG_MOTOR_OPERATIONS_H_
#define _BEAGLEG_MOTOR_OPERATIONS_H_

#include <stdio.h>

struct MotionQueue;

enum {
  BEAGLEG_NUM_MOTORS = 8
};

// The movement command send to motor operations either changes speed, or
// provides a steady speed.
struct MotorMovement {
  // Speed is steps/s. If initial speed and final speed differ, the motor will
  // accelerate or decelerate to reach the final speed within the given number of
  // alotted steps of the axis with the most number of steps; all other axes are
  // scaled accordingly. Uses jerk-settings to increase/decrease acceleration; the
  // acceleration is zero at the end of the move.
  float v0;     // initial speed
  float v1;     // final speed
  
  // Bits that are set in parallel with the motor control that should be
  // set at the beginning of the motor movement.
  unsigned char aux_bits;   // Aux-bits to switch.

  int steps[BEAGLEG_NUM_MOTORS]; // Steps for axis. Negative for reverse.
};

struct MotorOperations {
  void *user_data;
  
  // Waits for the queue to be empty and Enables/disables motors according to the
  // given boolean value (Right now, motors cannot be individually addressed).
  void (*motor_enable)(void *user, char on);
  
  // Enqueue a coordinated move command.
  // If there is space in the ringbuffer, this function returns immediately,
  // otherwise it waits until a slot frees up.
  // Returns 0 on success, 1 if this is a no-op with no steps to move and 2 on
  // invalid parameters.
  // If "err_stream" is non-NULL, prints error message there.
  // Automatically enables motors if not already.
  int (*enqueue)(void *user, const struct MotorMovement *param, FILE *err_stream);

  // Wait, until all elements in the ring-buffer are consumed.
  void (*wait_queue_empty)(void *user);
};

// Initialize beagleg motor operations.
// The MotorOperations struct is initialized with functions to enqueue MotorMovement requests.
// The implementation connects that with the MotionQueue that accepts lower level
// MotionSegments. MotionQueue is our backend.
//
// Returns 0 on success, != 0 on some error.
int beagleg_init_motor_ops(struct MotionQueue *backend,
                           struct MotorOperations *control);

#endif  // _BEAGLEG_MOTOR_OPERATIONS_H_
