project('play-gl-ground', 'cpp')

inc_dir = include_directories('include')

sdl2 = dependency('sdl2')
gl = dependency('opengl')
glew = dependency('glew')

executable('triangle', 'triangle.cpp',
           dependencies : [sdl2, gl, glew],
           include_directories : inc_dir,
           cpp_args : '-std=c++20')

executable('rectangle', 'rectangle.cpp',
           dependencies : [sdl2, gl, glew],
           include_directories : inc_dir,
           cpp_args : '-std=c++20')
