#include "catch.hpp"
#include "HPGE.h"
#include "chai3d.h"

SCENARIO( "First we check that basic functions are working" ) {
  double w = 18.0;
  double x = 17.0;
  double y = 16.0;
  double z = 15.0;
  auto q = chai3d::cQuaternion(w, x, y, z);
  REQUIRE( HPGE::get(q, 0) == w );
  REQUIRE( HPGE::get(q, 1) == x );
  REQUIRE( HPGE::get(q, 2) == y );
  REQUIRE( HPGE::get(q, 3) == z );
}

void tochai(const double* input, double* output) {
  auto res = HPGE::PosToChai (input);
  output [0] = res.x ();
  output [1] = res.y ();
  output [2] = res.z ();
}

void compareD3(const double * a, const double * b) {
  REQUIRE( a [0] == Approx (b [0]) );
  REQUIRE( a [1] == Approx (b [1]) );
  REQUIRE( a [2] == Approx (b [2]) );
}

#define BACKAGAIN()					\
  THEN( "Transforming back keep the same value" ) {	\
    HPGE::PosFromChai(out, out);			\
    compareD3 (point, out);				\
  }

// #define BACKAGAIN() REQUIRE(true)

SCENARIO( "Conversions" ) {
  GIVEN( "A point to transform") {
    double point [3] {100.0, 20.0, 3.0};
    double out [3];
    WHEN( "No mirror and nothing else" ) {
      tochai (point, out);
      THEN( "The transformed point is the same as the input" ) {
	compareD3 (point, out);
      }
      BACKAGAIN();
    }

    WHEN( "We set the unity mirroring" ) {
      HPGE::set_world_mirror (2, 3, -1);
      tochai (point, out);
      THEN( "The transformation is correct" ) {
	double expected [3]{-3, 100, 20};
	compareD3 (expected, out);
      }
      BACKAGAIN();
    }

    WHEN( "Scale change only" ) {
      HPGE::set_world_mirror (1, 2, 3);
      HPGE::set_world_scale (0.1, 10, 100);
      tochai (point, out);
      THEN( "The transformation is correct" ) {
	double expected [3]{10, 200, 300};
	compareD3 (expected, out);
      }
      BACKAGAIN();
    }

    WHEN( "unity mirroring + scale change" ) {
      HPGE::set_world_mirror (2, 3, -1);
      HPGE::set_world_scale (0.1, 10, 100);
      tochai (point, out);
      THEN( "The transformation is correct" ) {
	double expected [3]{-300, 10, 200};
	compareD3 (expected, out);
      }
      BACKAGAIN();
    }

    WHEN( "Translation only" ) {
      HPGE::set_world_mirror (1, 2, 3);
      HPGE::set_world_scale (1.0, 1.0, 1.0);
      HPGE::set_world_translation (4.0, 5.0, 6.0);
      tochai (point, out);
      THEN( "The transformation is correct" ) {
	double expected [3]{104, 25, 9};
	compareD3 (expected, out);
      }
      BACKAGAIN();
    }

    WHEN( "Translation + Scale" ) {
      HPGE::set_world_mirror (1, 2, 3);
      HPGE::set_world_scale (0.1, 10, 100);
      HPGE::set_world_translation (4.0, 5.0, 6.0);
      tochai (point, out);
      THEN( "The transformation is correct" ) {
	double expected [3]{14, 205, 306};
	compareD3 (expected, out);
      }
      BACKAGAIN();
    }

    WHEN( "Translation + Mirror" ) {
      HPGE::set_world_mirror (2, 3, -1);
      HPGE::set_world_scale (1, 1, 1);
      HPGE::set_world_translation (4.0, 5.0, 6.0);
      tochai (point, out);
      THEN( "The transformation is correct" ) {
	double expected [3]{
	  -1 * 3 + 4,
	    100 + 5,
	    20 + 6};
	compareD3 (expected, out);
      }
      BACKAGAIN();
    }

    WHEN( "Translation + Mirror + Scale" ) {
      HPGE::set_world_mirror (2, 3, -1);
      HPGE::set_world_scale (0.1, 10, 100);
      HPGE::set_world_translation (4.0, 5.0, 6.0);
      tochai (point, out);
      THEN( "The transformation is correct" ) {
	double expected [3] {
	  (-1 * 3) * 100 + 4,
	    100 * 0.1 + 5,
	    20 * 10 + 6
	    };
	compareD3 (expected, out);
      }
      BACKAGAIN();
    }

    WHEN( "Rotation only - 180 X" ) {
      HPGE::set_world_mirror (1, 2, 3);
      HPGE::set_world_scale (1.0, 1.0, 1.0);
      HPGE::set_world_translation (0, 0, 0);
      HPGE::set_world_rotation_eulerXYZ (180, 0, 0);

      tochai (point, out);
      THEN( "The transformation is correct" ) {
	double expected [3]{100, -20.0, -3};
	compareD3 (expected, out);
      }
      BACKAGAIN();
    }

    WHEN( "Rotation only - 180 Y" ) {
      HPGE::set_world_mirror (1, 2, 3);
      HPGE::set_world_scale (1.0, 1.0, 1.0);
      HPGE::set_world_translation (0, 0, 0);
      HPGE::set_world_rotation_eulerXYZ (0, 180, 0);

      tochai (point, out);
      THEN( "The transformation is correct" ) {
	double expected [3]{-100, 20.0, -3};
	compareD3 (expected, out);
      }
      BACKAGAIN();
    }

    WHEN( "Rotation only - 180 Z" ) {
      HPGE::set_world_mirror (1, 2, 3);
      HPGE::set_world_scale (1.0, 1.0, 1.0);
      HPGE::set_world_translation (0, 0, 0);
      HPGE::set_world_rotation_eulerXYZ (0, 0, 180);

      tochai (point, out);
      THEN( "The transformation is correct" ) {
	double expected [3]{-100, -20.0, 3};
	compareD3 (expected, out);
      }
      BACKAGAIN();
    }

    WHEN( "Rotation only - 90 X" ) {
      HPGE::set_world_mirror (1, 2, 3);
      HPGE::set_world_scale (1.0, 1.0, 1.0);
      HPGE::set_world_translation (0, 0, 0);
      HPGE::set_world_rotation_eulerXYZ (90, 0, 0);

      tochai (point, out);
      THEN( "The transformation is correct" ) {
	double expected [3]{100, 3, -20};
	compareD3 (expected, out);
      }
      BACKAGAIN();
    }

    WHEN( "Rotation only - 90 Y" ) {
      HPGE::set_world_mirror (1, 2, 3);
      HPGE::set_world_scale (1.0, 1.0, 1.0);
      HPGE::set_world_translation (0, 0, 0);
      HPGE::set_world_rotation_eulerXYZ (0, 90, 0);

      tochai (point, out);
      THEN( "The transformation is correct" ) {
	double expected [3]{-3, 20.0, 100};
	compareD3 (expected, out);
      }
      BACKAGAIN();
    }

    WHEN( "Rotation only - 90 Z" ) {
      HPGE::set_world_mirror (1, 2, 3);
      HPGE::set_world_scale (1.0, 1.0, 1.0);
      HPGE::set_world_translation (0, 0, 0);
      HPGE::set_world_rotation_eulerXYZ (0, 0, 90);

      tochai (point, out);
      THEN( "The transformation is correct" ) {
	double expected [3]{20, -100.0, 3};
	compareD3 (expected, out);
      }
      BACKAGAIN();
    }

    WHEN( "Rotation (180-X) + Mirroring" ) {
      HPGE::set_world_mirror (2, 3, -1);
      HPGE::set_world_rotation_eulerXYZ (180, 0, 0);
      tochai (point, out);
      THEN( "The transformation is correct" ) {
	double expected [3]{3, 100, -20};
	compareD3 (expected, out);
      }
      BACKAGAIN();
    }

    WHEN( "Rotation (180-Y) + Mirroring" ) {
      HPGE::set_world_mirror (2, 3, -1);
      HPGE::set_world_rotation_eulerXYZ (0, 180, 0);
      tochai (point, out);
      THEN( "The transformation is correct" ) {
	double expected [3]{3, -100, 20};
	compareD3 (expected, out);
      }
      BACKAGAIN();
    }

    WHEN( "Rotation (180-Z) + Mirroring" ) {
      HPGE::set_world_mirror (2, 3, -1);
      HPGE::set_world_rotation_eulerXYZ (0, 0, 180);
      tochai (point, out);
      THEN( "The transformation is correct" ) {
	double expected [3]{-3, -100, -20};
	compareD3 (expected, out);
      }
      BACKAGAIN();
    }
  }
}

