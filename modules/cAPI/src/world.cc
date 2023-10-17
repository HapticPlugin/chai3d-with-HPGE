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
#include <mutex>

namespace HPGE {
  extern std::atomic<bool> initialized; // thread.cc
  extern std::atomic<bool> running;
  std::atomic<int> id {1};

  // a world that contains all objects of the virtual environment
  std::mutex world_mutex;
  chai3d::cWorld * world {nullptr};

  // They could be accessed by `hapticDeviceInfo` and tool but having
  // them we can avoid locking them (they are set on initialize only)
  double workspaceScaleFactor;
  double maxStiffness;
  double maxDamping;
  double maxForce;

  // define the radius of the tool (sphere)
  double toolRadius = 0.0;

  // Store all created objects indexed by a UUID
  std::map<int,HPGE::objectStr> objects;

  extern double rotation [4]; // transformation.cc

  extern std::mutex objects_mutex;
  extern std::mutex chai3d_mutex;

  const int DEFAULT_INTERPOLATION_MS {20};

  // Add the object to the map of existing objects, returning
  // its object id Please DO NOT assume the id are sequential.
  // They might be random as well
  int addObjectToMap (HPGE::objectStr Obj) {
    int idx = id.fetch_add (1);
    objects_mutex.lock ();
    objects [idx] = Obj;
    objects_mutex.unlock ();
    return idx;
  }

  HPGE::objectStr* getObjectFromMap (int objectId) {
    objects_mutex.lock ();
    // FIXME: replace find() with count(), 0
    bool found = objects.find (objectId) != objects.end ();
    objects_mutex.unlock ();
    if (found) {
      // this (in theory) violates the lock
      return &objects.at(objectId);
    }

    return nullptr;
  }

  namespace debug {
    bool isInterpolationEnabled (int objectId, bool position) {
      auto obj = getObjectFromMap (objectId);
      if (obj == nullptr) { return false; }

      if (position) {
	return obj -> interpolate.position_enabled;
      } else {
	return obj -> interpolate.rotation_enabled;
      }
    }

    bool isPositionInterpolationEnabled (const int objectId) {
      return isInterpolationEnabled (objectId, true);
    }
    bool isRotationInterpolationEnabled (const int objectId) {
      return isInterpolationEnabled (objectId, false);
    }

    std::string getObjectTag (const int objectId) {
      auto obj = getObjectFromMap (objectId);
      if (obj == nullptr) { return ""; }

      return obj -> tag;
    }

    int getCreatedObjectsNumber (void) {
      return objects.size ();
    }
  }

  ErrorMsg interpolationControl (const int objectId,
				 const bool position,
				 const bool enable) {
    auto obj = getObjectFromMap (objectId);
    if (obj == nullptr) { return retErr (OBJ_NOT_FOUND); }

    if (obj -> interpolate.cycles <= 0) {
      obj -> interpolate.cycles = DEFAULT_INTERPOLATION_MS;
    }

    if (position) {
      obj -> interpolate.current_cycle = 0;
      obj -> interpolate.position_enabled = enable;
    } else {
      obj -> interpolate.rotation_enabled = enable;
    }
    return retErr (SUCCESS);
  }

  int ObjectEnable (int objectId, bool enable) {
    auto obj = getObjectFromMap (objectId);
    if (obj == nullptr) { return retErr (OBJ_NOT_FOUND); }

    obj -> obj -> setEnabled (enable);
    return retErr (SUCCESS);
  }

