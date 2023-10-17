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

int createCube () {
  double position[3]{0.0, 0.0, 0.0};
  double scale[3]{3.0, 3.0, 3.0};
  double rotation[4]{1.0, 0.0, 0.0, 0.0};
  return HPGE::create_box_object (scale, position, rotation);
}