// SCENARIO( "Ex" ) {
//   GIVEN( "E" ) {
//     double point [3] {100.0, 20.0, 3.0};
//     WHEN( "Mirror only" ) {
//       HPGE::set_world_mirror (2, 3, -1);
//       HPGE::set_world_rotation_quaternion (1.0, 0.0, 0.0, 0.0); // no rotation

//       // double m [3];
//       // HPGE::get_world_mirror (m);
//       // CAPTURE( m );
//       // HPGE::get_world_orderp (m);
//       // CAPTURE( m );


//       THEN( "The point coordinates are changed accordingly" ) {
//	auto res = HPGE::PosToChai (point);

//	CAPTURE( point );
//	CAPTURE( res );
//	// the z is mirrored
//	REQUIRE(   res.y () == Approx (point [0]) );
//	REQUIRE(   res.z () == Approx (point [1]) );
//	REQUIRE( - res.x () == Approx (point [2]) );

//	REQUIRE( res.x () == -3.0);
//	REQUIRE( res.y () == 100.0);
//	REQUIRE( res.z () == 20.0);

//	// // double tmp[3]{res[0], res[1], res[2]};
//	// auto tmp = chai3d::cVector3d (res.x (), res.y (), res.z ());
//	// auto back = HPGE::PosFromChai(tmp);
//	// REQUIRE( back [0] == Approx (point [0]) );
//	// REQUIRE( back [1] == Approx (point [1]) );
//	// REQUIRE( back [2] == Approx (point [2]) );
//       }
//     }

