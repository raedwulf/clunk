import os, sys

env = Environment()
debug = False

have_samplerate = True
#have_samplerate = False

sdl_cflags = env.ParseFlags('!pkg-config --cflags sdl')
sdl_libs = env.ParseFlags('!pkg-config --libs sdl')
samplerate_cflags = env.ParseFlags('!pkg-config --cflags samplerate')
samplerate_libs = env.ParseFlags('!pkg-config --libs samplerate')

env.MergeFlags(sdl_cflags)
env.MergeFlags(sdl_libs)

if have_samplerate:
	env.MergeFlags(samplerate_cflags)
	env.MergeFlags(samplerate_libs)

lib_dir = '.'
have_sse = False
#have_sse = True
debug = True
#debug = False
prefix = ''

Export('sdl_cflags')
Export('sdl_libs')
Export('lib_dir')
Export('have_sse')
Export('env')
Export('debug')
env['prefix'] = ''

env.Append(LIBPATH=['.'])
env.Append(CPPDEFINES=['DEBUG', '_REENTRANT'])
if have_sse:
	env.Append(CPPDEFINES=['CLUNK_USES_SSE'])

if debug:
	buildmode = 'debug'
	env.Append(CXXFLAGS=['-ggdb'])
else:
	buildmode = 'release'
	env.Append(CXXFLAGS=['-O3', '-mtune=native', '-march=native'])

clunk_src = [
	'context.cpp', 'sample.cpp', 'object.cpp', 'source.cpp', 'sdl_ex.cpp', 'stream.cpp', 
	'kemar.c', 'buffer.cpp', 'distance_model.cpp', 'logger.cpp', 'clunk_ex.cpp', 'clunk_c.cpp'
]
if have_sse:
	clunk_src.append('sse_fft_context.cpp')

clunk = env.SharedLibrary('clunk', 
	clunk_src, 
	LIBS=['SDL','samplerate'])


if sys.platform != 'win32':
	env.Append(CFLAGS=['-Wall', '-pedantic'])
	env.Append(CXXFLAGS=['-Wall', '-pedantic'])
	env.Append(LINKFLAGS=['-Wl,-rpath,'+ lib_dir])
	env.Append(LINKFLAGS=['-Wl,-rpath-link,.'])

env.Program('clunk_test', ['test.cpp'], LIBS=['clunk'])
env.Program('clunk_c_test', ['test_c.c'], LIBS=['clunk'])
