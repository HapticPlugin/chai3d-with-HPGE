#include "catch.hpp"
#include "HPGE.h"
#include "chai3d.h"
#include "debug.hh"

#include <thread> // required to check interpolation time
#include <chrono> // required to check interpolation time

const int period {1000};

SCENARIO( "Test object position interpolation", "[position]" ) {
  GIVEN( "A working haptic loop" ) {
    REQUIRE( HPGE::initialize (-1, 1.0, 0.1) == HPGE::SUCCESS );
    REQUIRE( HPGE::start () == HPGE::SUCCESS );
    int obj = createCubeMesh ();

    REQUIRE( HPGE::add_object_to_world (obj) == HPGE::SUCCESS );

    WHEN( "We enable position interpolation" ) {
      REQUIRE( HPGE::enable_position_interpolation (obj)
		== HPGE::SUCCESS );

      THEN( "The position interpolation is enabled" ) {
	REQUIRE( HPGE::debug::isPositionInterpolationEnabled (obj) );
      }
    }
    WHEN( "We disable position interpolation" ) {
      REQUIRE( HPGE::disable_position_interpolation (obj)
		== HPGE::SUCCESS );

      THEN( "The position interpolation is enabled" ) {
	REQUIRE_FALSE( HPGE::debug::isPositionInterpolationEnabled (obj) );
      }
    }
    // Should not be required to stop, tested in test-thread.cc
    // REQUIRE( HPGE::stop () == HPGE::SUCCESS );
    REQUIRE( HPGE::deinitialize () == HPGE::SUCCESS );
  }

  GIVEN( "An object is created with interpolation enabled" ) {
    REQUIRE( HPGE::initialize (-1, 1.0, 0.1) == HPGE::SUCCESS );
    REQUIRE( HPGE::start () == HPGE::SUCCESS );
    int obj = createCubeMesh ();
    REQUIRE( HPGE::add_object_to_world (obj) == HPGE::SUCCESS );
    REQUIRE( HPGE::enable_position_interpolation (obj) == HPGE::SUCCESS );

    WHEN( "We set the initial object position" ) {
      double setpos [3] { 0.0, 0.0, 0.0 };
      REQUIRE( HPGE::set_object_position (obj, setpos) == HPGE::SUCCESS );
      THEN( "The position is the one we want" ) {
	double readpos [3];
	REQUIRE( HPGE::get_object_position (obj, readpos)
		 == HPGE::SUCCESS );

	REQUIRE( readpos [0] == setpos [0] );
	REQUIRE( readpos [1] == setpos [1] );
	REQUIRE( readpos [2] == setpos [2] );
      }
    }

    WHEN( "We set the position and then move the object" ) {
      double setpos [3] { 0.0, 0.0, 0.0 };
      REQUIRE( HPGE::set_object_position (obj, setpos) == HPGE::SUCCESS );

      THEN( "The position is the one we want" ) {
	double readpos [3];
	REQUIRE( HPGE::get_object_position (obj, readpos)
		 == HPGE::SUCCESS );

	REQUIRE( readpos [0] == setpos [0] );
	REQUIRE( readpos [1] == setpos [1] );
	REQUIRE( readpos [2] == setpos [2] );
      }

      setpos [0] = 2.0;
      setpos [1] = 10.0;
      setpos [2] = 300.0;


      // We store the time at which we set the object position
      auto t0 = std::chrono::high_resolution_clock::now();
      // We set an high interpolation period
      REQUIRE( HPGE::set_interpolation_period (obj, period, 1.0)
	       == HPGE::SUCCESS);
      REQUIRE( HPGE::debug::isPositionInterpolationEnabled (obj) );
      REQUIRE( HPGE::set_object_position (obj, setpos) == HPGE::SUCCESS );

      THEN( "The position is different from the one we setted" ) {
	double readpos [3];
	REQUIRE( HPGE::get_object_position (obj, readpos)
		 == HPGE::SUCCESS );

	auto t1 = std::chrono::high_resolution_clock::now ();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>
	  (t1 - t0);

	CAPTURE( ms.count () );

	REQUIRE( readpos [0] != setpos [0] );
	REQUIRE( readpos [1] != setpos [1] );
	REQUIRE( readpos [2] != setpos [2] );
      }

      THEN( "The position is reached in about the expected time" ) {
	double readpos [3];
	REQUIRE( HPGE::get_object_position (obj, readpos)
		 == HPGE::SUCCESS );
	std::this_thread::sleep_for(std::chrono::milliseconds (period + 300));

	auto t1 = std::chrono::high_resolution_clock::now ();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>
	  (t1 - t0);

	REQUIRE( (t1 - t0).count () >= period );
	REQUIRE( HPGE::get_object_position (obj, readpos)
		 == HPGE::SUCCESS );

	REQUIRE( readpos [0] == setpos [0] );
	REQUIRE( readpos [1] == setpos [1] );
	REQUIRE( readpos [2] == setpos [2] );
      }
    }
    // Should not be required to stop, tested in test-thread.cc
    // REQUIRE( HPGE::stop () == HPGE::SUCCESS );
    REQUIRE( HPGE::deinitialize () == HPGE::SUCCESS );
  }
}

