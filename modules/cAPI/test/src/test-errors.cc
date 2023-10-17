#include "catch.hpp"
#include "HPGE.h"
#include "chai3d.h"

SCENARIO( "An error code is set", "" ) {
  const int buffer_big = 100;
  const int buffer_small = 0;
  char message_big [buffer_big];
  char message_big_last [buffer_big];
  char message_small [1]; // VS does not allow 0-sized arrays

  // TODO: Don't know of a way to "iterate" an enum
  for (int err = -4; err < 26; ++err) {
    int res = HPGE::retErr (static_cast<HPGE::ErrorMsg> (err));
    REQUIRE( res == err );

    int got_last_error = HPGE::last_error_msg (buffer_big, message_big_last);
    int got_error = HPGE::get_error_msg (res, buffer_big, message_big);

    bool succ = (got_error == HPGE::SUCCESS);//  ||
		 // got_error == HPGE::INVALID_ERROR_NUM );
    CAPTURE( err );
    CAPTURE( got_error );
    REQUIRE( succ );
    REQUIRE( got_error == got_last_error );
    REQUIRE( std::string(message_big_last) == std::string(message_big) );
  }

  for (int err = -100; err < 100; ++err) {
    int res = HPGE::retErr (static_cast<HPGE::ErrorMsg> (err));
    REQUIRE( res == err );

    int got_last_error = HPGE::last_error_msg (buffer_big, message_big_last);
    CAPTURE( got_last_error );
    CAPTURE( res );
    int got_error = HPGE::get_error_msg (res, buffer_big, message_big);
    bool succ = (got_error == HPGE::SUCCESS ||
		 got_error == HPGE::INVALID_ERROR_NUM );
    REQUIRE( succ );
    REQUIRE( got_error == got_last_error );
    REQUIRE( std::string(message_big_last) == std::string(message_big) );
  }
    // TODO: Don't know of a way to "iterate" an enum
  for (int err = -4; err < 26; ++err) {
    int res = HPGE::retErr (static_cast<HPGE::ErrorMsg> (err));
    REQUIRE( res == err );

    int got_error = HPGE::get_error_msg (res, buffer_small, message_small);
    bool succ = (got_error == HPGE::BUFFER_TOO_SMALL);//  ||
		 // got_error == HPGE::INVALID_ERROR_NUM );
    REQUIRE( succ );
  }

  for (int err = -100; err < 100; ++err) {
    int res = HPGE::retErr (static_cast<HPGE::ErrorMsg> (err));
    REQUIRE( res == err );

    int got_error = HPGE::get_error_msg (res, buffer_small, message_small);
    bool succ = (got_error == HPGE::BUFFER_TOO_SMALL ||
		 got_error == HPGE::INVALID_ERROR_NUM );
    REQUIRE( succ );
  }
}
