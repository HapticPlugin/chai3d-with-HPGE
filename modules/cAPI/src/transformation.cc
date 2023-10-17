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
#include <atomic>

namespace HPGE {
  // default axis // FIXME: those must be atomic if changed at runtime
  int mirror [3] = { 1, 1, 1 };
  int order [3] = { 0, 1, 2 };
  int mirror_quat [4] = { 1, 1, 1, 1 };
  double scale [3] = { 1.0, 1.0, 1.0 };
  double rotation [4] = { 1.0, 0.0, 0.0, 0.0 };
  double transl [3] = { 0.0, 0.0, 0.0 };
  extern std::atomic<bool> initialized;
  // https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
  void rotateVectorByQuaternion (const chai3d::cVector3d& v,
				    const chai3d::cQuaternion& q,
				    chai3d::cVector3d& vprime) {
    // Extract the vector part of the quaternion
    chai3d::cVector3d u(q.x, q.y, q.z);

    // Extract the scalar part of the quaternion
    double s = q.w;

    chai3d::cVector3d ucp = u;
    ucp.cross(v);

    // Do the math
    vprime = 2.0 * u.dot(v) * u
      + (s*s - u.dot(u)) * v
      + 2.0 * s * ucp;
  }
  void CreateQuatMirror (bool rightHanded) {
    mirror_quat [0] = 1;

    if (rightHanded) {
      mirror_quat [1] = mirror [0];
      mirror_quat [2] = mirror [1];
      mirror_quat [3] = mirror [2];
    } else { // mind the minus!
      mirror_quat [1] = - mirror [0];
      mirror_quat [2] = - mirror [1];
      mirror_quat [3] = - mirror [2];
    }
  }

  void PosFromChai (const chai3d::cVector3d chaiPosition,
		    double externalPosition [3]) {
    auto mirrored =
      chai3d::cVector3d (chaiPosition.get(order [0]) * mirror [0],
			 chaiPosition.get(order [1]) * mirror [1],
			 chaiPosition.get(order [2]) * mirror [2]);

    auto quat = chai3d::cQuaternion(rotation [0],
				    rotation [1],
				    rotation [2],
				    rotation [3]);

    // quat.invert (); // <- this changes the 90 degree test

    chai3d::cVector3d rotated;
    rotateVectorByQuaternion (mirrored, quat, rotated);

    externalPosition [0] = rotated.x () / scale [0];
    externalPosition [1] = rotated.y () / scale [1];
    externalPosition [2] = rotated.z () / scale [2];
  }

  chai3d::cVector3d PosToChai (const double inputPosition [3]) {
    // 2. Rotate
    chai3d::cVector3d rotated;
    auto tmpc = chai3d::cVector3d (inputPosition [0],
				   inputPosition [1],
				   inputPosition [2]);

    auto quat = chai3d::cQuaternion (rotation [0],
				     rotation [1],
				     rotation [2],
				     rotation [3]);

    quat.invert (); // <- this changes the 90 degree test
    rotateVectorByQuaternion(tmpc, quat, rotated);

    // 1. Scale + 2. Rotation + Mirror
    double tmppos [3];

    tmppos [order [0]] =
      (rotated.x () * scale [0]) * mirror [0];
    tmppos [order [1]] =
      (rotated.y () * scale [1]) * mirror [1];
    tmppos [order [2]] =
      (rotated.z () * scale [2]) * mirror [2];

    // 3. Translate
    return chai3d::cVector3d (tmppos [0],
			      tmppos [1],
			      tmppos [2]);
  }

  chai3d::cVector3d ScaleToChai (const double inputScale [3]) {
    double tmposcale [3];

    tmposcale [order [0]] = inputScale [0];// * scale [0];
    tmposcale [order [1]] = inputScale [1];// * scale [1];
    tmposcale [order [2]] = inputScale [2];// * scale [2];

    return chai3d::cVector3d (tmposcale [0], tmposcale [1], tmposcale [2]);
  }

  void RotFromChai (const chai3d::cQuaternion inputRotation,
		    double outputRotation [4]) {
    chai3d::cQuaternion rotChange = chai3d::cQuaternion (rotation [0],
							 rotation [1],
							 rotation [2],
							 rotation [3]);
    // chai3d::cQuaternion mirrorq = chai3d::cQuaternion (mirror_quat [0],
    //						       mirror_quat [1],
    //						       mirror_quat [2],
    //						       mirror_quat [3]);

    chai3d::cQuaternion rotcopy = inputRotation;

    // rotcopy.invert ();
    // rotChange.invert ();
    rotcopy.dot (rotChange);
    // mirrorq.invert ();
    // rotcopy.dot(mirrorq);

    // w quaternion component is not flipped
    outputRotation [0] = rotcopy.w;
    // + 1 is the offset between rotation and xyz vectors sizes
    outputRotation [1] = get (rotcopy, order [0] + 1) * mirror_quat [1];
    outputRotation [2] = get (rotcopy, order [1] + 1) * mirror_quat [2];
    outputRotation [3] = get (rotcopy, order [2] + 1) * mirror_quat [3];
  }

