#include "catch.hpp"
#include "HPGE.h"
#include "chai3d.h"

SCENARIO( "Test initialization" ) {
  GIVEN( "The device is initialized" ) {
    REQUIRE( HPGE::initialize(-1, 1.0, 1.0) == HPGE::SUCCESS );
    WHEN( "We initialize the device another time" ) {
      int ret = HPGE::initialize(-1, 1.0, 1.0);
      THEN( "The initialization fails") {
	REQUIRE( ret != HPGE::SUCCESS);
	REQUIRE( ret == HPGE::ALREADY_INITD);
      }
    }
    REQUIRE( HPGE::deinitialize() == HPGE::SUCCESS );
  }
  GIVEN( "The device is initialized" ) {
    REQUIRE( HPGE::initialize(-1, 1.0, 1.0) == HPGE::SUCCESS );
    WHEN( "We start the haptic thread" ) {
      int ret = HPGE::start();
      CAPTURE( ret );
      THEN( "We can stop the thread without problems or deadlocks" ) {
	ret = HPGE::stop ();
	REQUIRE( ret == HPGE::SUCCESS );
      }
      THEN( "Stopping twice is not a problem" ) {
	ret = HPGE::stop ();
	REQUIRE( ret == HPGE::SUCCESS );
	ret = HPGE::stop ();
	REQUIRE( ret == HPGE::NOT_RUNNING );
      }
    }
    REQUIRE( HPGE::deinitialize() == HPGE::SUCCESS );
  }
  GIVEN( "The device is initialized and the thread started" ) {
    REQUIRE( HPGE::initialize(-1, 1.0, 1.0) == HPGE::SUCCESS );
    REQUIRE( HPGE::start() == HPGE::SUCCESS );

    WHEN( "We deinitialize instead of stopping" ) {
      HPGE::deinitialize();
      THEN( "We can de-initialize it again without problems") {
	HPGE::deinitialize();
	REQUIRE( HPGE::deinitialize() == HPGE::NOT_INITD );
      }
    }
    HPGE::deinitialize();
    REQUIRE( HPGE::deinitialize() == HPGE::NOT_INITD );
  }
  GIVEN( "The device is initialized and the thread started" ) {
    REQUIRE( HPGE::initialize(-1, 1.0, 1.0) == HPGE::SUCCESS );
    REQUIRE( HPGE::start() == HPGE::SUCCESS );

    WHEN( "We deinitialize instead of stopping" ) {
      REQUIRE( HPGE::deinitialize () == HPGE::SUCCESS );
      THEN( "We can de-initialize it again without problems") {
	REQUIRE( HPGE::deinitialize() == HPGE::NOT_INITD );
      }
    }
    REQUIRE( HPGE::deinitialize() == HPGE::NOT_INITD );
  }
}
