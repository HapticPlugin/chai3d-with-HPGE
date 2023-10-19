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
#include <thread>
#include <chrono>

namespace HPGE {
  // `sleep_ms` is used to check if the loop has stopped
  const int sleep_ms = 2;
  // The thread that will run the haptic loop
  chai3d::cThread* hapticsThread = nullptr;
  extern double toolRadius;           // world.cc
  // a virtual tool representing the haptic device in the scene
  chai3d::cToolCursor * tool = nullptr; // init'd in initialize()
  // a pointer to the current haptic device
  chai3d::cGenericHapticDevicePtr hapticDevice; // init'd in initialize()
  // a haptic device handler
  chai3d::cHapticDeviceHandler handler = chai3d::cHapticDeviceHandler ();
  // get spec of haptic device
  chai3d::cHapticDeviceInfo hapticDeviceInfo;

  extern double workspaceScaleFactor; // world.cc
  extern double maxStiffness;         // world.cc
  extern double maxDamping;           // world.cc
  extern double maxForce;             // world.cc

  // State variable describing the state of the simulation
  // initialized: true if device is initialized
  std::atomic<bool> initialized {false};
  std::atomic<bool> running {false};
  std::atomic<bool> stopped {false};
  extern std::atomic<bool> logging; // logging.cc
  std::mutex objects_mutex;
  extern std::mutex hook_mutex; // hooks.cc
  extern std::mutex world_mutex; // world.cc
  extern std::map<int,HPGE::objectStr> objects;
  extern std::atomic<int> id;
  extern std::mutex data_mutex;

  extern ForceHookPtr force_hook_fn;       // hooks.cc
  extern std::atomic<bool> hook_use_proxy; // hooks.cc
  extern chai3d::cWorld * world;           // world.cc

  // logging.cc
  extern std::array<SavedData*,RECORDING_DATA_BUFFER_SIZE> Data;
  extern SavedData * current_data;
  extern Logging log;

  // External frame reference!
  extern double storedPosition [3];      // position.cc
  extern double storedVelocity [3];      // position.cc
  extern double storedRotation [4];      // position.cc
  extern double storedProxyPosition [3]; // position.cc
  extern double storedForce [3];        // position.cc
  // Frequency as Hz (1.0 second)
  chai3d::cFrequencyCounter frequencyCounter (1.0);
  std::atomic<int> loop {0};
   // increased by external commands only. Reset on `start()`
  std::atomic<unsigned int> ticks {0};

  double loopFrequency {0.0};
  // The `wait_for_small_forces` default is used in `stop()` too. If
  // changed here, change it there too.
  bool wait_for_small_forces {false};
  bool raise_forces {false};

  void resetClock (void) {
    frequencyCounter.reset ();
    loopFrequency = 0.0;
  }


  void updateDeviceInformation (void) {
    chai3d::cVector3d proxyPosition;
    chai3d::cVector3d devicePosition;
    chai3d::cVector3d deviceVelocity;
    chai3d::cMatrix3d deviceRotationMat;
    chai3d::cVector3d deviceForce;
    // Ask the device the actual position etc.
    tool -> updateFromDevice ();

    // Update positions, rotation, velocity
    proxyPosition = tool -> m_hapticPoint -> getGlobalPosProxy ();
    devicePosition = tool -> getDeviceGlobalPos ();
    deviceForce = tool -> getDeviceGlobalForce ();
    // deviceVelocity = chai3d::cVector3d ();

    deviceVelocity = tool -> getDeviceLocalLinVel ();
    deviceRotationMat = tool -> getDeviceGlobalRot ();
    chai3d::cQuaternion deviceRotationQuat;
    deviceRotationQuat.fromRotMat (deviceRotationMat);

    // Used by getters and by the hook

    // Convert position, rotation, force and velocity in the
    // caller coordinates system so that `get_tool_(proxy_)*` can
    // access them

    // TODO: It would be more efficient to do this only if hook is
    // active, or upon read, but only once per frame...
    RotFromChai (deviceRotationQuat, storedRotation);
    PosFromChai (proxyPosition, storedProxyPosition);
    PosFromChai (devicePosition, storedPosition);
    PosFromChai (deviceVelocity, storedVelocity);
    PosFromChai (deviceForce, storedForce);
  }

