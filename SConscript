import os, sys
Import('env')
Import('debug')
Import('sdl_cflags')
Import('sdl_libs')
Import('samplerate_cflags')
Import('samplerate_libs')
Import('lib_dir')
Import('have_sse')
Import('have_samplerate')

env = env.Clone()
env.Append(CPPPATH=['..', '.'])
env.Append(LIBPATH=['.'])
env.Append(CPPDEFINES=['CLUNKAPI=DLLEXPORT'])
if have_sse: 
	env.Append(CPPDEFINES=['CLUNK_USES_SSE'])

if have_samplerate:
	clunk_libs = ['SDL', 'samplerate']
else:
	clunk_libs = ['SDL']

if sys.platform != 'win32':
	env.Append(LINKFLAGS=['-Wl,-rpath,'+ lib_dir])
	clunk_libs.append('m')

env.MergeFlags(sdl_cflags, sdl_libs)

if have_samplerate:
	env.MergeFlags(samplerate_cflags, samplerate_libs)
	
clunk_src = [
	'context.cpp', 'sample.cpp', 'object.cpp', 'source.cpp', 'sdl_ex.cpp', 'stream.cpp', 
	'kemar.c', 'buffer.cpp', 'distance_model.cpp', 'logger.cpp', 'clunk_ex.cpp', ]
	
if have_sse:
	clunk_src.append('sse_fft_context.cpp')

clunk = env.SharedLibrary('clunk', clunk_src, LIBS=clunk_libs)

if sys.platform != 'win32' and len(env['prefix']) > 0:
	Import('install_targets')
	Import('lib_dir')
	install_targets.append(Install(lib_dir, clunk))
else:
	Install('#', clunk[0])
