#include "catch.hpp"
#include "HPGE.h"
#include "chai3d.h"
#include <atomic>
#include <thread>
#include <chrono>

void example_hook (const double position [3],
		   const double velocity [3],
		   double force [3]) {
  force [0] = position [0] + velocity [0];
  force [1] = position [1] + velocity [1];
  force [2] = position [2] + velocity [2];
  return;
}

std::atomic<int> global_hook_run_times{0};

void global_hook (const double position [3],
		  const double velocity [3],
		  double force [3]) {
  global_hook_run_times.fetch_add(1);
  return;
}

SCENARIO( "We want to register a hook", "[hook]" ) {
  GIVEN( "The device is initilized" ) {
    HPGE::initialize(-1, 10.0, 10.0);
    WHEN( "No hook is assigned" ) {
      THEN( "Nothing bad happens if we try to run the hook") {
	double pos [3] {1.0, 2.0, 3.0};
	double vel [3] {10.0, 20.0, 30.0};
	double frc [3];
	bool has_run = HPGE::debug::run_hook (pos, vel, frc);
	REQUIRE( true ); // No segfault
	REQUIRE( has_run == false ); // not run
      }
    }
    WHEN( "We set the hook" ) {
      HPGE::set_hook (1, // proxy or position
			 example_hook);
      THEN( "The hook is called as expected") {
	double pos [3] {1.0, 2.0, 3.0};
	double vel [3] {10.0, 20.0, 30.0};
	double frc [3];
	bool has_run = HPGE::debug::run_hook (pos, vel, frc);
	REQUIRE( has_run );
	// CAPTURE(frc [0], ", ", frc [1], ", ", frc [2],"\n");

	REQUIRE( frc [0] == pos [0] + vel [0] );
	REQUIRE( frc [1] == pos [1] + vel [1] );
	REQUIRE( frc [2] == pos [2] + vel [2] );
      }
    }
    WHEN( "We remove the hook" ) {
      int res = HPGE::remove_hook ();
      REQUIRE( res == HPGE::SUCCESS );

      THEN( "Calling the hook does not segfault" ) {
	double pos [3] {1.0, 2.0, 3.0};
	double vel [3] {10.0, 20.0, 30.0};
	double frc [3];
	bool has_run = HPGE::debug::run_hook (pos, vel, frc);
	REQUIRE( true ); // No segfault
	REQUIRE( has_run == false ); // not run
      }
    }
    WHEN( "We remove the hook twice" ) {
      int res = HPGE::remove_hook ();
      THEN( "The removal fails" ) {
	CAPTURE( HPGE::ErrorMsgStrings.at (res) );
	REQUIRE( res != HPGE::SUCCESS );
      }
    }
  }
}

SCENARIO( "We want to register a hook in the main loop",
	  "[hook]" ) {
  GIVEN( "The device is initilized and started" ) {
    HPGE::start();
    WHEN( "No hook is registered" ) {
      THEN( "Nothing bad happens" ) {
	// We sleep a bit to be sure the loop starts
	double pos [3];
	for (int i = 0; i < 10; ++i ) {
	  HPGE::get_tool_position (pos);
	}
      }
    }
    WHEN( "No hook is registered" ) {
      THEN( "Nothing bad happens" ) {
	// We sleep a bit to be sure the loop starts
	double pos [3];
	for (int i = 0; i < 10; ++i ) {
	  HPGE::get_tool_position (pos);
	}
      }
    }
    WHEN( "A hook is registered" ) {
      int res;
      res = HPGE::stop ();
      res = HPGE::deinitialize ();
      res = HPGE::initialize (-1, 10.0, 10.0);
      CAPTURE( HPGE::ErrorMsgStrings.at (res) );
      REQUIRE( res == HPGE::SUCCESS );

      global_hook_run_times = 0;
      REQUIRE(global_hook_run_times == 0);

      HPGE::set_hook (1, global_hook);

      res = HPGE::start ();
      CAPTURE( HPGE::ErrorMsgStrings.at (res) );
      REQUIRE( res == HPGE::SUCCESS );


      THEN( "The hook has been run (at least once)" ) {
	double pos [3];
	// We sleep a bit to be sure the loop starts
	HPGE::get_tool_position (pos);
	HPGE::get_tool_velocity (pos);

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	int last_val = global_hook_run_times;
	CAPTURE( global_hook_run_times );
	REQUIRE( global_hook_run_times > 0 );

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	CAPTURE( global_hook_run_times );
	REQUIRE( global_hook_run_times > last_val );
      }
    }
  }
}
