#include "catch.hpp"
#include "HPGE.h"
#include "chai3d.h"
#include "debug.hh"

#include <thread>
#include <chrono>

SCENARIO( "We want to set logging parameters", "[logging]" ) {
  GIVEN( "The device is not yet initialized" ) {
    REQUIRE( HPGE::is_initialized () == HPGE::NOT_INITD );
    REQUIRE( HPGE::is_running () == HPGE::NOT_RUNNING );
    WHEN( "We start set the recording parameter" ) {
      const int object_number = 0;
      int objects [1] {  }; // VS does not allow 0-sized arrays
      int status = HPGE::init_logging (0, 0, 0, 0, 0,
							 object_number,
							 objects);
      THEN( "Setting them succeeds" ) {
	REQUIRE( status == HPGE::SUCCESS );
      }
    }

    WHEN( "We initialize the device" ) {
      REQUIRE( HPGE::initialize (-1, 1.0, 0.1) == HPGE::SUCCESS );
      REQUIRE( HPGE::is_running () == HPGE::NOT_RUNNING );

      THEN( "Setting them continue succeeding" ) {
	const int object_number = 0;
	int objects [1] {  }; // VS does not allow 0-sized arrays
	int status = HPGE::init_logging (0, 0, 0, 0, 0,
							 object_number,
							 objects);
	REQUIRE( status == HPGE::SUCCESS );
      }

      REQUIRE( HPGE::is_running () == HPGE::NOT_RUNNING );
      REQUIRE( HPGE::stop() == HPGE::NOT_RUNNING );
      REQUIRE( HPGE::deinitialize () == HPGE::SUCCESS );
      REQUIRE( HPGE::is_running () == HPGE::NOT_RUNNING );
    }
    WHEN( "We initialize the device and start the loop" ) {
      REQUIRE( HPGE::initialize (-1, 1.0, 0.1) == HPGE::SUCCESS );
      REQUIRE( HPGE::is_running () == HPGE::NOT_RUNNING );
      REQUIRE( HPGE::start () == HPGE::SUCCESS );
      REQUIRE( HPGE::is_running () == HPGE::SUCCESS );
      THEN( "Setting them continue succeeding" ) {
	const int object_number = 0;
	int objects [1] {  }; // VS does not allow 0-sized arrays
	int status = HPGE::init_logging (0, 0, 0, 0, 0,
							 object_number,
							 objects);
	REQUIRE( status == HPGE::SUCCESS );
      }
      REQUIRE( HPGE::is_running () == HPGE::SUCCESS );
      REQUIRE( HPGE::stop () == HPGE::SUCCESS );
      REQUIRE( HPGE::is_running () == HPGE::NOT_RUNNING );
      REQUIRE( HPGE::deinitialize () == HPGE::SUCCESS );
      REQUIRE( HPGE::is_running () == HPGE::NOT_RUNNING );
    }
  }
}

SCENARIO( "We want to verify they they get set correctly", "[logging]" ) {
  GIVEN( "The device is not initialized" ) {
    REQUIRE( HPGE::is_initialized () == HPGE::NOT_INITD );
    REQUIRE( HPGE::is_running () == HPGE::NOT_RUNNING );
    WHEN( "We enable the position logging only" ) {
      const int object_number = 0;
      int objects [1] {  }; // VS does not allow 0-sized arrays
      int status = HPGE::init_logging (0, 0, 1, 1, 1,
						       object_number,
						       objects);
      THEN( "Setting them succeeds" ) {
	REQUIRE( status == HPGE::SUCCESS );
      }
    }
    WHEN( "We enable the velocity logging only" ) {
      const int object_number = 0;
      int objects [1] {  }; // VS does not allow 0-sized arrays
      int status = HPGE::init_logging (0, 1, 0, 1, 1,
						       object_number,
						       objects);
      THEN( "Setting them succeeds" ) {
	REQUIRE( status == HPGE::SUCCESS );
      }
    }
    WHEN( "We enable the force logging only" ) {
      const int object_number = 0;
      int objects [1] {  }; // VS does not allow 0-sized arrays
      int status = HPGE::init_logging (0, 1, 1, 0, 1,
						       object_number,
						       objects);
      THEN( "Setting them succeeds" ) {
	REQUIRE( status == HPGE::SUCCESS );
      }
    }
    WHEN( "We enable the interaction logging only but with 0 objects" ) {
      const int object_number = 0;
      int objects [1] {  }; // VS does not allow 0-sized arrays
      int status = HPGE::init_logging (0, 1, 1, 1, 0,
						       object_number,
						       objects);
      THEN( "Setting them succeeds" ) {
	REQUIRE( status == HPGE::SUCCESS );
      }
    }
    WHEN( "We enable the interaction logging with non-existing objects" ) {
      const int object_number = 2;
      int objects [object_number] { 0, 1 };
      int status = HPGE::init_logging (0, 1, 1, 1, 0,
						       object_number,
						       objects);
      THEN( "Setting them _does not_ succeeds" ) {
	REQUIRE( status == HPGE::OBJ_NOT_FOUND );
      }
    }
    WHEN( "We enable the interaction logging with non-existing objects" ) {
      const int object_number = 2;
      int objects [object_number] { 0, 1 };
      int status = HPGE::init_logging (0, 1, 1, 1, 0,
						       object_number,
						       objects);
      THEN( "Setting them _does not_ succeeds" ) {
	REQUIRE( status == HPGE::OBJ_NOT_FOUND );
      }
    }
    WHEN( "We enable everything but interaction with non-existing objects" ) {
      const int object_number = 2;
      int objects [object_number] { 0, 1 };
      int status = HPGE::init_logging (0, 0, 0, 0, 1,
						       object_number,
						       objects);
      THEN( "Setting them _does not_ succeeds" ) {
	REQUIRE( status == HPGE::SUCCESS );
      }
    }
  }
}

