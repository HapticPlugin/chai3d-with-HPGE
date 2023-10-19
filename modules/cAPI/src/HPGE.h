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
#pragma warning(disable:4996)
#include <array>
#include <map>
#include "chai3d.h"
#include "structs.h"

#if (defined(linux) || defined(__linux) || defined(__linux__))
#define FUNCDLL_API
#else
#define FUNCDLL_API __declspec(dllexport)
#endif

#define ERRORS_BUFFER_SIZE_INT 10

//https://stackoverflow.com/questions/17323084/c-pointer-to-function-assignment
typedef void (*ForceHookPtr)(const double [3], // input position
			     const double [3], // input velocity
			     double [3]);      // output force

namespace HPGE {
  // Simplify debugging by adding mock functions
  namespace debug {
    bool run_hook (const double position [3],
		   const double velocity [3],
		   double force [3]);
    // world.cc
    bool isInterpolationEnabled (const int objectId, const bool position);
    bool isPositionInterpolationEnabled (const int objectId);
    bool isRotationInterpolationEnabled (const int objectId);
    FUNCDLL_API std::string getObjectTag (const int objectId);
    int getCreatedObjectsNumber (void);
  }
  bool interpolationControl (const int objectId);
  std::ostream& operator<<(std::ostream& os, const SavedFrame& f);
  std::ostream& operator<<(std::ostream& os, const SavedData& f);

  void updateDeviceInformation (void);

  extern const std::map<int, std::string> ErrorMsgStrings;

  inline
  signed char floatToByte (float f) {
    signed char c = (signed char) std::max (0,
			      std::min (255,
					static_cast<int>(floor (f * 256.0)))
			      );
    return c - 128;
  }

  typedef enum
    {
     INVALID_ERROR_NUM = -4,
     NOT_IMPLEMENTED = -3,
     IS_RELEASE = -2,
     GENERIC_FAIL = -1,   // error codes < 0 are library errors
     SUCCESS = 0,         // To check for success use res == 0;
			  // everything else is an error
     BUFFER_TOO_SMALL,    // error codes > 0 are user errors
     DEVICE_NOT_FOUND,
     NOT_INITD,           // run init before
     ALREADY_INITD,       // run stop before
     NOT_RUNNING,         // run start
     ALREADY_RUNNING,     // run stop
     CANT_BE_INITD,
     THREAD_RUNNING,      // even if it shouldn't
     THREAD_NOT_RUNNING,  // even if it shouldn't
     ALREADY_STOPPED,
     OBJ_NOT_FOUND,
     FAIL_SET_TEXTURE,
     FAIL_ALLOCATE_TEXTURE,
     SAMPLING_TOO_SMALL,
     INVALID_PARAMS,      // Unknown invalid_params error
     INVALID_PARAMS_SUM_NOT_SIX,
     INVALID_PARAMS_GT_THAN_THREE,
     INVALID_PARAMS_SAME_VAL,
     ENABLE_WHEN_PAUSED,
     DISABLE_WHEN_PAUSED,
     EXPORT_FAILED,
     NOT_A_MESH,
     NOT_RECORDING,
     ALREADY_RECORDING,     // run stop
     NEGATIVE_CYCLES,
     OVESHOT_TOO_LOW,
     NO_HOOK_EXISTING
    } ErrorMsg;

  inline bool const isMesh(ObjectTypes x) { return x == CMeshType; }

  void hapticLoop (void);
  int addObjectToMap (objectStr Obj);
  HPGE::objectStr* getObjectFromMap (int objectId);
  ErrorMsg retErr (ErrorMsg err);
  void checkHandlerOrInitialize (void);
  void resetClock (void);

  unsigned long long Now (void); // logging.cc
  bool updateLerp (void);

  int setDynamicObjects (bool status);
  void waitForSmallForces (bool enable);
  void ObjectEnable (bool enable);

