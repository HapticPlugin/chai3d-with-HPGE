// This file is part of HPGE, an Haptic Plugin for Game Engines
// -----------------------------------------

// Software License Agreement (BSD License)
// Copyright (c) 2017-2023,
// Copyright © 2017-2019 Gabriel Baud-Bovy (gabriel.baud-bovy <at> iit.it)
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
#include "versioninfo.hh"
#include <atomic>

namespace HPGE {
  ErrorMsg errors [ERRORS_BUFFER_SIZE_INT] = { SUCCESS };
  const std::map<int, std::string> ErrorMsgStrings {
    { SUCCESS, "Success"}
    , { INVALID_ERROR_NUM, "Fail: The error you are trying to access does not exists!" }
    , { GENERIC_FAIL, "Fail: unknown error" }
    , { NOT_IMPLEMENTED, "This function hasn't been implemented (yet)" }
    , { IS_RELEASE, "Compiled as release, not as debug"}
    , { BUFFER_TOO_SMALL, "Fail: The allocation buffer you provided is too small" }
    , { DEVICE_NOT_FOUND, "Fail: No haptic device found (is the device dll in the bin folder?)"}
    , { NOT_INITD, "Fail: You must initialize the haptic device" }
    , { ALREADY_INITD, "Fail: Haptic device has already been initialized" }
    , { CANT_BE_INITD, "Fail: You can call this function only when device is not initialzied" }
    , { ALREADY_STOPPED, "Fail: You already stopped this simulation" }
    , { NOT_RUNNING, "Fail: Haptic thread is not running (remember to call start)" }
    , { ALREADY_RUNNING, "Fail: Haptic thread is already running (remember to call stop)" }
    , { THREAD_NOT_RUNNING, "Fail: Thread not running, even if it should be. Retry" }
    , { THREAD_RUNNING, "Fail: Thread running, even if it should be. Retry" }
    , { OBJ_NOT_FOUND, "Fail: Invalid object id" }
    , { FAIL_SET_TEXTURE, "Fail: Could not set texture" }
    , { FAIL_ALLOCATE_TEXTURE, "Fail: Could not allocate texture" }
    , { SAMPLING_TOO_SMALL, "Fail: sampling rate too low (minimum is = 1)" }
    , { INVALID_PARAMS, "Fail: Params provided are invalid" }
    , { INVALID_PARAMS_SUM_NOT_SIX, "Fail: Params provided are invalid (sum must be 6)" }
    , { INVALID_PARAMS_GT_THAN_THREE, "Fail: Params provided are invalid (each val must be less then 3)" }
    , { INVALID_PARAMS_SAME_VAL, "Fail: Params provided are invalid (2 of them having same value)" }
    , { ENABLE_WHEN_PAUSED, "You can enable this feature only when the haptic loop is paused" }
    , { DISABLE_WHEN_PAUSED, "You can disable this feature only when the haptic loop is paused" }
    , { EXPORT_FAILED, "Could not export the object to the specified filename"}
    , { NOT_A_MESH, "The function you are calling takes mesh object, this isn't a mesh" }
    , { NOT_RECORDING, "Fail: You are not recording data, so I cannot stop recording!" }
    , { ALREADY_RECORDING, "Fail: You are already recording, you have to stop it before!" }
    , { NEGATIVE_CYCLES, "Fail: The interpolation cycle period cannot be negative (negative time)" }
    , { OVESHOT_TOO_LOW, "The overshot percentage is too low (either 0.0 or negative)" }
    , { NO_HOOK_EXISTING, "No hooks were set, so no hook have been removed!" }
  };

  std::atomic<int> errorPos {0};

  // return an error message and store it in the errors buffer
  ErrorMsg retErr (ErrorMsg err) {
    // Cycle the error buffer
    if (errorPos >= ERRORS_BUFFER_SIZE_INT - 1) {
      errorPos.store (0);
    } else {
      errorPos.fetch_add (1);
    }

    errors [errorPos.load ()] = err;

    return err;
  }

  extern "C" {
	void get_version_info(int * major,
	      int * minor,
	      int * patch) {
	      *major = version_major;
	      *minor = version_minor;
	      *patch = version_patch;
	}

    int get_error_msg (int err, int buffer_size, char * message) {
      // Check that the error message exists or fail
      if (ErrorMsgStrings.count (err) == 0) {
	return retErr (INVALID_ERROR_NUM);
      }

      std::string msg = ErrorMsgStrings.at (err);
      if (static_cast<int> (msg.length ()) + 1 <= buffer_size) {
	std::copy (msg.begin (), msg.end (), message);
	// don't forget the terminating 0
	message [msg.size ()] = '\0';
	return retErr (SUCCESS);
      }
      return retErr (BUFFER_TOO_SMALL);
    }

    int last_error_msg (int buffer_size, char * message) {
      return get_error_msg (errors [errorPos.load ()],
			    buffer_size, message);
    }
  }
}
