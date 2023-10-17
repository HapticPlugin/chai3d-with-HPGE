#include "catch.hpp"
#include "HPGE.h"
#include "chai3d.h"

SCENARIO( "Check bitmap values conversions", "[textures, conversions]" ) {
  signed char c;
  WHEN( "We check a negative value" ) {
    c = HPGE::floatToByte (-10000.0f);
    THEN( "The char must be -128" ) {
      REQUIRE( (short)c == -128 );
    }
  }
  WHEN( "We give 0" ) {
    c = HPGE::floatToByte (0.0f);
    THEN( "The char must be -128" ) {
      REQUIRE( (short)c == -128 );
    }
  }

  WHEN( "We give 0.5" ) {
    c = HPGE::floatToByte (0.5f);
    THEN( "The char must be 0" ) {
      REQUIRE( (short)c == 0 );
    }
  }

  WHEN( "We give a 1.0f" ) {
    c = HPGE::floatToByte (1.0f);
    THEN( "The char must be the maximum (127)" ) {
      REQUIRE( (short)c == 127 );
    }
  }
    WHEN( "We give a big value 1000.0f" ) {
    c = HPGE::floatToByte (1000.0f);
    THEN( "The char must be the maximum (127)" ) {
      REQUIRE( (short)c == 127 );
    }
  }
}
