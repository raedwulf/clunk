import os, sys

env = Environment()
debug = False

sdl_cflags = env.ParseFlags('!pkg-config --cflags sdl')
sdl_libs = env.ParseFlags('!pkg-config --libs sdl')

env.MergeFlags(sdl_cflags)
env.MergeFlags(sdl_libs)

lib_dir = '.'
have_sse = True
debug = True
prefix = ''

Export('sdl_cflags')
Export('sdl_libs')
Export('lib_dir')
Export('have_sse')
Export('env')
Export('debug')
env['prefix'] = ''

env.Append(LIBPATH=['.'])
env.Append(CPPDEFINES=['DEBUG'])
buildmode = 'debug'

clunk = env.SharedLibrary('clunk', 
	[
		'context.cpp', 'sample.cpp', 'object.cpp', 'source.cpp', 'sdl_ex.cpp', 'stream.cpp', 
		'kemar.c', 'buffer.cpp', 'distance_model.cpp', 'logger.cpp', 'clunk_ex.cpp', 
	], 
	LIBS=['SDL'])


if sys.platform != 'win32':
	env.Append(LINKFLAGS=['-Wl,-rpath,'+ lib_dir])
	env.Append(LINKFLAGS=['-Wl,-rpath-link,.'])

env.Program('clunk_test', ['test.cpp'], LIBS=['clunk'])