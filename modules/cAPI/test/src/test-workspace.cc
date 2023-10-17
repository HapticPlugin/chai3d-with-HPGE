#include "catch.hpp"
#include "HPGE.h"
#include "chai3d.h"

SCENARIO( "Axis mirror is set correctly", "[mirror, axis, workspace]" ) {
  int msg;
  double out[3];
  chai3d::cQuaternion rotation;
  for (int x = -10; x < 10; ++x) {
    for (int y = -10; y < 10; ++y) {
      for (int z = -10; z < 10; ++z) {
	msg = HPGE::set_world_mirror (x,y,z);
	if ((abs(x) == 1 && abs(y) == 2 && abs(z) == 3) ||
	    (abs(x) == 1 && abs(y) == 3 && abs(z) == 2) ||
	    (abs(x) == 2 && abs(y) == 1 && abs(z) == 3) ||
	    (abs(x) == 2 && abs(y) == 3 && abs(z) == 1) ||
	    (abs(x) == 3 && abs(y) == 2 && abs(z) == 1) ||
	    (abs(x) == 3 && abs(y) == 1 && abs(z) == 2)) {

	  CAPTURE( HPGE::ErrorMsgStrings.at(msg) );
	  REQUIRE( msg == HPGE::SUCCESS );

	  HPGE::get_world_mirror (out);
	  REQUIRE( out[0] == HPGE::sig (x));
	  REQUIRE( out[1] == HPGE::sig (y));
	  REQUIRE( out[2] == HPGE::sig (z));
	  HPGE::get_world_order (out);
	  REQUIRE( out[0] == abs (x) - 1);
	  REQUIRE( out[1] == abs (y) - 1);
	  REQUIRE( out[2] == abs (z) - 1);

	  GIVEN( "The angle is rotated with a valid system") {
	    rotation = chai3d::cQuaternion (-10.0, 1.0, 2.0, 3.0);
	    chai3d::cQuaternion rotout;
	    double outputrotation [4] {0};

	    WHEN( "We rotate the angle back and forth" ) {
	      HPGE::RotFromChai (rotation, outputrotation);
	      HPGE::RotToChai(outputrotation, &rotout);
	      CAPTURE( outputrotation );

	      THEN( "The angle is still the same" ) {
		REQUIRE( rotout == rotation );
	      }
	    }
	  }

	  GIVEN( "Another angle is rotated with a valid system") {
	    rotation = chai3d::cQuaternion (-10.0, -1.0, 20000.0, -3.0);
	    chai3d::cQuaternion rotout;
	    double outputrotation [4] = {0};

	    WHEN( "We rotate the angle back and forth" ) {
	      HPGE::RotFromChai (rotation, outputrotation);
	      HPGE::RotToChai(outputrotation, &rotout);
	      CAPTURE( outputrotation );
	      THEN( "The angle is still the same" ) {
		REQUIRE( rotout == rotation );
	      }
	    }
	  }
	} else {
	  // FAIL to set
	  CAPTURE( HPGE::ErrorMsgStrings.at(msg) );
	  CAPTURE( x );
	  CAPTURE( y );
	  CAPTURE( z );
	  REQUIRE( msg != HPGE::SUCCESS );
	}
      }
    }
  }
}
