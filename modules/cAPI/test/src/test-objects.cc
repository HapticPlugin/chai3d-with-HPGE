// This test requires test-mock to pass!
#include "catch.hpp"
#include "HPGE.h"
#include "chai3d.h"
#include "debug.hh"

#include <thread>

const int mockDevId = -1;
const int buffer = 40;
const double origin [3]{0, 0, 0};

SCENARIO( "We add a cube (mesh) to the world", "[objects]" ) {
  // int status =
  HPGE::initialize (mockDevId, 10.0, 1.0);
  // REQUIRE( status == HPGE::SUCCESS );

  GIVEN( "A cube in the world" ) {
    int obj = createCubeMesh ();
    REQUIRE_FALSE( obj == -1 );
    // status =
    HPGE::add_object_to_world (obj);

    // CAPTURE( status );

    double pos [3];
    double rot [4];
    HPGE::get_object_position(obj, pos);

    double newpos [3]{10, 20, 30};
    double newrot [4]{0.0, 0.0, 1.0, 0};

    REQUIRE( pos [0] == 0);
    REQUIRE( pos [1] == 0);
    REQUIRE( pos [2] == 0);

    WHEN( "We change the coordinate system" ) {
      REQUIRE( HPGE::set_world_mirror(2, 3, -1) == HPGE::SUCCESS);

      THEN( "The object position should not change!" ) {
	HPGE::get_object_position(obj, pos);

	REQUIRE( pos [0] == 0);
	REQUIRE( pos [1] == 0);
	REQUIRE( pos [2] == 0);
      }
    }

    WHEN( "We change the object position" ) {
      REQUIRE( HPGE::set_world_mirror(1, 2, 3) == HPGE::SUCCESS);
      HPGE::set_object_position(obj, newpos);

      THEN( "The object position should change!" ) {
	HPGE::get_object_position(obj, pos);

	REQUIRE( pos [0] == newpos [0]);
	REQUIRE( pos [1] == newpos [1]);
	REQUIRE( pos [2] == newpos [2]);
      }

      // Place it back to the origin
      HPGE::set_object_position(obj, origin);
    }

    WHEN( "We change the coordinate system AND the object position" ) {
      REQUIRE( HPGE::set_world_mirror(2, 3, -1) == HPGE::SUCCESS);
      REQUIRE( HPGE::set_object_position(obj, newpos) == HPGE::SUCCESS);

      THEN( "The object position should not change!" ) {
	HPGE::get_object_position(obj, pos);

	REQUIRE( pos [0] == newpos [0] );
	REQUIRE( pos [1] == newpos [1] );
	REQUIRE( pos [2] == newpos [2] );
      }
      REQUIRE( HPGE::set_world_mirror(1, 2, 3) == HPGE::SUCCESS);
    }

    WHEN ( "We check the rotation " ) {
      HPGE::get_object_rotation (obj, rot);

      REQUIRE( rot [0] == 1 );
      REQUIRE( rot [1] == 0 );
      REQUIRE( rot [2] == 0 );
      REQUIRE( rot [3] == 0 );
    }

    WHEN( "We change the object rotation" ) {
      REQUIRE( HPGE::set_object_rotation (obj, newrot) == HPGE::SUCCESS );

      THEN( "The object is rotated") {
	REQUIRE( HPGE::get_object_rotation (obj, rot) == HPGE::SUCCESS );

	REQUIRE( rot [0] == Approx( newrot [0] ) );
	REQUIRE( rot [1] == Approx( newrot [1] ) );
	REQUIRE( rot [2] == Approx( newrot [2] ) );
	REQUIRE( rot [3] == Approx( newrot [3] ) );
      }
    }

    // WHEN( "We change the workspace rotation" ) {
    //   HPGE::set_world_rotation_eulerXYZ (0, 180, 0);

    //   THEN( "The object rotation does not change rotated") {
    //	double oldrot [4] {rot [0], rot [1], rot [2], rot [3]};
    //	// HPGE::set_object_rotation (obj, newrot);
    //	HPGE::get_object_rotation (obj, rot);
    //	CAPTURE( oldrot );
    //	double tmprot [4];
    //	HPGE::get_world_rotation (tmprot);
    //	CAPTURE( tmprot );

    //	CAPTURE( rot );
    //	CAPTURE( newrot );

    //	REQUIRE( rot [0] == Approx (newrot [0]));
    //	REQUIRE( rot [1] == Approx (newrot [1]));
    //	REQUIRE( rot [2] == Approx (newrot [2]));
    //	REQUIRE( rot [3] == Approx (newrot [3]));
    //   }
    // }
  }
  REQUIRE( HPGE::deinitialize() == HPGE::SUCCESS );
}

SCENARIO( "Compare meshes and specific implementations", "[objects]" ) {
  REQUIRE( HPGE::initialize (mockDevId, 10.0, 1.0) == HPGE::SUCCESS );
  GIVEN( "A mesh cube in the world" ) {
    int obj = createCube ();
    REQUIRE_FALSE( obj == -1 );
    // status =
    HPGE::add_object_to_world (obj);

    // CAPTURE( status );

    double pos [3];
    double rot [4];
    HPGE::get_object_position (obj, pos);

    double newpos [3]{10, 20, 30};
    double newrot [4]{0.0, 0.0, 1.0, 0};

    REQUIRE( pos [0] == 0 );
    REQUIRE( pos [1] == 0 );
    REQUIRE( pos [2] == 0 );
  }
  REQUIRE( HPGE::deinitialize() == HPGE::SUCCESS );
}

SCENARIO( "Test concurrency on adding and removing objects", "[objects]" ) {
  REQUIRE( HPGE::initialize (mockDevId, 10.0, 1.0) == HPGE::SUCCESS );
  GIVEN( "A running loop" ) {
    REQUIRE( HPGE::start () == HPGE::SUCCESS );
    const int created_objects = 100;

    WHEN( "When we create multiple objects in a loop" ) {
      for (int i = 0; i < created_objects; ++i) {
	int obj = createCube ();
	REQUIRE_FALSE( obj == -1 );
	HPGE::add_object_to_world (obj);
	std::this_thread::sleep_for (std::chrono::milliseconds (3));
      }
      THEN( "The program does not segfault and all objects are created" ) {
	REQUIRE( HPGE::debug::getCreatedObjectsNumber () ==
		 created_objects );
      }
    }

    WHEN( "WHEN WE CREATE MULTIPLE OBJECTS AND DISABLE/ENABLE THEM" ) {
      std::vector<int> objects { };
      bool disabling = true;
      for (int i = 0; i < created_objects; ++i) {
	int obj = createCube ();
	REQUIRE_FALSE( obj == -1 );
	objects.push_back (obj);
	REQUIRE( HPGE::add_object_to_world (obj) == HPGE::SUCCESS );
	std::this_thread::sleep_for (std::chrono::milliseconds (3));

	// Toggle all objects on and off
	for (int obj : objects) {
	  if (disabling) {
	    REQUIRE( HPGE::disable_object (obj) == HPGE::SUCCESS );
	    disabling = false;
	  } else {
	    REQUIRE( HPGE::enable_object (obj) == HPGE::SUCCESS );
	    disabling = true;
	  }
	}
      }

      THEN( "The program does not segfault and all objects are created" ) {
	REQUIRE( HPGE::debug::getCreatedObjectsNumber () ==
		 created_objects );
      }
    }
  }
  REQUIRE( HPGE::deinitialize() == HPGE::SUCCESS );
}
