thread_dep = dependency('threads')

cpp = meson.get_compiler('cpp')

# api = shared_library('HPGE', './src/HPGE.cpp' ,
#                      include_directories : [include_directories('./src'),
#					    include_directories('../../include')],
#		     link_with : chai3d_shared,
#                      install : true )

if target_machine.system() != 'linux'
  if host_machine.cpu_family() == 'x86_64'
    hdphantomname = 'hdPhantom64'
  else
    hdphantomname = 'hdPhantom32'
  endif
else
    hdphantomname = 'hdPhantom'
endif

hldep = cpp.find_library('HL', dirs : join_paths(meson.current_source_dir(),
						 '../../extras/hdPhantom/deps/'))
hddep = cpp.find_library('HD', dirs : join_paths(meson.current_source_dir(),
						 '../../extras/hdPhantom/deps/'))
hdphantom = shared_library(hdphantomname, '../../extras/hdPhantom/hdPhantom.cpp',
			   include_directories : [ include_directories('../../extras/hdPhantom/'),
						   include_directories('../../extras/hdPhantom/include'),
						   chaiInclude],
			   dependencies : [hldep, hddep],
                           install : true )

HGPE_includes = [ include_directories('./src')
		 , include_directories('../../include')
		 , include_directories('../../external/Eigen')
		 ]

HPGE_sources = [ './src/errors.cc'
		  , './src/hooks.cc'
		  , './src/logging.cc'
		  , './src/objects.cc'
		  , './src/positions.cc'
		  , './src/thread.cc'
		  , './src/transformation.cc'
		  , './src/versioninfo.cc'
		  , './src/world.cc' ]

tests_include = [ include_directories('../../include')
		, include_directories('./dependencies')
		, include_directories('./src') ]

api_static_chai = shared_library('HPGE',
				 HPGE_sources,
				 include_directories : HGPE_includes,
				 link_with : [ chai3d_static ],
				 dependencies : [thread_dep],
				 install : true)

if target_machine.system() != 'linux'
    tests_include += chaiInclude
	#catch = static_library('catch', './test/src/tests.cc'
	#	       , include_directories : [ include_directories('./dependencies') ]
	#	       , install : false)
else
	#catch = library('catch', './test/src/tests.cc'
	#			   , include_directories : [ include_directories('./dependencies') ]
	#			   , install : false)
endif



# tests_link = [ api_static_chai, catch ]

# benchpress = shared_library('benchpress', './test/src/benchmarks.cc'
#		       , include_directories : [ include_directories('./dependencies') ]
#		       , install : false)

#test_api = executable('test-api', './test/src/test-api.cc'
#		      , include_directories : tests_include
#		      , link_args : core_ldflags
#		      , link_with : api_static_chai)
#
#test_base_device = executable('test-base-device', './test/src/test-base-device.cc'
#			      , include_directories : tests_include
#			      , link_with : tests_link)
#
#test_errors = executable('test-errors', './test/src/test-errors.cc'
#			 , include_directories : tests_include
#			 , link_with : tests_link)
#
#test_hook = executable('test-hook', './test/src/test-hook.cc'
#		       , include_directories : tests_include
#		       , link_with : tests_link)
#
#test_interpolation = executable('test-interpolation',
#				'./test/src/test-interpolation.cc'
#				, include_directories : tests_include
#				, link_with : tests_link)
#
#test_logging = executable('test-logging',
#			  './test/src/test-logging.cc'
#			  , include_directories : tests_include
#			  , link_with : tests_link)
#
#test_loop_frequency = executable('test-loop-frequency',
#				 './test/src/test-loop-frequency.cc'
#				 , include_directories : tests_include
#				 , link_args : core_ldflags
#				 , link_with : api_static_chai)
#
#test_mock = executable('test-mock', './test/src/test-mock.cc'
#		       , include_directories : tests_include
#		       , link_with : tests_link)
#
#test_newthread = executable('test-newthread', './test/src/test-newthread.cc'
#			    , include_directories : tests_include
#			    , link_with : tests_link)
#
## FIXME: add comparison between cube/cube mesh, sphere/sphere mesh and so on
#test_objects = executable('test-objects', './test/src/test-objects.cc'
#			  , include_directories : tests_include
#			  , link_with : tests_link)
#
## test_point = executable('test-point', './test/src/test-point.cc'
##			, include_directories : tests_include
##			, link_with : tests_link)
#
#test_textures = executable('test-textures', './test/src/test-textures.cc'
#			, include_directories : tests_include
#			, link_with : tests_link)
#
#test_thread = executable('test-thread', './test/src/test-thread.cc'
#			 , include_directories : tests_include
#			 , link_with : tests_link)
#
#test_various = executable('test-various', './test/src/test-various.cc'
#			  , include_directories : tests_include
#			  , link_with : tests_link)
#
#test_workspace = executable('test-workspace', './test/src/test-workspace.cc'
#			    , include_directories : tests_include
#			    , link_with : tests_link)
#
## benchmark_point = executable('benchmark-point', './test/src/benchmark-point.cc'
##			     , include_directories : [ include_directories('./dependencies')
##						     , include_directories('../../include')
##						     , include_directories('./src') ]
##			     , link_with : [ benchpress, api_static_chai ]
##)
#
## test_template = executable('test-template', './test/src/template.cc'
##			   , include_directories : [ include_directories('./dependencies')
##						   , include_directories('../../include')
##						   , include_directories('./src') ]
##			   , link_with : tests_link
##)
#
#test('api', test_api, is_parallel : false)
#test('base_device', test_base_device, is_parallel : false)
#test('errors', test_errors, is_parallel : false)
#test('hook', test_hook, is_parallel : false)
#test('interpolation', test_interpolation, is_parallel : false)
#test('logging', test_logging, is_parallel : false)
#test('loop_frequency', test_loop_frequency, is_parallel : false)
#test('mock', test_mock, is_parallel : false)
#test('newthread', test_newthread, is_parallel : false)
## test('objects', test_objects, is_parallel : false)
## test('point', test_point, is_parallel : false) # FIXME: disabled until fixed
#test('textures', test_textures, is_parallel : false)
#test('thread', test_thread, is_parallel : false)
#test('various', test_various, is_parallel : false)
#test('workspace', test_workspace, is_parallel : false)

# test('test_template', test_temlate, is_parallel : false)

# benchmark('benchmark_point', benchmark_point)
