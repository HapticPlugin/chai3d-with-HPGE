#include "../include/chai3d.h"
#include <iostream>

const double toolRadius = 0.01;
const double hapticScale = 1.0;
const int deviceId = 1;

int main () {
  chai3d::cWorld world = chai3d::cWorld ();
  // a haptic device handler
  chai3d::cHapticDeviceHandler * handler; // init'd in initialize()
  chai3d::cGenericHapticDevicePtr hapticDevice; // init'd in initialize()

  // a virtual tool representing the haptic device in the scene
  chai3d::cToolCursor * tool; // init'd in initialize()
  // get spec of haptic device
  chai3d::cHapticDeviceInfo hapticDeviceInfo;

  // create a haptic device handler
  handler = new chai3d::cHapticDeviceHandler ();
  bool res = handler -> getDevice (hapticDevice, deviceId);

  // retrieve information about the current haptic device
  hapticDeviceInfo = hapticDevice -> getSpecifications ();
  std::cout << "Using haptic device: " << hapticDeviceInfo.m_modelName << std::endl;

  // create a 3D tool and add it to the world
  tool = new chai3d::cToolCursor (&world);

  world.addChild (tool);

  // connect the haptic device to the tool
  tool -> setHapticDevice (hapticDevice);

  tool -> setRadius (toolRadius);

  // enable if objects in the scene are going to rotate of translate
  // or possibly collide against the tool. If the environment
  // is entirely static, you can set this parameter to "false"
  tool -> enableDynamicObjects (true); // FIXME: add as a config option

  // map the physical workspace of the haptic device to a larger virtual workspace.
  tool -> setWorkspaceRadius (hapticScale);

  // haptic forces are enabled only if small forces are first sent to the device;
  // this mode avoids the force spike that occurs when the application starts when
  // the tool is located inside an object for instance.
  tool -> setWaitForSmallForce (true);

  // chai3d::cMultiMesh* object = new chai3d::cMultiMesh();
  // if (object -> loadFromFile("test.obj")) {
  //   std::cout << "Load successful!\n";
  // } else {
  //   std::cerr << "Could not load test.obj\n";
  // }


#define SPHERE
#ifdef SPHERE
  chai3d::cShapeSphere* object = new chai3d::cShapeSphere(0.3);
#else
  double normals[][3] = {
    {-1, 0, 0},
    {1, 0, 0},
    {-1, 0, 0},
    {1, 0, 0},
    {-1, 0, 0},
    {1, 0, 0},
    {-1, 0, 0},
    {1, 0, 0}
  };
  double vertPos[][3] = {
    {-0.5, -0.5, 0.5},
    {0.5, -0.5, 0.5},
    {-0.5, 0.5, 0.5},
    {0.5, 0.5, 0.5},
    {-0.5, 0.5, -0.5},
    {0.5, 0.5, -0.5},
    {-0.5, -0.5, -0.5},
    {0.5, -0.5, -0.5}
  };
  double uvs[][2] = {
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0}
  };
  int triPos[][3] = {
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

  chai3d::cMesh* object = new chai3d::cMesh();


  for (int i = 0; i < vertNum; ++i) {
    std::cout << vertNum << std::endl;
    int vertex = object -> newVertex ();

    chai3d::cVector3d vertPosVector3 = chai3d::cVector3d(vertPos[i][0],
    							 vertPos[i][1],
    							 vertPos[i][2]);
    chai3d::cVector3d vertNormalVecotor3 =
      chai3d::cVector3d(normals [i][0],
    			normals [i][1],
    			normals [i][2]);

    std::cout << "normals["<<i+1<<"] = [" << vertNormalVecotor3 << "]\n";
    std::cout << "vertpos["<<i+1<<"] = [" << vertPosVector3 << "]\n";
    std::cout << "uvs["<<i+1<<"] = [" << uvs[i][0] << ", " << uvs[i][1] << "]\n";
    object -> m_vertices -> setLocalPos (vertex, vertPosVector3);
    object -> m_vertices -> setNormal (vertex, vertNormalVecotor3);

    // Texture
    object -> m_vertices -> setTexCoord (vertex, uvs [i] [0], uvs [i] [1]);
  }

      for (int i = 0; i < triNum; ++i) {
	std::cout << "triangles[" << i+1 << "] = [" <<
	  chai3d::cVector3d(triPos [i][0],
			    triPos [i][1],
			    triPos [i][2]) << "]\n";
	object -> newTriangle (triPos [i] [0], triPos [i] [1], triPos [i] [2]);
      }

    object -> scaleXYZ (1.0, 1.0, 1.0);

    object -> computeBoundaryBox (true);
    object -> createAABBCollisionDetector (toolRadius);

    // object -> computeAllNormals ();


#endif
    object -> setLocalPos (chai3d::cVector3d(0.0, 0.0, 0.0));

    // object -> setStiffness(10.0);

    object -> createEffectVibration ();
    object -> m_material -> setVibrationFrequency (10.0);
    object -> m_material -> setVibrationAmplitude (2.0);
    object -> deleteEffectSurface ();

    world.addChild (object);

    // start the haptic tool
    tool -> start ();

    std::cout << "running!\n";
    // main haptic simulation loop
    for (int i = 0; i < 1000; ++i) {
      // update position and orientation of tool
      tool -> updateFromDevice ();

      world.computeGlobalPositions (true);
      tool -> computeInteractionForces ();

      // send forces to haptic device
      tool -> applyToDevice ();

      auto pos = tool -> m_hapticPoint -> getGlobalPosGoal ();
      // chai3d::cVector3d();
      // hapticDevice -> getPosition (pos);
      auto prox = tool -> m_hapticPoint -> getGlobalPosProxy ();
      if (pos.x() != prox.x() ||
	  pos.y() != prox.y() ||
	  pos.z() != prox.z()) {
	std::cout << pos << " -- " << prox << "\n";
	std::cout << "Different!\n";
      }
    }
    std::cout << "end!\n";
    tool -> stop ();

  return 0;
}
