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

#pragma once
namespace HPGE {
  // `RECORDING_DATA_BUFFER_SIZE` just be "big-enough" to prevent data
  // overrides, but garbage collection will happen only after the
  // array is full Setting it to a value that is too low will lead to
  // memory corruption. Setting it to a value too high will lead to
  // high memory usage
  const int RECORDING_DATA_BUFFER_SIZE = 2;

  extern "C" {
    typedef enum
      {
       CGenericObjectType,
       CMeshType,
       CMultiMeshType,
       CMultiPointType,
       CMultiSegmentType,
       CShapeBoxType,
       CShapeCylinderType,
       CShapeEllipsoidType,
       CShapeLineType,
       CShapeSphereType,
       CShapeTorusType,
       CVoxelObjectType,
       CWorldType
      } ObjectTypes;

    struct interpolationStr {
      int cycles;
      int current_cycle;
      double overshot;
      // Position
      bool position_enabled;
      bool reached_position;
      chai3d::cVector3d source_position;
      chai3d::cVector3d dest_position;
      // Rotation
      bool rotation_enabled;
      bool reached_rotation;
      chai3d::cQuaternion source_rotation;
      chai3d::cQuaternion dest_rotation;
    };

    struct objectStr {
      chai3d::cGenericObject* obj;
      ObjectTypes type;
      std::string tag;
      interpolationStr interpolate;
    };

    struct Logging {
      bool device_coordinates;
      int sampling_rate;
      bool position;           // global position
      bool velocity;           // global velocity
      bool force;              // global force
      bool interaction_forces; // contact force with objects defined in
      // `object_forces`
      std::vector<int> object_forces;
    };

    struct InteractionForces {
      objectStr obj;
      double force [3];
    };

    struct SavedFrame {
      unsigned long long timestamp;
      unsigned int ticks;
      double position [3];
      double velocity [3];
      double force [3];
      InteractionForces * interactions;
      std::string notes;
    };

    struct SavedData {
      // FIXME: This cannot be used in C#...
      std::vector<SavedFrame> frames;
    };
  }
}