SCENARIO( "Set object when loop is running", "[logging]" ) {
  GIVEN( "The device is initialized" ) {
    REQUIRE( HPGE::initialize (-1, 1.0, 0.1) == HPGE::SUCCESS );
    REQUIRE( HPGE::is_initialized () == HPGE::SUCCESS );
    REQUIRE( HPGE::start () == HPGE::SUCCESS );
    REQUIRE( HPGE::is_running () == HPGE::SUCCESS );
    // still initialized after start running
    REQUIRE( HPGE::is_initialized () == HPGE::SUCCESS );
    WHEN( "We enable the interaction logging with existing objects" ) {
      const int object_number = 2;
      int obj1 = createCubeMesh ();
      REQUIRE_FALSE( obj1 == -1 );
      int obj2 = createCubeMesh ();
      REQUIRE_FALSE( obj2 == -1 );

      CAPTURE( obj1 );
      CAPTURE( obj2 );

      int objects [object_number] { obj1, obj2 };

      int status = HPGE::init_logging (0, 1, 1, 1, 0,
						       object_number,
						       objects);
      THEN( "Setting them succeeds" ) {
	REQUIRE( status == HPGE::SUCCESS );
      }
    }
    REQUIRE( HPGE::deinitialize () == HPGE::SUCCESS );
  }
}

SCENARIO( "Data are actually saved", "[logging]" ) {
  GIVEN( "We declare what we want to save" ) {
    const int long_capture_time =  200;// ms
    REQUIRE( HPGE::initialize (-1, 1.0, 0.1) == HPGE::SUCCESS );
    REQUIRE( HPGE::is_running () == HPGE::NOT_RUNNING );
    REQUIRE( HPGE::start () == HPGE::SUCCESS );
    REQUIRE( HPGE::is_initialized () == HPGE::SUCCESS );
    REQUIRE( HPGE::is_running () == HPGE::SUCCESS );
    const int object_number = 2;
  int obj1 = createCubeMesh ();
    REQUIRE_FALSE( obj1 == -1 );
    int obj2 = createCubeMesh ();
    REQUIRE_FALSE( obj2 == -1 );

    CAPTURE( obj1 );
    CAPTURE( obj2 );

    int objects [object_number] { obj1, obj2 };

    int status = HPGE::init_logging (0, 1, 1, 1, 0,
						     object_number,
						     objects);
    REQUIRE( status == HPGE::SUCCESS );
    WHEN( "We stop recording without having started it" ) {
      const char * output_name {"./output.log"};
      int status = HPGE::stop_logging_and_save (output_name);
      THEN( "Stopping fails" ) {
	REQUIRE( status == HPGE::NOT_RECORDING );
      }
    }
    WHEN( "We start recording with an invalid sampling rate" ) {
      const int sampling_rate = -1;
      int status = HPGE::start_logging (sampling_rate);
      REQUIRE( status == HPGE::SAMPLING_TOO_SMALL );

      const char * output_name {"./output.log"};

      // Accumulate enough data
      std::this_thread::sleep_for(std::chrono::milliseconds(long_capture_time));

      status = HPGE::stop_logging_and_save (output_name);
      THEN( "Stopping fails (never started)" ) {
	REQUIRE( status == HPGE::NOT_RECORDING );
      }
    }
    WHEN( "We start recording" ) {
      const int sampling_rate = 1;
      int status = HPGE::start_logging (sampling_rate);
      REQUIRE( status == HPGE::SUCCESS );
      status = HPGE::start_logging (sampling_rate);
      REQUIRE( status == HPGE::ALREADY_RECORDING );

      const char * output_name {"./output.log"};

      // Accumulate enough data
      std::this_thread::sleep_for(std::chrono::milliseconds (long_capture_time));

      status = HPGE::stop_logging_and_save (output_name);
      THEN( "Stopping succeeds" ) {
	REQUIRE( status == HPGE::SUCCESS );
      }
    }
    WHEN( "We start and stop recording many times" ) {
      const int sampling_rate = 1;
      const char * output_name {"./output.log"};

      THEN( "Start and stopping fast continue working ") {
	// Must be more than RECORDING_DATA_BUFFER_SIZE
	for (int i = 0; i < 100; ++i) {
	  int status = HPGE::start_logging (1);
	  REQUIRE( status == HPGE::SUCCESS );
	  status = HPGE::start_logging (sampling_rate);
	  REQUIRE( status == HPGE::ALREADY_RECORDING );
	  std::this_thread::sleep_for (std::chrono::milliseconds (1));
	  status = HPGE::stop_logging_and_save (output_name);
	  REQUIRE( status == HPGE::SUCCESS );
	}
	// Must be more than RECORDING_DATA_BUFFER_SIZE
	// with and without sleep
	for (int i = 0; i < 100; ++i) {
	  int status = HPGE::start_logging (sampling_rate);
	  REQUIRE( status == HPGE::SUCCESS );
	  status = HPGE::start_logging (sampling_rate);
	  REQUIRE( status == HPGE::ALREADY_RECORDING );
	  // std::this_thread::sleep_for(std::chrono::milliseconds(1));
	  status = HPGE::stop_logging_and_save (output_name);
	  REQUIRE( status == HPGE::SUCCESS );
	}
      }
    }
    REQUIRE( HPGE::deinitialize () == HPGE::SUCCESS );
    REQUIRE( HPGE::is_running () == HPGE::NOT_RUNNING );
  }
}