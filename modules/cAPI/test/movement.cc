// This file is part of HPGE, an Haptic Plugin for Game Engines
// -----------------------------------------

// Software License Agreement (BSD License)
// Copyright (c) 2017-2023,
// Copyright © 2017-2019 Gabriel Baud-Bovy (gabriel.baud-bovy <at> iit.it)
// Copyright © 2017-2023 Nicolò Balzarotti (nicolo.balzarotti <at> iit.it)
// Istituto Italiano di Tecnologia (IIT), All rights reserved.
//

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:

// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.

// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.

// 3. Neither the name of HPGE, Istituto Italiano di Tecnologia nor the
// names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <chai3d.h>
#include <HPGE.h>
#include <math.h>

#include <thread>
#include <chrono>


int createCubeMesh () {
  double normals[][3]{
    {-1, 0, 0},
      {1, 0, 0},
	{-1, 0, 0},
	  {1, 0, 0},
	    {-1, 0, 0},
	      {1, 0, 0},
		{-1, 0, 0},
		  {1, 0, 0}
  };
  double vertPos[][3]{
    {-0.5, -0.5, 0.5},
      {0.5, -0.5, 0.5},
	{-0.5, 0.5, 0.5},
	  {0.5, 0.5, 0.5},
	    {-0.5, 0.5, -0.5},
	      {0.5, 0.5, -0.5},
		{-0.5, -0.5, -0.5},
		  {0.5, -0.5, -0.5}
  };
  double uvs[][2]{
    {0.0, 0.0},
      {0.0, 0.0},
	{0.0, 0.0},
	  {0.0, 0.0},
	    {0.0, 0.0},
	      {0.0, 0.0},
		{0.0, 0.0},
		  {0.0, 0.0}
  };
  int triPos[][3]{
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

  double pos[3]{0.0, 0.0, 0.0};
  double scale[3]{3.0, 3.0, 3.0};
  double rotation[4]{1.0, 0.0, 0.0, 0.0};

  return HPGE::create_mesh_object (pos, scale, rotation,
				   vertPos, normals,
				   vertNum,
				   triPos, triNum, uvNum, uvs);
}

int main () {
  HPGE::initialize (0, 10.0, 1.0);
  HPGE::start ();

  int cubeid = createCubeMesh ();
  HPGE::add_object_to_world (cubeid);

  HPGE::set_object_material (cubeid,
				 0.4,
				 1, // 0 = don't use
				 0.0,
				 0.0,
				 0.0,
				 0.0,
				 0.0, // viscosity
				 0.0,
				 0.0,
				 0.0,
				 0.0, // vibration freq
				 0.0); // vibration int

  HPGE::enable_position_interpolation (cubeid);
  HPGE::set_interpolation_period(cubeid, 2);

  double scale [3] = {5.0, 1.0, 5.0};
  HPGE::set_object_scale(cubeid, scale);

  double position [3] = {0.0, 0.0, 0.0};
  // // double diff = 0;
  // double diff = -0.00001;
  int loop = 0;
  const int ms = 20;
  HPGE::start_recording_stats(1);

  for (int i = 0; i < 500; ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    loop += ms;
    position [1] = cos(6.283185307179586 * 0.5 * loop / 1000.0) * 5.0;

    double readpos [3];
    HPGE::get_tool_position (readpos);
    std::cout << readpos[0] << "," << readpos[1] << "," << readpos[2] << " - ";

    HPGE::set_object_position (cubeid, position);
    std::cout << HPGE::loop_frequency () << " - " << position [1] << "\n";
  }
  HPGE::stop_recording_stats();


  return 0;
}