  extern "C" {
    FUNCDLL_API void get_version_info (int * major,
				       int * minor,
				       int * patch);

    FUNCDLL_API int get_error_msg (const int err,
				   const int buffer_size,
				   char * message);
    FUNCDLL_API int last_error_msg (const int buffer_size, char * message);
    // This must be called before 'start_recording_stats' else nothing
    // will be saved. 0 means "enable", everything else is OFF
    // device_coordinates: if 0 (true) saves position, velocity and
    // force in in the same coordinates and units as the real device.
    // If false, transform the values before saving (same units and
    // coordinates as the caller)
    FUNCDLL_API int init_logging (const int device_coordinates,
					       const int position,
					       const int velocity,
					       const int force,
					       const int interaction_forces,
					       const int object_number,
					       const int * objects);
    FUNCDLL_API int start_logging (const int sampling_rate);

    FUNCDLL_API int stop_logging_and_save (const char * filename);
    FUNCDLL_API int stop_logging_and_get (SavedData * data); // data get written
    FUNCDLL_API int is_logging (void); // data get written

    FUNCDLL_API void tick (void);
    FUNCDLL_API void log_annotate (char * /*string*/);

    FUNCDLL_API int count_devices (void);
    // Scan (count) devices before this
    FUNCDLL_API int get_device_name (const int device,
				     int buffer_size, char * name);

    // Haptic device and loops
    FUNCDLL_API int initialize (const int deviceId,
				const double hapticScale,
				const double radius);
    FUNCDLL_API int deinitialize (void);
    FUNCDLL_API int start (void);
    FUNCDLL_API int stop (void);
    // Do we need two different calls? do we need get_loops?
    FUNCDLL_API double get_loop_frequency (void); // loops/second
    FUNCDLL_API int get_loops (void);       // how many loops
    FUNCDLL_API long get_log_frame (void);

    FUNCDLL_API int is_initialized (void);
    FUNCDLL_API int is_running (void);

    FUNCDLL_API void set_hook (const int proxy, // 1: use position, 0: use proxy
			       void (*ForceHookPtr)
			       (const double position [3],
				const double velocity [3],
				double force [3]));
    FUNCDLL_API int remove_hook (void);

    FUNCDLL_API int enable_dynamic_objects (void);
    FUNCDLL_API int disable_dynamic_objects (void);

    // TODO: In PhysiX this is done with the "maximum depenetration
    // velocity" We could do something similar
    FUNCDLL_API void enable_wait_for_small_forces (void);
    FUNCDLL_API void disable_wait_for_small_forces (void);
    FUNCDLL_API void enable_rise_forces (void);
    FUNCDLL_API void disable_rise_forces (void);

    // World properties
    FUNCDLL_API void set_world_rotation_eulerXYZ (const double x,
						 const double y,
						 const double z);
    FUNCDLL_API void set_world_rotation_quaternion (const double w,
						   const double x,
						   const double y,
						   const double z);
    FUNCDLL_API void get_world_rotation (double* out);

    FUNCDLL_API int set_world_mirror (const int x, const int y, const int z);
    FUNCDLL_API void get_world_mirror (double* out);
    FUNCDLL_API void get_world_order (double* out);

    FUNCDLL_API void set_world_translation (const double x,
					   const double y,
					   const double z);

    FUNCDLL_API void get_world_translation (double* out);

    FUNCDLL_API void set_world_scale (const double x,
				     const double y,
				     const double z);

    FUNCDLL_API void get_world_scale (double* out);

    FUNCDLL_API int get_tool_force (double * /* outputForce [3] */);
    FUNCDLL_API int get_tool_proxy_position (double outputPosition [3]);
    FUNCDLL_API int get_tool_position (double outputPosition [3]);
    FUNCDLL_API int get_tool_velocity (double outputRotation [3]);
    FUNCDLL_API int get_tool_rotation (double outputRotation [4]);

    FUNCDLL_API int is_tool_button_pressed (const int buttonId);

    // Returns the UUID of the object.
    // Please DO NOT assume the id are sequential. They might be random as well
    FUNCDLL_API int create_mesh_object (const double objectPos [3],
					const double objectScale [3],
					const double objectRotation [4],
					const double vertPos [] [3],
					const double normals [] [3],
					const int vertNum,
					const int tris [] [3],  const int triNum,
					const int uvNum,  const double uvs [] [2]);
    FUNCDLL_API int create_sphere_object (const double radius,
				       const double position [3],
				       const double rotation[4]);
    FUNCDLL_API int create_box_object (const double scale [3],
				    const double position [3],
				    const double rotation [4]);

    // Mesh manipulations, might be re-added in the future
    /* FUNCDLL_API int export_object
       (const int objectId, const char * filename); */
    /* FUNCDLL_API int add_object_from_file (const double objectPos [], */
    /*					  const double objectScale [], */
    /*					  const double objectRotation [], */
    /*					  const char* filename); */

    FUNCDLL_API int add_object_to_world (const int objectId);
    FUNCDLL_API int disable_object (const int objectId);
    FUNCDLL_API int enable_object (const int objectId);
    FUNCDLL_API int object_exists (const int objectId);
    // Add a tag to an object, stored in the log file
    FUNCDLL_API int set_object_tag (const int objectId, const char * tag);

    // Haptic properties
    FUNCDLL_API int set_object_texture (const int objectId,
					const unsigned int size_x,
					const unsigned int size_y,
					const float pixels [],
					const int spherical);
    FUNCDLL_API int set_object_material
    (const int objectId,
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
     const double vibration_amplitude);

    FUNCDLL_API int enable_position_interpolation (const int objectId);
    FUNCDLL_API int disable_position_interpolation (const int objectId);

    FUNCDLL_API int enable_rotation_interpolation (const int objectId);
    FUNCDLL_API int disable_rotation_interpolation (const int objectId);

    FUNCDLL_API int set_interpolation_period (const int objectId,
					      const int cycles,
					      const double overshot);

    FUNCDLL_API int set_object_position (const int objectId,
					 const double objectPosition [3]);
    FUNCDLL_API int get_object_position (const int objectId,
					 double objectPosition [3]);
    FUNCDLL_API int set_object_rotation (const int objectId,
					 const double objectRotation [4]);
    FUNCDLL_API int set_object_rotation_euler (const int objectId,
					       const double objectRotation [3]);
    FUNCDLL_API int get_object_rotation (const int objectId,
					 double objectRotation [4]);
    FUNCDLL_API int set_object_scale (const int objectId,
				      const double objectScale [3]);

    void rotate_vector_by_quaternion (const chai3d::cVector3d& v,
				      const chai3d::cQuaternion& q,
				      chai3d::cVector3d& vprime);
  }

  FUNCDLL_API double get (chai3d::cQuaternion q, int el);

  void RotFromChai (const chai3d::cQuaternion chaiRotation, double outputRot [4]);

  chai3d::cVector3d ScaleToChai (const double inputScale [3]);

  void PosFromChai (const chai3d::cVector3d chaiPosition,
		    double externalPosition [3]);

  chai3d::cVector3d PosToChai (const double inputPosition [3]);

  void RotToChai (const double inputRotation [4], chai3d::cQuaternion* out);

  void CreateQuatMirror (bool rightHanded);

  template <typename T>
  inline
  int sig (T x) {
    return (x > 0) - (x < 0);
  }
} // namespace HPGE