// FIXME: Not working
// SCENARIO( "Test object rotation interpolation", "[interpolation]" ) {
//   GIVEN( "A working haptic loop" ) {
//     REQUIRE( HPGE::initialize (-1, 1.0, 0.1) == HPGE::SUCCESS );
//     REQUIRE( HPGE::start () == HPGE::SUCCESS );
//     int obj = createCubeMesh ();

//     REQUIRE( HPGE::add_object_to_world (obj) == HPGE::SUCCESS );

//     WHEN( "We enable rotation interpolation" ) {
//       REQUIRE( HPGE::enable_rotation_interpolation (obj)
//		== HPGE::SUCCESS );

//       THEN( "The rotation interpolation is enabled" ) {
//	REQUIRE( HPGE::debug::isRotationInterpolationEnabled (obj) );
//       }
//     }
//     WHEN( "We disable rotation interpolation" ) {
//       REQUIRE( HPGE::disable_rotation_interpolation (obj)
//		== HPGE::SUCCESS );

//       THEN( "The rotation interpolation is enabled" ) {
//	REQUIRE_FALSE( HPGE::debug::isRotationInterpolationEnabled (obj) );
//       }
//     }
//     // Should not be required to stop, tested in test-thread.cc
//     // REQUIRE( HPGE::stop () == HPGE::SUCCESS );
//     REQUIRE( HPGE::deinitialize () == HPGE::SUCCESS );
//   }

//   GIVEN( "An object is created with interpolation enabled" ) {
//     REQUIRE( HPGE::initialize (-1, 1.0, 0.1) == HPGE::SUCCESS );
//     REQUIRE( HPGE::start () == HPGE::SUCCESS );
//     int obj = createCubeMesh ();
//     REQUIRE( HPGE::add_object_to_world (obj) == HPGE::SUCCESS );
//     REQUIRE( HPGE::enable_rotation_interpolation (obj) == HPGE::SUCCESS );

//     WHEN( "We set the initial object rotation" ) {
//       double setrot [4] { 1.0, 10.0, 100.0, 1000.0 };
//       REQUIRE( HPGE::set_object_rotation (obj, setrot) == HPGE::SUCCESS );
//       THEN( "The rotation is the one we want" ) {
//	double readrot [4];
//	REQUIRE( HPGE::get_object_rotation (obj, readrot)
//		 == HPGE::SUCCESS );

//	REQUIRE( readrot [0] == Approx( setrot [0] ) );
//	REQUIRE( readrot [1] == Approx( setrot [1] ) );
//	REQUIRE( readrot [2] == Approx( setrot [2] ) );
//	REQUIRE( readrot [3] == Approx( setrot [3] ) );
//       }
//     }