//     WHEN( "mirror + Scale" ) {
//       HPGE::set_world_mirror (2, 3, -1);
//       // HPGE::set_world_rotation_quaternion (0.0, 1.0, 0.0, 0.0);
//       HPGE::set_world_rotation_quaternion (1.0, 0.0, 0.0, 0.0);
//       HPGE::set_world_scale (10.0, 20.0, 30.0);

//       // double m [3];
//       // HPGE::get_world_mirror (m);
//       // CAPTURE( m );
//       // HPGE::get_world_orderp (m);
//       // CAPTURE( m );


//       THEN( "The point coordinates are changed accordingly" ) {
//	auto res = HPGE::PosToChai (point);

//	CAPTURE( point );
//	CAPTURE( res );
//	// the z is mirrored
//	REQUIRE(   res.y () == Approx (point [0] * 10.0) );
//	REQUIRE(   res.z () == Approx (point [1] * 20.0) );
//	REQUIRE( - res.x () == Approx (point [2] * 30.0) );

//	// // double tmp[3]{res[0], res[1], res[2]};
//	// auto tmp = chai3d::cVector3d (res.x (), res.y (), res.z ());
//	// auto back = HPGE::PosFromChai(tmp);
//	// REQUIRE( back [0] == Approx (point [0]) );
//	// REQUIRE( back [1] == Approx (point [1]) );
//	// REQUIRE( back [2] == Approx (point [2]) );
//       }
//     }

//     WHEN( "mirror + Scale" ) {
//       HPGE::set_world_mirror (2, 3, -1);
//       // HPGE::set_world_rotation_quaternion (0.0, 1.0, 0.0, 0.0);
//       HPGE::set_world_rotation_quaternion (1.0, 0.0, 0.0, 0.0);
//       HPGE::set_world_scale (1.0, 1.0, 1.0);
//       HPGE::set_world_translation (100.0, 10.0, 1000.0);

//       THEN( "The point coordinates are changed accordingly" ) {
//	auto res = HPGE::PosToChai (point);

//	CAPTURE( point );
//	CAPTURE( res );
//	// the z is mirrored
//	REQUIRE(   res.y () == Approx (point [0]  + 100.0  ) );
//	REQUIRE(   res.z () == Approx (point [1]  + 10.0   ) );
//	REQUIRE( - res.x () == Approx (point [2]  + 1000.0 ) );

//	// // double tmp[3]{res[0], res[1], res[2]};
//	// auto tmp = chai3d::cVector3d (res.x (), res.y (), res.z ());
//	// auto back = HPGE::PosFromChai(tmp);
//	// REQUIRE( back [0] == Approx (point [0]) );
//	// REQUIRE( back [1] == Approx (point [1]) );
//	// REQUIRE( back [2] == Approx (point [2]) );
//       }
//     }

