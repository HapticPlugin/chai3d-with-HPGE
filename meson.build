project('chai3d', 'cpp',
        default_options : ['cpp_std=c++11',
        # 'backend_startup_project=HPGE'
        ],
        version : '3.2.1-small',
        license : 'BSD')

cpp = meson.get_compiler('cpp')
extra_args = [ '-Wall', '-fsanitize=address' ]

if target_machine.system() == 'linux'
  drd = cpp.find_library('drd',
			 dirs : join_paths(meson.current_source_dir(),
					   'dependencies'))
  dl = cpp.find_library('dl')
  usb = cpp.find_library('usb-1.0')
  thread_dep = dependency('threads')
endif

if target_machine.system() == 'linux'
  add_project_arguments('-DLINUX', language : 'cpp')
elif target_machine.system() == 'windows'
  add_project_arguments('-D_CRT_SECURE_NO_DEPRECATE', language : 'cpp')
  add_project_arguments('-DWIN64', language : 'cpp')
elif target_machine.system() == 'macosx'
  message('Platform untested')
  add_project_arguments('-DMACOS', language : 'cpp')
else
  message('Platform not supported')
endif

chaiSrc = [ join_paths(meson.current_source_dir(),
		       './src/collisions/CCollisionBrute.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/collisions/CCollisionAABBTree.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/collisions/CGenericCollision.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/collisions/CCollisionAABB.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/materials/CTexture3d.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/materials/CMaterial.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/materials/CTexture2d.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/materials/CTexture1d.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/materials/CTextureVideo.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/materials/CNormalMap.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/display/CCamera.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/display/CFrameBuffer.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/system/CGlobals.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/system/CString.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/system/CMutex.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/system/CThread.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/files/CFileModelSTL.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/files/CFileModel3DS.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/files/CFileXML.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/files/CFileImageJPG.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/files/CFileAudioWAV.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/files/CFileImagePNG.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/files/CFileImagePPM.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/files/CFileModelOBJ.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/files/CFileImageBMP.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/files/CFileImageGIF.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/files/CFileImageRAW.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/graphics/CPointArray.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/graphics/CDisplayList.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/graphics/CFog.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/graphics/CFont.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/graphics/CMultiImage.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/graphics/CColor.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/graphics/CVideo.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/graphics/CImage.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/graphics/CDraw3D.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/graphics/CSegmentArray.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/graphics/CPrimitives.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/graphics/CTriangleArray.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/widgets/CBackground.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/widgets/CBitmap.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/widgets/CLabel.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/widgets/CDial.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/widgets/CLevel.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/widgets/CGenericWidget.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/widgets/CViewPanel.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/widgets/CPanel.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/widgets/CScope.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/forces/CAlgorithmFingerProxy.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/forces/CGenericForceAlgorithm.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/forces/CAlgorithmPotentialField.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/world/CShapeEllipsoid.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/world/CShapeSphere.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/world/CMultiSegment.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/world/CGenericObject.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/world/CMesh.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/world/CMultiMesh.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/world/CShapeBox.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/world/CVoxelObject.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/world/CShapeTorus.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/world/CMultiPoint.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/world/CWorld.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/world/CShapeCylinder.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/world/CShapeLine.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/effects/CGenericEffect.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/effects/CEffectViscosity.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/effects/CEffectSurface.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/effects/CEffectVibration.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/effects/CEffectMagnet.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/effects/CEffectStickSlip.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/devices/CLeapDevices.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/devices/CDeltaDevices.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/devices/CMockDevice.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/devices/CGenericDevice.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/devices/CSixenseDevices.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/devices/CHapticDeviceHandler.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/devices/CPhantomDevices.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/devices/CGenericHapticDevice.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/shaders/CShaderProgram.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/shaders/CShader.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/timers/CFrequencyCounter.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/timers/CPrecisionClock.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/lighting/CDirectionalLight.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/lighting/CGenericLight.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/lighting/CShadowMap.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/lighting/CPositionalLight.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/lighting/CSpotLight.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/tools/CToolCursor.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/tools/CHapticPoint.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/tools/CGenericTool.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/tools/CToolGripper.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/audio/CAudioDevice.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/audio/CAudioBuffer.cpp')
	  , join_paths(meson.current_source_dir(),
		       './src/audio/CAudioSource.cpp')
	  ]


chaiInclude = [ include_directories('include') ]

if target_machine.system() == 'linux'
  dependencies = [drd, dl, usb, thread_dep]
  core_ldflags = [ ]
else
  core_ldflags = [ ]
  dependencies = [ ]
  chaiInclude += include_directories(join_paths(meson.current_source_dir(),
						'external/Eigen'))
  chaiInclude += include_directories(join_paths(meson.current_source_dir(),
						'external/'))
endif

# install_dir must be provided
install_subdir('include', install_dir : '')

chai3d_shared = shared_library('chai3d'
			      , chaiSrc
			      , include_directories : chaiInclude
			      , dependencies : dependencies
			      , link_args : core_ldflags
			      , c_args : extra_args
			      , install : true)

chai3d_static = static_library('chai3d'
			      , chaiSrc
			      , include_directories : chaiInclude
			      , dependencies : dependencies
			      , link_args : core_ldflags
			      , c_args : extra_args
			      , install : false)

chai3d_test = executable('test-chai'
			, './test/check_shape.cc'
			, include_directories : chaiInclude
			, link_args : core_ldflags
			, c_args : extra_args
			, link_with : chai3d_static
			, install : false)

test('chai3d', chai3d_test, is_parallel : true)


subdir('./modules/cAPI')
install_data(['./extras/hdPhantom/deps/libHD.so',
              './extras/hdPhantom/deps/libHD.so',
              './extras/hdPhantom/deps/libPhantomIOLib42.so'],
             rename : ['libHD.so', 'libHD.so.3.4', 'libPhantomIOLib42.so'],
             install_dir : 'lib')
