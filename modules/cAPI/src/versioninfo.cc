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

#include "versioninfo.hh"

// Change history:
// 0.4.2:   disable Magnetic field on Meshes
// 0.4.2.1: fix initialization not working sometimes
// 0.4.3:   fix off by one error in errorMsgStrings
// 0.4.4:   rise forces
// 1.0.0:   first released version
// 1.0.1:   fix segfault on less UVs than Vertexes (create_mesh)
// 1.0.2:   small fixes, csv output
// 2.0.0:   Change init_logging() parameters: allow save on device coordinates
// 2.0.1:   Fixes
// 2.1.0:   add set euler rotation
// 2.2.0:   add get_tool_force
// 2.2.1:   build fix
// 2.2.2:   fix wrong position/rotation update
// 2.3.0:   add log_annotate and get_log_frame
// 2.3.1:   fix log format
// 2.3.2:   fix log-related (annotate) crash
// 2.3.3:   fix hooks-related bugs
// 2.3.4:   delete reference to hook on stop
// 2.3.5:   fix logging coodinate system on log_annotate
// 2.3.6:   check state consistency through asserts, close devices