//     WHEN( "mirror + Rotation" ) {
//       HPGE::set_world_mirror (2, 3, -1);
//       // HPGE::set_world_rotation_quaternion (0.0, 1.0, 0.0, 0.0);
//       HPGE::set_world_rotation_eulerXYZ (180.0, 0.0, 0.0);
//       HPGE::set_world_scale (1.0, 1.0, 1.0);
//       HPGE::set_world_translation (0.0, 0.0, 0.0);

//       THEN( "The point coordinates are changed accordingly" ) {
//	auto res = HPGE::PosToChai (point);

//	CAPTURE( point );
//	CAPTURE( res );
//	// the z is mirrored
//	REQUIRE(   3.0   == Approx (res.x ()) );
//	REQUIRE(   100.0 == Approx (res.y ()) );
//	REQUIRE( - 20.0  == Approx (res.z ()) );

//	// REQUIRE(   res.y () == Approx (point [0]) );
//	// REQUIRE( - res.x () == Approx (point [1]) );
//	// REQUIRE( - res.z () == Approx (point [2]) );

//	// // double tmp[3]{res[0], res[1], res[2]};
//	// auto tmp = chai3d::cVector3d (res.x (), res.y (), res.z ());
//	// auto back = HPGE::PosFromChai(tmp);
//	// CAPTURE( back );
//	// REQUIRE( back [0] == Approx (point [0]) );
//	// REQUIRE( back [1] == Approx (point [1]) );
//	// REQUIRE( back [2] == Approx (point [2]) );
//       }
//     }
//     // // WHEN( "mirror + Rotation" ) {
//     // //   HPGE::set_world_mirror (2, 3, -1);
//     // //   // HPGE::set_world_rotation_quaternion (0.0, 1.0, 0.0, 0.0);
//     // //   HPGE::set_world_rotation_eulerXYZ (0.0, 180.0, 0.0);
//     // //   HPGE::set_world_scale (1.0, 1.0, 1.0);
//     // //   HPGE::set_world_translation (0.0, 0.0, 0.0);

//     // //   THEN( "The point coordinates are changed accordingly" ) {
//     // //	auto res = HPGE::PosToChai (point);

//     // //	CAPTURE( point );
//     // //	CAPTURE( res );
//     // //	// the z is mirrored
//     // //	REQUIRE(   res.y () == Approx (point [0]) );
//     // //	REQUIRE( - res.x () == Approx (point [1]) );
//     // //	REQUIRE( - res.z () == Approx (point [2]) );

//     // //	// double tmp[3]{res[0], res[1], res[2]};
//     // //	auto tmp = chai3d::cVector3d (res.x (), res.y (), res.z ());
//     // //	auto back = HPGE::PosFromChai(tmp);
//     // //	CAPTURE( back );
//     // //	REQUIRE( back [0] == Approx (point [0]) );
//     // //	REQUIRE( back [1] == Approx (point [1]) );
//     // //	REQUIRE( back [2] == Approx (point [2]) );
//     // //   }
//     // // }
//   }
// }

// SCENARIO( "A point is moved from Unity to CHAI", "[point, transform]" ) {
//   GIVEN( "A point in Unity coordinates" ) {
//     auto point = chai3d::cVector3d (100.0, 20.0, 3.0);

//     WHEN( "The workspace is mirrored (mirror the z)" ) {
//       HPGE::set_world_mirror (2, 3, -1);
//       HPGE::set_world_rotation_quaternion (1.0, 0.0, 0.0, 0.0); // no rotation

//       THEN( "The point coordinates are changed accordingly" ) {
//	auto res = HPGE::PosFromChai (point);

//	// the z is mirrored
//	REQUIRE( res [0] == Approx (point.z ()) );
//	REQUIRE( res [1] == Approx (point.x ()) );
//	REQUIRE( res [2] == Approx (point.y ()) );

//	double tmp[3]{res[0], res[1], res[2]};
//	chai3d::cVector3d back = HPGE::PosToChai(tmp);
//	REQUIRE( back.x () == Approx (point.x ()) );
//	REQUIRE( back.y () == Approx (point.y ()) );
//	REQUIRE( back.z () == Approx (point.z ()) );
//       }
//       // Restore the state
//       HPGE::set_world_mirror (1, 2, 3);
//       HPGE::set_world_rotation_quaternion (1.0, 0.0, 0.0, 0.0);
//     }

