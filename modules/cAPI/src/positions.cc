// This file is part of HPGE, an Haptic Plugin for Game Engines
// -----------------------------------------

// Software License Agreement (BSD License)
// Copyright (c) 2017-2023,
// Copyright © 2017-2019 Gabriel Baud-Bovy (gbaudbovy <at> gmail.com)
// Copyright © 2017-2023 Nicolò Balzarotti (nicolo.balzarotti <at> iit.it)
// Istituto Italiano di Tecnologia (IIT), All rights reserved.
//

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:

// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.

// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.

// 3. Neither the name of HPGE, Istituto Italiano di Tecnologia nor the
// names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "HPGE.h"
#include <atomic>
#include <mutex>

namespace HPGE {
  extern std::map<int,HPGE::objectStr> objects;
  extern std::atomic<bool> initialized;
  std::mutex chai3d_mutex;

  // External frame reference!
  // TODO: Needs a way to protect the access
  double storedPosition [3] {0.0,0.0,0.0};
  double storedVelocity [3] {0.0,0.0,0.0};
  double storedRotation [4] {1.0, 0.0,0.0,0.0};
  double storedProxyPosition [3] {0.0,0.0,0.0};
  double storedForce [3] {0.0,0.0,0.0};

  /// returns true if something changed
  bool updateLerp (void) {
    double ratio = 0.0;
    bool changed = false;
    for (auto &obj : objects) {
      // skip those object without interpolation enabled or that have
      // already reached the destination
      if ((obj.second.interpolate.reached_position &&
	   obj.second.interpolate.reached_rotation) ||
	  ! (obj.second.interpolate.position_enabled ||
	     obj.second.interpolate.rotation_enabled)) {
	continue;
      }
      changed = true;
      // We reach this point only if we have to move the object
      // towards the destination
      obj.second.interpolate.current_cycle += 1;
      // The increment must happen before of the check! (with the
      // strict check)
      if (obj.second.interpolate.current_cycle > // strict
	  obj.second.interpolate.cycles
	  * obj.second.interpolate.overshot) {
	// We have reached the destination (100%)
	obj.second.interpolate.reached_position = true;
	obj.second.interpolate.reached_rotation = true;
	// The next interpolate will start on the current position
	obj.second.interpolate.source_position =
	  obj.second.interpolate.dest_position;
	obj.second.interpolate.source_rotation =
	  obj.second.interpolate.dest_rotation;
	// Reset the counter for the next position update
	obj.second.interpolate.current_cycle = 0;
	continue; // because of the strict comparison, we are already there
      }

      // Compute the percentage of movement we have done
      ratio = (double)obj.second.interpolate.current_cycle /
	(double)obj.second.interpolate.cycles;

      if (obj.second.interpolate.position_enabled) {
	// chai3d_mutex.lock ();
	obj.second.obj -> setLocalPos
	  (chai3d::cLerp(ratio, obj.second.interpolate.source_position.x (),
			 obj.second.interpolate.dest_position.x ()),
	   chai3d::cLerp(ratio, obj.second.interpolate.source_position.y (),
			 obj.second.interpolate.dest_position.y ()),
	   chai3d::cLerp(ratio, obj.second.interpolate.source_position.z (),
			 obj.second.interpolate.dest_position.z ()));
	// chai3d_mutex.unlock ();
      }
      if (obj.second.interpolate.rotation_enabled) {
	chai3d::cQuaternion tmp;
	tmp.slerp(ratio,
		  obj.second.interpolate.source_rotation,
		  obj.second.interpolate.dest_rotation);
	chai3d::cMatrix3d rotmat;
	tmp.toRotMat (rotmat);
	obj.second.obj -> setLocalRot (rotmat);
      }
    }
    return changed;
  }

  extern "C" {
    int get_tool_force (double outputForce [3]) {
      if (! initialized.load ()) {
	outputForce [0] = 0.0;
	outputForce [1] = 0.0;
	outputForce [2] = 0.0;

	return retErr (NOT_INITD);
      } else {
	outputForce [0] = storedForce [0];
	outputForce [1] = storedForce [1];
	outputForce [2] = storedForce [2];

	return retErr(SUCCESS);
      }
    }

    int get_tool_position (double ouptutPosition [3]) {
      if (! initialized.load ()) {
	ouptutPosition [0] = 0.0;
	ouptutPosition [1] = 0.0;
	ouptutPosition [2] = 0.0;

	return retErr (NOT_INITD);
      } else {
	ouptutPosition [0] = storedPosition [0];
	ouptutPosition [1] = storedPosition [1];
	ouptutPosition [2] = storedPosition [2];

	return retErr (SUCCESS);
      }
    }

    int get_tool_rotation (double outputRotation [4]) {
      if (! initialized.load ()) {
	outputRotation [0] = 1.0;
	outputRotation [1] = 0.0;
	outputRotation [2] = 0.0;
	outputRotation [3] = 0.0;

	return retErr (NOT_INITD);
      } else {
	outputRotation [0] = storedRotation [0];
	outputRotation [1] = storedRotation [1];
	outputRotation [2] = storedRotation [2];
	outputRotation [3] = storedRotation [3];

	return retErr (SUCCESS);
      }
    }

    int get_tool_velocity (double outputVelocity [3]) {
      if (! initialized.load ()) {
	outputVelocity [0] = 0.0;
	outputVelocity [1] = 0.0;
	outputVelocity [2] = 0.0;

	return retErr (NOT_INITD);
      } else {
	outputVelocity [0] = storedVelocity [0];
	outputVelocity [1] = storedVelocity [1];
	outputVelocity [2] = storedVelocity [2];

	return retErr (SUCCESS);
      }
    }

    int get_tool_proxy_position (double outputPosition [3]) {
      if (! initialized.load ()) {
	outputPosition [0] = 0.0;
	outputPosition [1] = 0.0;
	outputPosition [2] = 0.0;

	return retErr (NOT_INITD);
      } else {
	outputPosition [0] = storedProxyPosition [0];
	outputPosition [1] = storedProxyPosition [1];
	outputPosition [2] = storedProxyPosition [2];

	return retErr (SUCCESS);
      }
    }
  }
}
