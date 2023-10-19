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

#include <mutex>
#include <atomic>

namespace HPGE {
  extern std::mutex world_mutex;        // world.cc
  extern chai3d::cToolCursor * tool;    // thread.cc
  extern std::atomic<bool> initialized; // thread.cc

  // enable if objects in the scene are going to rotate of translate
  // or possibly collide against the tool. If the environment
  // is entirely static, you can set this parameter to "false"
  int setDynamicObjects (bool status) {
    if (!initialized.load ()) { return retErr (NOT_INITD); }

    world_mutex.lock ();
    tool -> enableDynamicObjects (status);
    world_mutex.unlock ();

    return retErr (SUCCESS);
  }

  extern "C" {
    int disable_dynamic_objects (void) { return setDynamicObjects (false); }
    int enable_dynamic_objects (void) { return setDynamicObjects (true); }
  }
}
