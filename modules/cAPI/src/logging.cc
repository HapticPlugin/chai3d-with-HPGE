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
#include <chrono>
#include <mutex>

#include <fstream> // Write data to file
#include <thread> // debug

namespace HPGE {
  std::ostream& operator<<(std::ostream& os, const SavedFrame& f) {
    os << f.timestamp << "," << f.ticks << ","
       << f.position [0] << "," << f.position [1] << "," << f.position [2] << ","
       << f.velocity [0] << "," << f.velocity [1] << "," << f.velocity [2] << ","
       << f.force [0] << "," << f.force [1] << "," << f.force [2] << "," << f.notes;

    // TODO: add InteractionForces output
    return os;
  }

  std::ostream& operator<<(std::ostream& os, const SavedData& sd) {
    // TODO: print header, might be an option
    // FIXME: We should have a way to print version info too
    os << "frame" << "," << "timestamp" << "," << "ticks" << ","
       << "position_x" << "," << "position_y" << "," << "position_z" << ","
       << "velocity_x" << "," << "velocity_y" << "," << "velocity_z" << ","
       <<  "force_x"   << "," <<   "force_y"  << "," <<   "force_z" << ","
       << "notes"
       << "\n";

    long frame = 0;
    for (auto f : sd.frames) {
	os << frame++ << "," << f <<'\n';
    }
    return os;
  }

  std::array<SavedData*,RECORDING_DATA_BUFFER_SIZE> Data = { nullptr };
  SavedData * current_data; // Pointer storing where the main loop
			    // must write things

  std::atomic<bool> logging {false};
  std::atomic<int> logging_position {0};
  std::mutex data_mutex;

  Logging log { 0
      , false
      , false
      , false
      , false
      , false
      , {} // std::vector
  };
  unsigned long long Now (void) {
      // on windows, high_resolution_clock has a different epoch O.O
    return std::chrono::duration_cast<std::chrono::milliseconds>(
	std::chrono::system_clock::now ().time_since_epoch ()).count ();
  }

  int StopLogging (void) {
    if (!logging.load ()) { return -1;}
    // Stop logging immediately
    logging.store (false);
    data_mutex.lock ();
    // Move the point where we save the data
    int data_index = logging_position.load ();
    int prev_index = data_index; // used to get the old data
    if ((data_index + 1) >= RECORDING_DATA_BUFFER_SIZE) {
      logging_position.store (0);
    } else {
      logging_position.store (data_index + 1);
    }
    data_mutex.unlock ();
    // Data _must_ be deleted by the _saving_ (stop) function
    return prev_index;
  }

  extern "C" {
    int init_logging (const int device_coordinates,
		      const int position,
		      const int velocity,
		      const int force,
		      const int interaction_forces,
		      const int object_number,
		      const int * objects) {
      // Verify that every object passed exists. Fail if they don't
      // (but only if interaction_forces is set to true (= 0))
      std::vector<int> objectIds;
      if (interaction_forces == 0) {
	for (int id = 0; id < object_number; ++id) {
	  auto obj = getObjectFromMap (objects [id]);
	  // We don't really use the obj returned (just check for
	  // nullity) so we don't need locking objects
	  if (obj == nullptr) { return retErr (OBJ_NOT_FOUND); }
	  objectIds.push_back (id);
	}
      }
      // Maximum sampling rate by default
      log.sampling_rate = 1;

      // set parameters
      log.position = (position == 0);
      log.velocity = (velocity == 0);
      log.force = (force == 0);
      log.interaction_forces = (interaction_forces == 0);
      log.device_coordinates = (device_coordinates == 0);

      if (interaction_forces == 0) {
	for (auto &obj : objectIds) {
	  log.object_forces.push_back (obj);
	}
      }

      return retErr (SUCCESS);
    }

    int start_logging (const int sampling_rate) {
      if (sampling_rate <= 0) {
	return retErr (SAMPLING_TOO_SMALL);
      }
      if (logging.load ()) {
	return retErr (ALREADY_RECORDING);
      }
      data_mutex.lock ();
      // We can actually record. Start by updating the sampling rate
      log.sampling_rate = sampling_rate;

      int data_index = logging_position.load ();
      // std::cout << "index: " << data_index << "\n";
      Data [data_index] = new SavedData ();

      current_data = Data [data_index];
      data_mutex.unlock ();
      // Toggle at last so that the other thread can work on
      // initialized data
      logging.store (true);
      return retErr (SUCCESS);
    }
  }
  int is_logging (void) {
    if (!logging.load ()) {
      return retErr (NOT_RECORDING);
    }
    return retErr (SUCCESS);
  }

  int stop_logging_and_save (const char * filename) {
    int prev_index = StopLogging ();
    if (prev_index == -1) { return retErr (NOT_RECORDING); }
    data_mutex.lock ();
    // FIXME: Might return immediately and start a thread that does
    // the saving. Benchmark?
    // FIXME: debug only?
    // std::cerr << "Writing data to: " << std::string(filename)
    //	      << ":" << prev_index << "\n";

    // Simulate writing
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::ofstream output;
    output.open (filename); // , std::ios::out | std::ios::binary
    output << *Data [prev_index];
    output.close ();
    delete Data[prev_index];

    // Data [prev_index]; // Data to write is here
    // FIXME: Problems!
    // delete Data [prev_index];
    data_mutex.unlock ();
    return retErr (SUCCESS);
  }

  int stop_logging_and_get (SavedData * data) {
    return retErr (NOT_IMPLEMENTED);
    int prev_index = StopLogging ();
    if (prev_index == -1) { return retErr (NOT_RECORDING); }
    // Output the pointer
    data = Data [prev_index];
    // FIXME: add a way to free the memory (delete Data [prev_index])
    // allocated by new
    return retErr (SUCCESS);
  }
}
