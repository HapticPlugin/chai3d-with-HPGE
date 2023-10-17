#include <chrono>
#include <thread>

#include <iostream>
#include "HPGE.h"

// Abusing the hook system to add a custom sleep timer
int nanosecond_sleep = 0;

void sleepHook (const double position [3],
		const double velocity [3],
		double force [3]) {
  std::this_thread::sleep_for (std::chrono::nanoseconds (nanosecond_sleep));
  force [0] = 0.0;
  force [1] = 0.0;
  force [2] = 0.0;
}

int main (int argc, char *argv[]) {
  int init_status = HPGE::initialize (-1, 1.0, 0.01);
  assert (init_status == HPGE::SUCCESS);
  assert (HPGE::start () == HPGE::SUCCESS );

  HPGE::set_hook (0, sleepHook);

  // must be more than 1 second or frequency counter does not count it!
  std::this_thread::sleep_for (std::chrono::milliseconds (1100));
  double full_freq = HPGE::get_loop_frequency ();

  nanosecond_sleep = 100;
  std::this_thread::sleep_for (std::chrono::milliseconds (1100));
  double slower_freq = HPGE::get_loop_frequency ();

  nanosecond_sleep = 100000;
  std::this_thread::sleep_for (std::chrono::milliseconds (1100));
  double slowest_freq = HPGE::get_loop_frequency ();

  assert ((full_freq > slower_freq) && (slower_freq > slowest_freq));
  std::cout << full_freq << "\n";
  HPGE::deinitialize ();
  return 0;
}