//     WHEN( "The workspace is mirrored (mirror the z)" ) {
//       HPGE::set_world_mirror (1, 2, -3);
//       HPGE::set_world_rotation_quaternion (1.0, 0.0, 0.0, 0.0); // no rotation

//       THEN( "The point coordinates are changed accordingly" ) {
//	auto res = HPGE::PosFromChai(point);

//	REQUIRE( res [0] == Approx (point.x ()) );
//	REQUIRE( res [1] == Approx (point.y ()) );
//	// the z is mirrored
//	REQUIRE( res [2] == Approx (- point.z ()) );

//	double tmp[3]{res[0], res[1], res[2]};
//	chai3d::cVector3d back = HPGE::PosToChai(tmp);
//	REQUIRE( back.x () == Approx (point.x ()) );
//	REQUIRE( back.y () == Approx (point.y ()) );
//	REQUIRE( back.z () == Approx (point.z ()) );
//       }
//       // Restore the state
//       HPGE::set_world_mirror (1, 2, 3);
//       HPGE::set_world_rotation_quaternion (1.0, 0.0, 0.0, 0.0);
//     }

//     WHEN( "The workspace is mirrored (mirror the z) and rotated 180 degree (x)" ) {
//       HPGE::set_world_mirror (1, 2, -3);
//       HPGE::set_world_rotation_eulerXYZ (180.0, 0.0, 0.0);

//       THEN( "The point coordinates are changed accordingly" ) {
//	auto res = HPGE::PosFromChai(point);

//	REQUIRE( res [0] == Approx (point.x ()) );
//	REQUIRE( res [1] == Approx (- point.y ()) );
//	REQUIRE( res [2] == Approx (point.z ()) );

//	double tmp[3]{res[0], res[1], res[2]};
//	chai3d::cVector3d back = HPGE::PosToChai(tmp);
//	REQUIRE( back.x () == Approx (point.x ()) );
//	REQUIRE( back.y () == Approx (point.y ()) );
//	REQUIRE( back.z () == Approx (point.z ()) );
//       }
//       // Restore the state
//       HPGE::set_world_mirror (1, 2, 3);
//       HPGE::set_world_rotation_quaternion (1.0, 0.0, 0.0, 0.0);
//     } // WHEN

//     WHEN( "The workspace is mirrored (mirror the z) and rotated 180 degree (y)" ) {
//       HPGE::set_world_mirror (1, 2, -3);
//       HPGE::set_world_rotation_eulerXYZ (0.0, 180.0, 0.0);

//       THEN( "The point coordinates are changed accordingly" ) {
//	auto res = HPGE::PosFromChai(point);

//	REQUIRE( res [0] == Approx (- point.x ()) );
//	REQUIRE( res [1] == Approx (point.y ()) );
//	REQUIRE( res [2] == Approx (point.z ()) );

//	double tmp[3]{res[0], res[1], res[2]};
//	chai3d::cVector3d back = HPGE::PosToChai(tmp);
//	REQUIRE( back.x () == Approx (point.x ()) );
//	REQUIRE( back.y () == Approx (point.y ()) );
//	REQUIRE( back.z () == Approx (point.z ()) );
//       }
//       // Restore the state
//       HPGE::set_world_rotation_quaternion (1.0, 0.0, 0.0, 0.0);
//       HPGE::set_world_mirror (1, 2, 3);
//     } // WHEN

//     WHEN( "The workspace is mirrored (mirror the z) and rotated 180 degree (z)" ) {
//       HPGE::set_world_mirror (1, 2, -3);
//       HPGE::set_world_rotation_eulerXYZ (0.0, 0.0, 180.0);

//       THEN( "The point coordinates are changed accordingly" ) {
//	auto res = HPGE::PosFromChai(point);

//	REQUIRE( res [0] == Approx (- point.x ()) );
//	REQUIRE( res [1] == Approx (- point.y ()) );
//	REQUIRE( res [2] == Approx (- point.z ()) );
//       }
//       // Restore the state
//       HPGE::set_world_rotation_quaternion (1.0, 0.0, 0.0, 0.0);
//       HPGE::set_world_mirror (1, 2, 3);
//     } // WHEN

