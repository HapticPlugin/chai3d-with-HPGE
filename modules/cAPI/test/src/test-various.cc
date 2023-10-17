#include "catch.hpp"
#include "HPGE.h"
#include "chai3d.h"

SCENARIO( "Test quaternion accessor", "[quaternions]" ) {
  GIVEN( "A quaternion" ) {
    chai3d::cQuaternion q = chai3d::cQuaternion (1.0,20.0,300.0,400.0);
    WHEN ( "We access existing fiels" ) {
      THEN ( "The read values are correct" ) {
	REQUIRE( q.w == HPGE::get (q, 0) );
	REQUIRE( q.x == HPGE::get (q, 1) );
	REQUIRE( q.y == HPGE::get (q, 2) );
	REQUIRE( q.z == HPGE::get (q, 3) );
      }
    }

    WHEN ( "We access non-existing fiels" ) {
      THEN ( "The read values are zeros" ) {
	REQUIRE( 0.0 == HPGE::get (q, 4) );
	REQUIRE( 0.0 == HPGE::get (q, 5) );
	REQUIRE( 0.0 == HPGE::get (q, 6) );
	REQUIRE( 0.0 == HPGE::get (q, 7) );
      }
    }
  }
}
