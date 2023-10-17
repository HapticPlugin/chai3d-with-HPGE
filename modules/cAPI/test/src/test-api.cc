#include <chrono>
#include <thread>

#include <iostream>
#include "HPGE.h"

int main (int argc, char *argv[]) {
  std::cout << "Starting\n";
  int init_status = HPGE::initialize (-1, 1.0, 0.01);
  assert (init_status == HPGE::SUCCESS);
  std::cout << "initialized\n";

  assert (HPGE::start () == HPGE::SUCCESS );
  // std::cout << "Started\n";
  // std::cout << "Long (start) wait started\n";
  // std::this_thread::sleep_for (std::chrono::milliseconds (1000));
  // std::cout << "Long (start) wait ended\n";

  double normals [] [3] {
    {-1, 0, 0},
    {1, 0, 0},
    {-1, 0, 0},
    {1, 0, 0},
    {-1, 0, 0},
    {1, 0, 0},
    {-1, 0, 0},
    {1, 0, 0}
  };
  double vertPos [] [3] {
    {-0.5, -0.5, 0.5},
    {0.5, -0.5, 0.5},
    {-0.5, 0.5, 0.5},
    {0.5, 0.5, 0.5},
    {-0.5, 0.5, -0.5},
    {0.5, 0.5, -0.5},
    {-0.5, -0.5, -0.5},
    {0.5, -0.5, -0.5}
  };
  double uvs [] [2] {
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0}
  };
  int triPos [] [3] {
    {0, 1, 2},
    {2, 1, 3},
    {2, 3, 4},
    {4, 3, 5},
    {4, 5, 6},
    {6, 5, 7},
    {6, 7, 0},
    {0, 7, 1},
    {1, 7, 3},
    {3, 7, 5},
    {6, 0, 4},
    {4, 0, 2}
  };

  int vertNum = 8;
  int triNum = 12;
  int uvNum = 8;

  double position [3] {0.0, 0.0, 0.0};
  double scale [3] {3.0, 3.0, 3.0};
  double rotation [4] {1.0, 0.0, 0.0, 0.0};

  int obj = HPGE::create_mesh_object (position, scale, rotation,
				      vertPos, normals,
				      vertNum,
				      triPos, triNum, uvNum, uvs);

  std::cout << "Long wait started\n";
  std::this_thread::sleep_for (std::chrono::milliseconds (3));
  std::cout << "Long wait ended\n";

  std::cout << "Long wait started\n";
  std::this_thread::sleep_for (std::chrono::milliseconds (3));
  std::cout << "Long wait ended\n";

  assert(obj != -1);

  int exists = HPGE::object_exists (obj);
  std::cout << "object id: " << obj << "\n";
  std::cout << "exists error code: " << exists << "\n";
  assert(exists == HPGE::SUCCESS);

  std::cout << "added\n";
  HPGE::add_object_to_world (obj);
  std::cout << "added to world\n";

  int object_props_set = HPGE::set_object_material
    (obj,
     0.0,
     0, // 0 = don't use
     0.0,
     0.0,
     0.0,
     0.0,
     1.0, // viscosity
     0.0,
     0.0,
     0.0,
     0.0, // vibration freq
     0.0); // vibration int

  auto name = HPGE::debug::getObjectTag (obj);
  std::cout << name << "\n";
  assert (name != "example cube");
  int tag_set = HPGE::set_object_tag (obj, "example cube");
  assert (tag_set == HPGE::SUCCESS );
  assert (HPGE::debug::getObjectTag (obj) == "example cube");

  std::cout << "init: " << init_status << "\n";
  std::cout << "wait\n";

  std::this_thread::sleep_for (std::chrono::milliseconds (3));

  std::cout << "set\n";
  object_props_set = HPGE::set_object_material
    (obj,
     0.0,
     1, // 0 = don't use
     1.0,
     1.0,
     1.0,
     1.0,
     0.0, // viscosity
     1.0,
     1.0,
     1.0,
     1.0, // vibration freq
     1.0); // vibration int

  assert (object_props_set == HPGE::SUCCESS);
  std::cout << "wait\n";
  std::this_thread::sleep_for (std::chrono::milliseconds (10));
  std::cout << "Done!\n";

  // Dirty stop
  // HPGE::stop ();
  // HPGE::deinitialize ();
  return 0;
}