//     WHEN( "The workspace is mirrored (z), rotated (x) and rescaled x(3,0.02,0.1)" ) {
//       HPGE::set_world_mirror (1, 2, -3);
//       HPGE::set_world_rotation_eulerXYZ (180.0, 0.0, 0.0);
//       HPGE::set_world_scale (3.0, 0.02, 0.1);

//       THEN( "The point coordinates are changed accordingly" ) {
//	auto res = HPGE::PosFromChai(point);

//	// REQUIRE( res [0] == Approx (point.x () * 3.0) );
//	// REQUIRE( res [1] == Approx (- point.y () * 0.02) );
//	// REQUIRE( res [2] == Approx (point.z () * 0.1) );

//	double tmp[3]{res[0], res[1], res[2]};
//	chai3d::cVector3d back = HPGE::PosToChai(tmp);
//	REQUIRE( back.x () == Approx (point.x ()) );
//	REQUIRE( back.y () == Approx (point.y ()) );
//	REQUIRE( back.z () == Approx (point.z ()) );
//       }
//       // Restore the state
//       HPGE::set_world_rotation_quaternion (1.0, 0.0, 0.0, 0.0);
//       HPGE::set_world_mirror (1, 2, 3);
//       HPGE::set_world_scale (1.0, 1.0, 1.0);
//     } // WHEN

//     WHEN( "The workspace is mirrored (z), rotated (y) and rescaled x(3,0.02,0.1)" ) {
//       HPGE::set_world_mirror (1, 2, -3);
//       HPGE::set_world_rotation_eulerXYZ (0.0, 180.0, 0.0);
//       HPGE::set_world_scale (3.0, 0.02, 0.1);

//       THEN( "The point coordinates are changed accordingly" ) {
//	auto res = HPGE::PosFromChai(point);

//	// REQUIRE( res [0] == Approx (- point.x () * 3.0) );
//	// REQUIRE( res [1] == Approx (point.y () * 0.02) );
//	// REQUIRE( res [2] == Approx (point.z () * 0.1) );

//	double tmp[3]{res[0], res[1], res[2]};
//	chai3d::cVector3d back = HPGE::PosToChai(tmp);
//	REQUIRE( back.x () == Approx (point.x ()) );
//	REQUIRE( back.y () == Approx (point.y ()) );
//	REQUIRE( back.z () == Approx (point.z ()) );
//       }
//       // Restore the state
//       HPGE::set_world_rotation_quaternion (1.0, 0.0, 0.0, 0.0);
//       HPGE::set_world_mirror (1, 2, 3);
//       HPGE::set_world_scale (1.0, 1.0, 1.0);
//     } // WHEN

//     WHEN( "The workspace is mirrored (z), rotated (z) and rescaled x(3,0.02,0.1)" ) {
//       HPGE::set_world_mirror (1, 2, -3);
//       HPGE::set_world_rotation_eulerXYZ (0.0, 0.0, 180.0);
//       HPGE::set_world_scale (3.0, 0.02, 0.1);

//       THEN( "The point coordinates are changed accordingly" ) {
//	auto res = HPGE::PosFromChai(point);

//	// REQUIRE( res [0] == Approx (- point.x () * 3.0) );
//	// REQUIRE( res [1] == Approx (- point.y () * 0.02) );
//	// REQUIRE( res [2] == Approx (- point.z () * 0.1) );

//	double tmp[3]{res[0], res[1], res[2]};
//	chai3d::cVector3d back = HPGE::PosToChai(tmp);
//	REQUIRE( back.x () == Approx (point.x ()) );
//	REQUIRE( back.y () == Approx (point.y ()) );
//	REQUIRE( back.z () == Approx (point.z ()) );
//       }
//       // Restore the state
//       HPGE::set_world_rotation_quaternion (1.0, 0.0, 0.0, 0.0);
//       HPGE::set_world_mirror (1, 2, 3);
//       HPGE::set_world_scale (1.0, 1.0, 1.0);
//     } // WHEN

//     WHEN( "The workspace is mirrored (z), rotated (x), rescaled and translated" ) {
//       HPGE::set_world_mirror (1, 2, -3);
//       HPGE::set_world_rotation_eulerXYZ (180.0, 0.0, 0.0);
//       HPGE::set_world_scale (3.0, 0.02, 0.1);
//       HPGE::set_world_translation (8.0, 9.0, 7.0);

