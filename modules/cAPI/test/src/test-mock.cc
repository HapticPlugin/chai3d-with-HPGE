#include "catch.hpp"
#include "HPGE.h"
#include "chai3d.h"

SCENARIO( "Test a real device", "[devices, haptics]" ) {
  // This gets done only if a real device is found. This is true only
  // if countDevices is > 1 (mock device always exists)

  GIVEN( "We count the number of devices" ) {
    int devs = HPGE::count_devices ();
    THEN( "The device number must be at least 1" ) {
      REQUIRE( devs >= 1 );
    }

    // CAPTURE( devs ); // Log the number to help debugging
    // if (devs > 1) {
    //   CAPTURE( "There is at least one device connected");

    //   WHEN( "We check that the device is working" ) {
    //	int status = HPGE::initialize (0, 1.0, 0.05);
    //	REQUIRE( status == HPGE::SUCCESS );
    //   }
    // } else {
    //   // There's something wrong if this is false (where's the mock?)
    //   REQUIRE( devs == 1 );
    // }
  }
}