//     WHEN( "We set the rotation and then rotate the object" ) {
//       double setrot [4] { 0.0, 0.04993761694389223, 0.0, 0.9987523388778445 };
//       REQUIRE( HPGE::set_object_rotation (obj, setrot) == HPGE::SUCCESS );
//       REQUIRE( HPGE::disable_rotation_interpolation (obj) == HPGE::SUCCESS );
//       REQUIRE_FALSE( HPGE::debug::isRotationInterpolationEnabled (obj) );
//       THEN( "The rotation is the one we want" ) {
//	double readrot [4];
//	REQUIRE( HPGE::get_object_rotation (obj, readrot)
//		 == HPGE::SUCCESS );

//	CAPTURE( readrot [0] );
//	CAPTURE( readrot [1] );
//	CAPTURE( readrot [2] );
//	CAPTURE( readrot [3] );

//	CAPTURE( setrot [0] );
//	CAPTURE( setrot [1] );
//	CAPTURE( setrot [2] );
//	CAPTURE( setrot [3] );

//	REQUIRE( readrot [0] == Approx( setrot [0] ));
//	REQUIRE( readrot [1] == Approx( setrot [1] ));
//	REQUIRE( readrot [2] == Approx( setrot [2] ));
//	REQUIRE( readrot [3] == Approx( setrot [3] ));
//       }

//       setrot [0] = 0.8228813130667904;
//       setrot [1] = 0.02837521769195829;
//       setrot [2] = 0.0;
//       setrot [3] = 0.5675043538391658;


//       // We store the time at which we set the object rotation
//       auto t0 = std::chrono::high_resolution_clock::now();
//       // We set an high interpolation period
//       REQUIRE( HPGE::set_interpolation_period (obj, period, 1.0)
//	       == HPGE::SUCCESS);
//       REQUIRE( HPGE::debug::isPositionInterpolationEnabled (obj) );
//       REQUIRE( HPGE::set_object_rotation (obj, setrot) == HPGE::SUCCESS );

//       THEN( "The rotation is different from the one we setted" ) {
//	REQUIRE( HPGE::debug::isPositionInterpolationEnabled (obj) );
//	double readrot [4];
//	REQUIRE( HPGE::get_object_rotation (obj, readrot)
//		 == HPGE::SUCCESS );

//	auto t1 = std::chrono::high_resolution_clock::now ();
//	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>
//	  (t1 - t0);

//	CAPTURE( ms.count () );

//	CAPTURE( readrot [0] );
//	CAPTURE( readrot [1] );
//	CAPTURE( readrot [2] );
//	CAPTURE( readrot [3] );

//	bool equals = ((readrot [0] == Approx( setrot [0] )) ||
//		       (readrot [1] == Approx( setrot [1] )) ||
//		       (readrot [2] == Approx( setrot [2] )) ||
//		       (readrot [3] == Approx( setrot [3] )));

//	REQUIRE_FALSE( equals );
//       }

//       THEN( "The position is reached in about the expected time" ) {
//	double readrot [4];
//	REQUIRE( HPGE::get_object_rotation (obj, readrot)
//		 == HPGE::SUCCESS );
//	std::this_thread::sleep_for(std::chrono::milliseconds (period));

//	auto t1 = std::chrono::high_resolution_clock::now ();
//	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>
//	  (t1 - t0);

//	REQUIRE( (t1 - t0).count () >= period );
//	REQUIRE( HPGE::get_object_rotation (obj, readrot)
//		 == HPGE::SUCCESS );

//	REQUIRE( readrot [0] == setrot [0] );
//	REQUIRE( readrot [1] == setrot [1] );
//	REQUIRE( readrot [2] == setrot [2] );
//	REQUIRE( readrot [3] == setrot [3] );
//       }
//     }
//     // Should not be required to stop, tested in test-thread.cc
//     // REQUIRE( HPGE::stop () == HPGE::SUCCESS );
//     REQUIRE( HPGE::deinitialize () == HPGE::SUCCESS );
//   }
// }