//       THEN( "The point coordinates are changed accordingly" ) {
//	auto res = HPGE::PosFromChai(point);

//	// REQUIRE( res [0] == Approx (point.x () * 3.0    + 8.0) );
//	// REQUIRE( res [1] == Approx (- point.y () * 0.02 + 9.0) );
//	// REQUIRE( res [2] == Approx (point.z () * 0.1    + 7.0) );

//	double tmp[3]{res[0], res[1], res[2]};
//	chai3d::cVector3d back = HPGE::PosToChai(tmp);
//	REQUIRE( back.x () == Approx (point.x ()) );
//	REQUIRE( back.y () == Approx (point.y ()) );
//	REQUIRE( back.z () == Approx (point.z ()) );
//       }
//       // Restore the state
//       HPGE::set_world_rotation_quaternion (1.0, 0.0, 0.0, 0.0);
//       HPGE::set_world_mirror (1, 2, 3);
//       HPGE::set_world_scale (1.0, 1.0, 1.0);
//       HPGE::set_world_translation (0.0, 0.0, 0.0);
//     } // WHEN

//     WHEN( "The workspace is mirrored (z), rotated (y), rescaled and translated" ) {
//       HPGE::set_world_mirror (1, 2, -3);
//       HPGE::set_world_rotation_eulerXYZ (0.0, 180.0, 0.0);
//       HPGE::set_world_scale (3.0, 0.02, 0.1);
//       HPGE::set_world_translation (8.0, 9.0, 7.0);

//       THEN( "The point coordinates are changed accordingly" ) {
//	auto res = HPGE::PosFromChai(point);

//	// REQUIRE( res [0] == Approx (- point.x () * 3.0 + 8.0) );
//	// REQUIRE( res [1] == Approx (point.y () * 0.02  + 9.0) );
//	// REQUIRE( res [2] == Approx (point.z () * 0.1   + 7.0) );

//	double tmp[3]{res[0], res[1], res[2]};
//	chai3d::cVector3d back = HPGE::PosToChai(tmp);
//	REQUIRE( back.x () == Approx (point.x ()) );
//	REQUIRE( back.y () == Approx (point.y ()) );
//	REQUIRE( back.z () == Approx (point.z ()) );
//       }
//       // Restore the state
//       HPGE::set_world_rotation_quaternion (1.0, 0.0, 0.0, 0.0);
//       HPGE::set_world_mirror (1, 2, 3);
//       HPGE::set_world_scale (1.0, 1.0, 1.0);
//       HPGE::set_world_translation (0.0, 0.0, 0.0);
//     } // WHEN

//     WHEN( "The workspace is mirrored (z), rotated (z), rescaled and translated" ) {
//       HPGE::set_world_mirror (1, 2, -3);
//       HPGE::set_world_rotation_eulerXYZ (0.0, 0.0, 180.0);
//       HPGE::set_world_scale (3.0, 0.02, 0.1);
//       HPGE::set_world_translation (8.0, 9.0, 7.0);

//       THEN( "The point coordinates are changed accordingly" ) {
//	auto res = HPGE::PosFromChai(point);

//	// REQUIRE( res [0] == Approx (- point.x () * 3.0  + 8.0) );
//	// REQUIRE( res [1] == Approx (- point.y () * 0.02 + 9.0) );
//	// REQUIRE( res [2] == Approx (- point.z () * 0.1  + 7.0) );

//	double tmp[3]{res[0], res[1], res[2]};
//	chai3d::cVector3d back = HPGE::PosToChai(tmp);
//	REQUIRE( back.x () == Approx (point.x ()) );
//	REQUIRE( back.y () == Approx (point.y ()) );
//	REQUIRE( back.z () == Approx (point.z ()) );
//       }
//       // Restore the state
//       HPGE::set_world_rotation_quaternion (1.0, 0.0, 0.0, 0.0);
//       HPGE::set_world_mirror (1, 2, 3);
//       HPGE::set_world_scale (1.0, 1.0, 1.0);
//       HPGE::set_world_translation (0.0, 0.0, 0.0);
//     } // WHEN
//   }
// } // SCENARIO
