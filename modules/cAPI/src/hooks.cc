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
  ForceHookPtr force_hook_fn {nullptr};
  std::atomic<bool> hook_use_proxy {false};
  std::mutex hook_mutex;

  namespace debug {
    bool run_hook
    (const double position [3],
     const double velocity [3],
     double force [3]) {
      if (force_hook_fn != nullptr) {
	(force_hook_fn)(position, velocity, force);
	return true;
      }
      return false;
    }
  }

  double get (chai3d::cQuaternion q, int el) {
    switch (el) {
    case 1:
      return q.x;
    case 2:
      return q.y;
    case 3:
      return q.z;
      // Access more frequently to it with .w, so can be placed after
      // others to be faster
    case 0:
      return q.w;
    default:
      return 0.0;
    }
  }

  extern "C" {
    void set_hook (const int proxy, // 1: use position, 0: use proxy
		   void (*f)
		   (const double position [3],
		    const double velocity [3],
		    double force [3])) {
      hook_use_proxy.store (proxy == 0);
      hook_mutex.lock ();
      force_hook_fn = f;
      hook_mutex.unlock();
    }

    int remove_hook (void) {
      if (force_hook_fn != nullptr) {
	hook_mutex.lock ();
	force_hook_fn = nullptr;
	hook_mutex.unlock ();

	return retErr (SUCCESS);
      }
      return retErr (NO_HOOK_EXISTING);
    }
  }
}