  void hapticLoop (void) {
    resetClock ();
    loop.store (0);

    stopped.store (false);
    while (running.load ()) {
      double hook_force [3] {0.0,0.0,0.0};

      loopFrequency = frequencyCounter.signal (1);
      loop.fetch_add (1);

      // Update object position (to allow force computing)
      objects_mutex.lock ();
      bool updated = updateLerp ();
      objects_mutex.unlock ();

      hook_mutex.lock ();
      // Run the hook, if exists
      if (force_hook_fn != nullptr) {
	double external_force [3] { 0.0, 0.0, 0.0 };
	(* force_hook_fn) (hook_use_proxy.load () ?
			   storedProxyPosition : storedPosition,
			   storedVelocity,
			   external_force);

	hook_force [0] = external_force [0];
	hook_force [1] = external_force [1];
	hook_force [2] = external_force [2];
      }
      hook_mutex.unlock ();

      world_mutex.lock ();
      // if position or roation has been updated,
      // force (by setting the parameter to false) to compute vertex positions
      world -> computeGlobalPositions (updated);
      updateDeviceInformation ();

      // compute interaction forces
      tool -> computeInteractionForces ();
      world_mutex.unlock ();
      // Add the external-hook force
      tool -> addDeviceGlobalForce (hook_force [0],
				    hook_force [1],
				    hook_force [2]);

      // send forces to haptic device
      tool -> applyToDevice ();


      // Store data, if needed
      SavedFrame thisframe;
      if(logging.load() &&
	 // requires custom HPGE
	 (loop % log.sampling_rate == 0)) {
	chai3d::cVector3d devicePos;
	chai3d::cVector3d deviceVel;
	chai3d::cVector3d deviceFor;

	// Get information (position, velocity and force) in the
	// same coordinates and units as the real device.
	hapticDevice -> getPosition (devicePos);
	hapticDevice -> getLinearVelocity(deviceVel);
	hapticDevice -> getForce (deviceFor);

	if(log.device_coordinates) {
	  thisframe = { Now (), ticks.load (),
			{ devicePos.x (),
			  devicePos.y (),
			  devicePos.z ()
			},
			{ deviceVel.x (),
			  deviceVel.y (),
			  deviceVel.z ()
			},
			{ deviceFor.x (),
			  deviceFor.y (),
			  deviceFor.z ()
			},
			nullptr,
			"auto",
	  };
	} else {
	  // Get transformed coordinates: same axis and units as the
	  // caller
	  thisframe = { Now(), ticks.load (),
			{ storedPosition [0],
			  storedPosition [1],
			  storedPosition [2]
			},
			{ storedVelocity [0],
			  storedVelocity [1],
			  storedVelocity [2]
			},
			{
			  storedForce [0],
			  storedForce [1],
			  storedForce [2]
			},
			nullptr,
		        "auto"};
	}

	data_mutex.lock ();
	current_data -> frames.push_back (thisframe);
	data_mutex.unlock ();
      }
    }
    tool -> setForcesOFF ();
    
    stopped.store (true);
  }

  void waitForSmallForces (bool enable) {
    // Since this affects early behavior, we can change this both
    // online and offline

    // If it's running, change it online
    if (initialized.load ()) {
      wait_for_small_forces = enable;
      tool -> setWaitForSmallForce (enable);
    } else { // If not running, will be used on initialize
      wait_for_small_forces = enable;
      raise_forces = ! enable;
    }
  }

  // WARNING: Untested
  void riseForces (bool enable) {
    if(initialized.load()) {
      raise_forces = enable;
      wait_for_small_forces = ! enable;
      tool -> setWaitForSmallForce (wait_for_small_forces);
      tool -> setUseForceRise (raise_forces);
    } else {
      raise_forces = enable;
      wait_for_small_forces = ! enable;
    }
  }