  void RotToChai (const double inputRotation [4], chai3d::cQuaternion* out) {
    double outputRot [4];

    outputRot [0] = mirror_quat [0] * inputRotation [0];

    outputRot [order [0] + 1] = mirror_quat [1] * inputRotation [1];
    outputRot [order [1] + 1] = mirror_quat [2] * inputRotation [2];
    outputRot [order [2] + 1] = mirror_quat [3] * inputRotation [3];

    auto tmp = chai3d::cQuaternion (outputRot [0],
				    outputRot [1],
				    outputRot [2],
				    outputRot [3]);

     auto rot = chai3d::cQuaternion (rotation [0],
    				    rotation [1],
    				    rotation [2],
    				    rotation [3]);

     rot.invert ();

     tmp.dot (rot);

    out -> w = tmp.w;
    out -> x = tmp.x;
    out -> y = tmp.y;
    out -> z = tmp.z;
  }

  extern "C" {
    // The values must be between 1 and 3 (1-based indexing)
    int set_world_mirror (const int x, const int y, const int z) {
      if (initialized.load ()) { return retErr (CANT_BE_INITD); }
      // check validity
      // x = 1, y = 2, z = 3
      // 1x1, 1x2, 1x3, or 6,0,0...
      bool valid = abs (x) + abs (y) + abs (z) == 6;
      if (!valid) { return retErr (INVALID_PARAMS_SUM_NOT_SIX); }

      // keeps 1,2,3, or 2,2,2
      valid = valid && abs (x) <= 3 && abs (y) <= 3 && abs (z) <= 3;
      if (!valid) { return retErr (INVALID_PARAMS_GT_THAN_THREE); }
      // keeps 1,2,3 only
      valid = valid &&
	abs (x) != abs (y) &&
	abs (x) != abs (z) &&
	abs (y) != abs (z);

      if (!valid) { return retErr (INVALID_PARAMS_SAME_VAL); }

      order [0] = abs (x) - 1;
      order [1] = abs (y) - 1;
      order [2] = abs (z) - 1;

      mirror [0] = sig (x);
      mirror [1] = sig (y);
      mirror [2] = sig (z);


      // check handedness by computing cross product between two first
      // vectors of the matrix and checking if it corresponds to third
      // vector
      int m [3] [3] = { {0} };

      m [order [0]] [0] = mirror [0];
      m [order [1]] [1] = mirror [1];
      m [order [2]] [2] = mirror [2];

      int v [3] = { 0 };

      v [0] = m [1] [0] * m [2] [1] - m [2] [0] * m [1] [1];
      v [1] = m [2] [0] * m [0] [1] - m [0] [0] * m [2] [1];
      v [2] = m [0] [0] * m [1] [1] - m [1] [0] * m [0] [1];

      bool rightHandedWorld = (v [0] == m [0] [2] &&
			       v [1] == m [1] [2] &&
			       v [2] == m [2] [2]);

      CreateQuatMirror (rightHandedWorld);

      return retErr (SUCCESS);
    }

    void set_world_scale (const double x, const double y, const double z) {
      scale [0] = x;
      scale [1] = y;
      scale [2] = z;
    }

    void set_world_translation (const double x, const double y, const double z) {
      transl [0] = x;
      transl [1] = y;
      transl [2] = z;
    }

    void set_world_rotation_eulerXYZ (const double x,
				     const double y,
				     const double z) {
      chai3d::cMatrix3d m;
      chai3d::cQuaternion q;
      m.setExtrinsicEulerRotationDeg(x, y, z, chai3d::C_EULER_ORDER_XYZ);
      q.fromRotMat(m);
      rotation [0] = q.w;
      rotation [1] = q.x;
      rotation [2] = q.y;
      rotation [3] = q.z;
    }

    void get_world_mirror (double* out) {
      out [0] = mirror [0];
      out [1] = mirror [1];
      out [2] = mirror [2];
    }

    void get_world_order (double* out) {
      out [0] = order [0];
      out [1] = order [1];
      out [2] = order [2];
    }

    void get_world_rotation (double* out) {
      out[0] = rotation[0];
      out[1] = rotation[1];
      out[2] = rotation[2];
      out[3] = rotation[3];
    }
  }
}
