#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <GL/glew.h>

#include "util.h"

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 512

float rectangle_vertices[] =
        {
         0.2f,  0.5f, 0.0f,  // middle top
         0.2f, -0.5f, 0.0f,  // middle bottom
         -0.5f, -0.5f, 0.0f,  // bottom left
         -0.5f,  0.5f, 0.0f,   // top left
         0.5f, 0.5f, 0.0f,// top right
         0.5f, -0.5f, 0.0f// bottom right
        };

unsigned int rectangle_index[] =
        {  // note that we start from 0!
         0, 1, 3,   // first triangle
         1, 2, 3,    // second triangle
         0, 1, 4,
         1, 4, 5
        };

constexpr int num_triangles = sizeof(rectangle_index) / sizeof(rectangle_index[0]);

int
main(int argc, char* args[]) {

  SDL_Window* window = nullptr;
  SDL_GLContext gl_context;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
          fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
          return 1;
  }

  window = SDL_CreateWindow(
                            "rectangle",
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            SCREEN_WIDTH, SCREEN_HEIGHT,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
                            );

  if (window == nullptr) {
          fprintf(stderr, "could not create window: %s\n", SDL_GetError());
          return 1;
  }

  // Main context for GL
  gl_context = SDL_GL_CreateContext(window);

  // Set all necesary attributes for opengl
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); // Dont use deprecated code

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // Double buffer

  // Draw the triangles with wireframe
  // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // To fill the triangles
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glewExperimental = GL_TRUE;

  // This makes our buffer swap syncronized with the monitor's vertical refresh
  SDL_GL_SetSwapInterval(1);

  glewInit();

  // Clear our buffer with a black background
  // This is the same as :
  //      SDL_SetRenderDrawColor(&renderer, 255, 0, 0, 255);
  //      SDL_RenderClear(&renderer);
  //
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(window);

  bool running = true;
  SDL_Event event;

  // Create the Element Buffer Object
  unsigned int EBO;
  glGenBuffers(1, &EBO);

  // Use a Vertex Array Object to ease things
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Create a Virtual Buffer Object to draw the triangle
  unsigned int VBO;
  glGenBuffers(1, &VBO);

  // Copy the triangle vertices into the buffer memory
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), rectangle_vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangle_index), rectangle_index, GL_STATIC_DRAW);

  // We now tell opengl how to interpret the vertices
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  // Enable attribute index 0 as being used
  glEnableVertexAttribArray(0);

  auto vertex_shader_temp = load_shader("triangle_vs.glsl");
  const char* vertex_shader_src = vertex_shader_temp.c_str();

  // Create a vertex shader to compile the shader src
  unsigned int vertex_shader;
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vertex_shader, 1, &vertex_shader_src, nullptr);
  glCompileShader(vertex_shader);

  auto fragment_shader_temp = load_shader("triangle_fs.glsl");
  const char* fragment_shader_src = fragment_shader_temp.c_str();

  // Generate and compile the fragment shader
  unsigned int fragment_shader;
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_src, nullptr);
  glCompileShader(fragment_shader);



  // A shader program is the result of linking multiple shaders
  unsigned int shader_program;
  shader_program = glCreateProgram();

  // Link all of the shaders
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  GLint is_linked;
  int max_length;

  glGetProgramiv(shader_program, GL_LINK_STATUS, &is_linked);
  if(!is_linked) {

          // Noticed that glGetProgramiv is used to get the length for a shader program, not glGetShaderiv.
          glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);

          // The maxLength includes the null character
          auto shaderProgramInfoLog = (char *)malloc(max_length);

          // Notice that glGetProgramInfoLog, not glGetShaderInfoLog.
          glGetProgramInfoLog(shader_program, max_length, &max_length, shaderProgramInfoLog);

          // Handle the error in an appropriate way such as displaying a message or writing to a log file.
          // In this simple program, we'll just leave
          free(shaderProgramInfoLog);
          exit(1);
  }

  glUseProgram(shader_program);

  // Once the shaders are linked we don't need the individual versions
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);


  while (running) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT) running = false;

        // First draw the background and then draw the triangle
        glClearColor(0.5, 0.5, 0.5, 0.1);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, num_triangles, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        SDL_GL_SwapWindow(window);

        if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                        running = false;
                        break;
                default:
                        break;
                }
        }

  }

  // Delete our opengl context
  SDL_GL_DeleteContext(gl_context);

  // Destroy our window
  SDL_DestroyWindow(window);

  // Shutdown SDL 2
  SDL_Quit();

  return 0;
}