  extern "C" {
    int count_devices (void) {
      if(initialized.load()) {
	// store the error
	retErr(CANT_BE_INITD);
	// return a negative value (saying that we have no devices)
	return -1;
      }

      handler.update ();
      return handler.getNumDevices ();
    }
    int get_device_name (const int deviceId,
			 int buffer_size, char * name) {
      int foundDevices = count_devices();
      if(deviceId > foundDevices) {
	return retErr (DEVICE_NOT_FOUND);
      }

      chai3d::cGenericHapticDevicePtr tmpDevice;
      chai3d::cHapticDeviceInfo tmpInfo;

      if (!handler.getDevice (tmpDevice, deviceId + 1)) {
	return retErr (DEVICE_NOT_FOUND);
      }

      tmpInfo = tmpDevice -> getSpecifications ();
      tmpDevice -> close ();

      std::string msg = tmpInfo.m_modelName;
      // copy of the code in errors.cc, might be refactored
      if (static_cast<int> (msg.length ()) + 1 <= buffer_size) {
	std::copy (msg.begin (), msg.end (), name);
	// don't forget the terminating 0
	name [msg.size ()] = '\0';
	return retErr (SUCCESS);
      }
      return retErr (BUFFER_TOO_SMALL);
    }

    int initialize (int deviceId, double hapticScale, double radius) {
      if (initialized.load ()) { return retErr (ALREADY_INITD); }
      // This should not be possible! (unless we support multiple devices)
      if (running.load ()) { return retErr (ALREADY_RUNNING); }

      assert(count_devices () != -1);

      // 1 is the offset created by the CMockDevice, that is
      // always present
      if (!handler.getDevice (hapticDevice, deviceId + 1)) {
	return retErr (DEVICE_NOT_FOUND);
      }
      hapticDeviceInfo = hapticDevice -> getSpecifications ();

      world = new chai3d::cWorld ();
      // create a 3D tool and add it to the world
      tool = new chai3d::cToolCursor (world);
      world -> addChild (tool);
      tool -> setHapticDevice (hapticDevice);

      toolRadius = radius;
      tool -> setRadius (radius);
      // FIXME: do we need it?
      tool -> setWorkspaceScaleFactor (hapticScale * 10);
      tool -> enableDynamicObjects (true);
      // tool -> start ();
      workspaceScaleFactor = tool -> getWorkspaceScaleFactor ();

      // don't divide by workspace scale to keep stiffness invariant to scale change
      maxStiffness = hapticDeviceInfo.m_maxLinearStiffness
	/ workspaceScaleFactor;
      maxDamping = hapticDeviceInfo.m_maxLinearDamping
	/ workspaceScaleFactor;
      maxForce = hapticDeviceInfo.m_maxLinearForce;

      tool -> setWaitForSmallForce (wait_for_small_forces);
      tool -> setUseForceRise (raise_forces);
      tool -> start ();

      // Set initial device position
      updateDeviceInformation (); // requires tool
      world -> computeGlobalPositions (false);

      initialized.store (true);
      stopped.store (false);
      return retErr (SUCCESS);
    }

    int deinitialize (void) {
      if (!initialized.load ()) {
	return retErr (NOT_INITD);
      }

      // If running, try to stop. Forward errors
      if (running.load ()) {
	ErrorMsg stop_status = (ErrorMsg) stop ();
	if (stop_status != SUCCESS) {
	  // This should not ever happen. If it does, it's a bug
	  return retErr (stop_status);
	}
	// Else it will segfault, so signal it's a programming error
	assert(stopped.load () == true);
      }

      tool -> setForcesOFF ();
      tool -> stop ();
      hapticDevice -> close ();

      world -> deleteAllChildren ();

      //delete world;
      world = nullptr;
      //delete handler;
      //handler = nullptr;

      id.store (0);

      objects.clear ();

      initialized.store (false);

      // If we need to `delete` something we can do it here
      // for (auto &obj : objects) {
      // }

      return retErr (SUCCESS);
    }

    int start (void) {
      if (!initialized.load ()) {
	return retErr (NOT_INITD);
      }
      if (running.load ()) {
	return retErr (ALREADY_RUNNING);
      }
      ticks.store (0);

      hapticsThread = new chai3d::cThread ();

      running.store (true); // The loop will run until this is true
      stopped.store (true); // The loop will change this, telling us that's started

      hapticsThread -> start (hapticLoop, chai3d::CTHREAD_PRIORITY_HAPTICS);

      // Don't return until thread started
      while (stopped.load ()) {
	std::this_thread::sleep_for (std::chrono::milliseconds (sleep_ms));
      }

      return retErr (SUCCESS);
    }

    int stop (void) {
      if (!initialized.load ()) {
	return retErr (NOT_INITD);
      }

      if (!running.load ()) {
	return retErr (NOT_RUNNING);
      }

      running.store (false);

      while (! stopped.load ()) {
	std::this_thread::sleep_for (std::chrono::milliseconds (sleep_ms));
      }

      // Do not call the force hook next time the loop is started!
      // It might get deleted in the meanwhile
      force_hook_fn = nullptr;

      // restore default to prevent strange behavior
      wait_for_small_forces = false;
      raise_forces = false;

      return retErr (SUCCESS);
    }

    // The tick is increased only if the thread is running
    void tick (void) {
      if (!running.load ()) { return; }
      ticks.fetch_add (1);
    }

    // Add a line to the log, containing this string
    void log_annotate (char * string) { // null-terminated
	if (!running.load ()) { return; }

	SavedFrame thisframe;
	auto note = std::string(string);
	if(log.device_coordinates) {
	    chai3d::cVector3d devicePos;
	    chai3d::cVector3d deviceVel;
	    chai3d::cVector3d deviceFor;

	    // Get information (position, velocity and force) in the
	    // same coordinates and units as the real device.
	    hapticDevice -> getPosition (devicePos);
	    hapticDevice -> getLinearVelocity(deviceVel);
	    hapticDevice -> getForce (deviceFor);

	    thisframe = { Now (), ticks.load (),
	    { devicePos.x (),
		devicePos.y (),
		devicePos.z ()
	    },
	    { deviceVel.x (),
		deviceVel.y (),
		deviceVel.z ()
	    },
	    { deviceFor.x (),
		deviceFor.y (),
		deviceFor.z ()
	    },
		nullptr,
		note,
	    };
	}
	else {
	    // Get transformed coordinates: same axis and units as the
	    // caller
	    thisframe = { Now(), ticks.load(),
	    { storedPosition [0],
		storedPosition [1],
		storedPosition [2]
	    },
	    { storedVelocity [0],
		storedVelocity [1],
		storedVelocity [2]
	    },
	    {
		storedForce [0],
		storedForce [1],
		storedForce [2]
	     },
	    nullptr,
	    std::string(string)
	    };
	}
	data_mutex.lock ();
	current_data -> frames.push_back(thisframe);
	data_mutex.unlock ();
	return;
    }

    // Returns the current log frame number
    long get_log_frame (void) {
	if (!logging.load()) return -1;
	data_mutex.lock ();
	auto res = current_data -> frames.size();
	data_mutex.unlock ();
	return res;
    }

    int is_initialized (void) {
      if (initialized.load ()) {
	return retErr (SUCCESS);
      }
      return retErr (NOT_INITD);
    }

    int is_running (void) {
      if (running.load ()) {
	return retErr (SUCCESS);
      }
      return retErr (NOT_RUNNING);
    }

    // it's pressed if it's == 0, else is undefined
    // either an error or not pressed
    int is_tool_button_pressed (const int buttonId) {
      if (!initialized.load ()) { return -1; };
      // FIXME: should we take the world lock?
      return tool -> getUserSwitch (buttonId) ? 0 : 1;
    }

    void enable_wait_for_small_forces (void) { waitForSmallForces (true); }
    void disable_wait_for_small_forces (void) { waitForSmallForces (false); }
    void enable_rise_forces (void) { riseForces (true); }
    void disable_rise_forces (void) { riseForces (false); }

    double get_loop_frequency (void) {
      if (!running.load ()) { return -1.0; }
      return frequencyCounter.getFrequency ();
    }

    int get_loops (void) {
      if (!running.load ()) { return -1; }
      return loop.load ();
    }
  }
}
