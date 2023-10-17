#include "catch.hpp"
#include "HPGE.h"
#include "chai3d.h"

#include <thread>

SCENARIO( "Test initialization and haptic loop work" ) {
  GIVEN( "We start an uninitialized device" ) {
    REQUIRE( HPGE::start() == HPGE::NOT_INITD );
    REQUIRE( HPGE::stop() == HPGE::NOT_INITD );
    REQUIRE( HPGE::deinitialize() == HPGE::NOT_INITD );
  }

  GIVEN( "We initialize our device" ) {
    int msg = HPGE::initialize(-1, 1.0, 1.0);
    CAPTURE( HPGE::ErrorMsgStrings.at(msg) );
    REQUIRE ( msg == HPGE::SUCCESS );
    WHEN( "We test various initialization conditions" ) {
      int msg = HPGE::initialize(-1, 1.0, 1.0);
      CAPTURE( HPGE::ErrorMsgStrings.at(msg) );
      REQUIRE ( msg == HPGE::ALREADY_INITD );
      msg = HPGE::deinitialize();
      CAPTURE( HPGE::ErrorMsgStrings.at(msg) );
      REQUIRE ( msg == HPGE::SUCCESS );
    }
    CAPTURE("Cleared the state (deinit) before leaving");
    HPGE::deinitialize();
  }

  GIVEN( "The device is initialized" ) {
    HPGE::initialize(-1, 1.0, 1.0);
    WHEN( "Calling stop on a deinitialized device does nothing" ) {
      HPGE::start();
      HPGE::stop();
      HPGE::deinitialize();
      REQUIRE( HPGE::stop() == HPGE::NOT_INITD );
    }
    HPGE::deinitialize();
  }

  GIVEN( "The device is working and the thread is running" ) {
    REQUIRE( HPGE::initialize (-1, 1.0, 1.0) == HPGE::SUCCESS );
    REQUIRE( HPGE::start () == HPGE::SUCCESS );

    // more than 1 second
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    double loop = HPGE::get_loops ();
    REQUIRE_FALSE( loop == -1 );
    CAPTURE( loop );
    REQUIRE(loop > 1000);
    WHEN( "Nobody is pressing the button" ) {
      REQUIRE(HPGE::is_tool_button_pressed(0) != 0);
    }

    HPGE::stop();
    HPGE::deinitialize();
  }
}

SCENARIO ( "Test a non-existing device" ) {
  GIVEN ( "We try to initialize a device that does not exist ") {
    REQUIRE( HPGE::initialize(999, 1.0, 0.01) == HPGE::DEVICE_NOT_FOUND );

    REQUIRE( HPGE::is_tool_button_pressed(0) == -1 );
  }
}