  extern "C" {
    // By default, this does not add the object to the world
    int create_mesh_object (const double objectPos [],
			    const double objectScale [],
			    const double objectRotation [],
			    const double vertPos [] [3],
			    const double normals [] [3],
			    const int vertNum,
			    const int triPos [] [3], const int triNum,
			    const int uvNum, const double uvs [] [2]) {
      if (!initialized.load ()) { return -1; } // FIXME: add error codes?
      chai3d::cMesh * object = new chai3d::cMesh ();

      for (int i = 0; i < vertNum; ++i) {
	int vertex = object -> newVertex ();

	chai3d::cVector3d vertPosVector3 = PosToChai (vertPos [i]);
	chai3d::cVector3d vertNormalVecotor3 =
	  chai3d::cVector3d (normals [i][0],
			     normals [i][1],
			     normals [i][2]);

	object -> m_vertices -> setLocalPos (vertex, vertPosVector3);
	object -> m_vertices -> setNormal (vertex, vertNormalVecotor3);

	// Texture
	if(i < uvNum) {
	  object -> m_vertices -> setTexCoord (vertex, uvs [i] [0], uvs [i] [1]);
	}
      }



      // set triangles
      for (int i = 0; i < triNum; ++i) {
	object -> newTriangle (triPos [i] [2], triPos [i] [1], triPos [i] [0]);
      }
      auto objPos = PosToChai (objectPos);
      object -> setLocalPos (objPos);
      // scale object
      auto convertedScale = ScaleToChai (objectScale);
      object -> scaleXYZ (convertedScale.get (0),
			  convertedScale.get (1),
			  convertedScale.get (2));
      // rotate object
      chai3d::cMatrix3d rotmat;
      chai3d::cQuaternion qrot;
      RotToChai (objectRotation, &qrot);
      qrot.toRotMat (rotmat);

      object -> setLocalRot (rotmat);
      // compute a boundary box
      object -> computeBoundaryBox (true);

      // compute collision detection algorithm
      const int brute = 1; // FIXME: add as a parameter
      if (brute == 0) {
	object -> createBruteForceCollisionDetector ();
      } else {
	object -> createAABBCollisionDetector (toolRadius);
      }

      //object -> computeAllNormals ();

      objectStr obj { object, CMeshType, "unnamed",
		      { DEFAULT_INTERPOLATION_MS, 0, 1.0,
			// POSITION
			false, true, // enable, reached
			objPos, objPos, // source, dest
			// ROTATION
			false, true, // enable, reached
			// source, dest
			qrot, qrot
		      }
      };

      return addObjectToMap (obj);
    }

    int create_box_object (const double scale [3],
			   const double position [3],
			   const double rotation [4]) {
      auto objScale = ScaleToChai (scale);
      chai3d::cShapeBox * object = new chai3d::cShapeBox (objScale.x (),
							  objScale.y (),
							  objScale.z ());

      auto objPos = PosToChai (position);
      object -> setLocalPos (objPos);

      // rotate object
      chai3d::cMatrix3d rotmat;
      chai3d::cQuaternion qrot;
      RotToChai (rotation, &qrot);
      qrot.toRotMat (rotmat);

      object -> setLocalRot (rotmat);

      // compute a boundary box
      object -> computeBoundaryBox (true);

      objectStr obj
	{ (chai3d::cGenericObject*)object, CShapeBoxType, "unnamedbox",
	  { DEFAULT_INTERPOLATION_MS, 0, 1.0,
	    // POSITION
	    false, true, // enable, reached
	    objPos, objPos, // source, dest
	    // ROTATION
	    false, true, // enable, reached
	    // source, dest
	    qrot, qrot
	  }
	};

      return addObjectToMap (obj);
    }

    // By default, this does not add the object to the world
    int create_sphere_object (const double radius,
			   const double position [3],
			   const double rotation [4]) {
      chai3d::cShapeSphere * object = new chai3d::cShapeSphere(radius);

      auto objPos = PosToChai (position);
      object -> setLocalPos (objPos);

      // rotate object
      chai3d::cMatrix3d rotmat;
      chai3d::cQuaternion qrot;
      RotToChai (rotation, &qrot);
      qrot.toRotMat (rotmat);

      object -> setLocalRot (rotmat);

      // compute a boundary box
      object -> computeBoundaryBox (true);

      objectStr obj
	{ (chai3d::cGenericObject*)object, CShapeSphereType, "unnamedsphere",
	  { DEFAULT_INTERPOLATION_MS, 0, 1.0,
	    // POSITION
	    false, true, // enable, reached
	    objPos, objPos, // source, dest
	    // ROTATION
	    false, true, // enable, reached
	    // source, dest
	    qrot, qrot
	  }
	};

      return addObjectToMap (obj);
    }

    int object_exists (const int objectId) {
      auto obj = getObjectFromMap (objectId);
      if (obj == nullptr) { return retErr (OBJ_NOT_FOUND); }
      return retErr (SUCCESS);
    }

    int add_object_to_world (int objectId) {
      if (!initialized.load ()) { return retErr (NOT_INITD); }
      auto obj = getObjectFromMap (objectId);
      if (obj == nullptr) { return retErr (OBJ_NOT_FOUND); }

      // add object to world
      world_mutex.lock ();
      world -> addChild (obj -> obj);
      world_mutex.unlock ();

      return retErr (SUCCESS);
    }

    int get_object_position (const int objectId,
			     double objectPosition [3]) {
      auto obj = getObjectFromMap (objectId);
      if (obj == nullptr) { return retErr (OBJ_NOT_FOUND); }

      PosFromChai (obj -> obj -> getLocalPos (), objectPosition);

      return retErr (SUCCESS);
    }

    int set_object_position (const int objectId,
			     const double objectPosition [3]) {
      auto obj = getObjectFromMap (objectId);
      if (obj == nullptr) { return retErr (OBJ_NOT_FOUND); }

      chai3d::cVector3d dest = PosToChai (objectPosition);

      // objects_mutex.lock ();
      if (obj -> interpolate.position_enabled) {
	// Start interpolating
	obj -> interpolate.dest_position = dest;
	obj -> interpolate.reached_position = false;
	// RETURN TO PREVENT SETTING THE POSITION
	// objects_mutex.unlock ();
	return retErr (SUCCESS);
      }

	// Abusing the interpolation system to set
	// the position in the main loop only
	obj -> interpolate.dest_position = dest;
	obj -> interpolate.position_enabled = true;
      obj -> interpolate.reached_position = false;
	obj -> interpolate.cycles = 1;
	obj -> interpolate.current_cycle = 0;

      return retErr (SUCCESS);
    }

    int set_object_rotation (const int objectId,
			     const double objectRotation [4]) {
      auto obj = getObjectFromMap (objectId);
      if (obj == nullptr) { return retErr (OBJ_NOT_FOUND); }

      chai3d::cQuaternion dest;
      RotToChai (objectRotation, &dest);

      auto rotChange = chai3d::cQuaternion (rotation [0],
					    rotation [1],
					    rotation [2],
					    rotation [3]);
      rotChange.invert ();
      dest.dot (rotChange);

      if (obj -> interpolate.rotation_enabled) {
	// Start interpolating
	obj -> interpolate.dest_rotation = dest;
	obj -> interpolate.reached_rotation = false;
	// RETURN TO PREVENT SETTING THE POSITION
	return retErr (SUCCESS);
      }

      // Abusing the interpolation system to set
      // the position in the main loop only
      obj -> interpolate.dest_rotation = dest;
      obj -> interpolate.rotation_enabled = true;
      obj -> interpolate.reached_rotation = false;
      obj -> interpolate.cycles = 1;
      obj -> interpolate.current_cycle = 0;

      return retErr (SUCCESS);
    }

	int set_object_rotation_euler (const int objectId,
	    const double objectRotation [3]) {
	    auto obj = getObjectFromMap (objectId);
	    if (obj == nullptr) { return retErr (OBJ_NOT_FOUND); }

	    double tmp [3];
	    tmp[0] = + objectRotation[2];
	    tmp[1] = - objectRotation[0];
	    tmp[2] = - objectRotation[1];

	    // FIXME: use order and mirror
	    // mirror: 1, 1, -1
	    // order:  1, 2, 0

	    chai3d::cMatrix3d rot = chai3d::cRotEulerDeg(tmp [0], tmp [1], tmp [2],
							chai3d::C_EULER_ORDER_XYZ);
	    chai3d::cQuaternion dest;

	    dest.fromRotMat(rot);

	    if (obj -> interpolate.rotation_enabled) {
		// Start interpolating
		obj -> interpolate.dest_rotation = dest;
		obj -> interpolate.reached_rotation = false;
		// RETURN TO PREVENT SETTING THE POSITION
		return retErr (SUCCESS);
	    }

	    // Abusing the interpolation system to set
	    // the position in the main loop only
	    obj -> interpolate.dest_rotation = dest;
	    obj -> interpolate.rotation_enabled = true;
	    obj -> interpolate.reached_rotation = false;
	    obj -> interpolate.cycles = 1;
	    obj -> interpolate.current_cycle = 0;

	    return retErr (SUCCESS);
	}


    // README: if the object was NOT a mesh, only the first value of
    // objectScale is used others are discarded because it's not
    // possible to scale with XYZ
    int set_object_scale (const int objectId, const double objectScale [3]) {
      auto obj = getObjectFromMap (objectId);
      if (obj == nullptr) { return retErr (OBJ_NOT_FOUND); }

      auto converted = ScaleToChai (objectScale);
      if (isMesh (obj -> type)) {
	static_cast<chai3d::cMesh*>
	  (obj -> obj) -> scaleXYZ (converted.get (0),
				    converted.get (1),
				    converted.get (2));
      } else {
	static_cast<chai3d::cMesh*>
	  (obj -> obj) -> scale (converted.get (0));
      }

      return retErr (SUCCESS);
    }

    int get_object_rotation (const int objectId,
			     double objectRotation [4]) {
      auto obj = getObjectFromMap (objectId);
      if (obj == nullptr) { return retErr (OBJ_NOT_FOUND); }

      // FIXME: local or global?

      // objects_mutex.lock ();
      // chai3d_mutex.lock ();
      auto rotMat = obj -> obj -> getLocalRot ();
      // objects_mutex.unlock ();
      // chai3d_mutex.unlock ();

      chai3d::cQuaternion newrot;
      newrot.fromRotMat (rotMat);
      RotFromChai(newrot, objectRotation);

      return retErr (SUCCESS);
    }

    int set_object_tag (const int objectId,
			const char * tag) {
      auto obj = getObjectFromMap (objectId);
      if (obj == nullptr) { return retErr (OBJ_NOT_FOUND); }

      obj -> tag = std::string (tag);

      return retErr (SUCCESS);
    }

    int set_object_material (const int objectId,
			     const double stiffness,
			     const int surface, // 0 = don't use
			     const double staticfriction,
			     const double dynamicfriction,
			     const double magnetic_max_force,
			     const double magnetic_max_distance,
			     const double viscosity,
			     const double level,
			     const double stickslip_stiffness,
			     const double stickslip_maxforce,
			     const double vibration_freq,
			     const double vibration_amplitude) {
      auto obj = getObjectFromMap (objectId);
      if (obj == nullptr) { return retErr (OBJ_NOT_FOUND); }

      world_mutex.lock ();
      obj -> obj -> deleteAllEffects (); // DONT USE IT (jump in rendering)

      if (surface != 0) {
	obj -> obj -> createEffectSurface ();
      } else {
	obj -> obj -> deleteEffectSurface ();
      }

      obj -> obj -> m_material -> setStiffness
	(stiffness * maxStiffness);
      obj -> obj -> setStiffness
	(stiffness * maxStiffness);
      obj -> obj -> m_material -> setStaticFriction (staticfriction);
      obj -> obj -> m_material -> setDynamicFriction (dynamicfriction);
      obj -> obj -> setFriction
	(staticfriction, dynamicfriction);
      // SetUseHapticTexture must be true, but we'll enable it when
      // adding the texture only
      obj -> obj -> m_material -> setTextureLevel (level);
      // Use Haptic Texture will be enabled when setting the texture
      obj -> obj -> m_material -> setUseHapticTexture (false);

      if (viscosity > 0.0) {
	obj -> obj -> createEffectViscosity ();
	obj -> obj -> m_material -> setViscosity
	  (viscosity * maxDamping);
      } else {
	obj -> obj -> deleteEffectViscosity ();
      }

      // Mesh don't support magnetic fields
      if ((obj -> type != CMeshType) &&
	  (magnetic_max_force > 0.0 &&
	   magnetic_max_distance > 0.0)) {
	obj -> obj -> createEffectMagnetic ();
	obj -> obj -> m_material -> setMagnetMaxForce
	  (magnetic_max_force * maxForce);
	obj -> obj -> m_material -> setMagnetMaxDistance
	  (magnetic_max_distance);
      } else {
	obj -> obj -> deleteEffectMagnetic ();
      }

      if (stickslip_stiffness > 0.0) {
	obj -> obj -> createEffectStickSlip ();
	obj -> obj -> m_material -> setStickSlipStiffness
	  (stickslip_stiffness * maxStiffness);
	obj -> obj -> m_material -> setStickSlipForceMax
	  (stickslip_maxforce * maxForce);
      } else {
	obj -> obj -> deleteEffectStickSlip ();
      }

      if (vibration_freq > 0.0 && vibration_amplitude > 0.0) {
	obj -> obj -> createEffectVibration ();
	obj -> obj -> m_material -> setVibrationFrequency (vibration_freq);
	obj -> obj -> m_material -> setVibrationAmplitude (vibration_amplitude);
      } else {
	obj -> obj -> deleteEffectVibration ();
      }
      world_mutex.unlock ();

      return retErr (SUCCESS);
    }

    int set_object_texture (const int objectId,
			    const unsigned int size_x, const unsigned int size_y,
			    const float pixels [], const int spherical) {
      if (!initialized) { return retErr (NOT_INITD); }

      auto obj = getObjectFromMap (objectId);
      if (obj == nullptr) { return retErr (OBJ_NOT_FOUND); }

      chai3d::cImagePtr texture_img (new chai3d::cImage ());
      if (!texture_img -> allocate (size_x, size_y)) {
	return retErr (FAIL_ALLOCATE_TEXTURE);
      }
      // Add the pixels
      for (unsigned int y = 0; y < size_y; ++y) {
	for (unsigned int x = 0; x < size_x; ++x) {
	  texture_img -> setPixelColor (x, y,
					floatToByte (pixels [y * size_x + x]));
	}
      }

      world_mutex.lock ();
      obj -> obj -> m_texture = chai3d::cTexture2d::create ();
      if (!obj -> obj -> m_texture -> setImage (texture_img)) {
	world_mutex.unlock ();
	return retErr (FAIL_SET_TEXTURE);
      }

      obj -> obj -> m_texture -> setSphericalMappingEnabled (spherical == 1);

      obj -> obj -> setUseTexture (true, true);
      obj -> obj -> m_material -> setWhite ();

      // create normal map from texture data
      chai3d::cNormalMapPtr normalMap = chai3d::cNormalMap::create ();
      normalMap -> createMap (obj -> obj -> m_texture);
      obj -> obj -> m_normalMap = normalMap;
      obj -> obj -> m_normalMap -> flip (false, true);

      // set haptic properties
      obj -> obj -> m_material -> setUseHapticTexture (true);
      obj -> obj -> m_material -> setHapticTriangleSides (true, false);

      // enable texture rendering. Disable with level = 0 in haptic properties
      obj -> obj -> m_material -> setUseHapticTexture (true);
      world_mutex.unlock ();

      return retErr (SUCCESS);
    }

    int enable_position_interpolation (const int objectId) {
      return retErr (interpolationControl (objectId
					   , true // position
					   , true // enable
					   ));
    }

    int disable_position_interpolation (const int objectId) {
      return retErr (interpolationControl (objectId
					   , true // position
					   , false // enable
					   ));
    }

    int enable_rotation_interpolation (const int objectId) {
      return retErr (interpolationControl (objectId
					   , false // position
					   , true // enable
					   ));
    }

    int disable_rotation_interpolation (const int objectId) {
      return retErr (interpolationControl (objectId
					   , false// position
					   , false // enable
					   ));
    }

    int set_interpolation_period (const int objectId,
				  const int cycles,
				  const double overshot) {
      auto obj = getObjectFromMap (objectId);
      if (obj == nullptr) { return retErr (OBJ_NOT_FOUND); }

      if (cycles < 0) {
	return retErr (NEGATIVE_CYCLES);
      } else if (cycles == 0) {
	obj -> interpolate.position_enabled = false;
	obj -> interpolate.rotation_enabled = false;
      }
      if (overshot <= 0.0) {
	return retErr (OVESHOT_TOO_LOW);
      }

      obj -> interpolate.current_cycle = 0;
      obj -> interpolate.overshot = overshot;
      obj -> interpolate.cycles = cycles;
      return retErr (SUCCESS);
    }

    int enable_object (const int objectId) {
      return ObjectEnable (objectId, true);
    }
    int disable_object (const int objectId) {
      return ObjectEnable (objectId, false);
    }
  }
}
