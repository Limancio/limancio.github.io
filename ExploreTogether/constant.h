#ifndef CONSTANT_H
#define CONSTANT_H

#define TEXTURE_OPEN_FILE_PATH     "data/textures/*"
#define TEXTURE_FILE_PATH          "data/textures/"
#define TEXTURE_MESH_FILE_PATH     "data/textures/mesh/"
#define TEXTURE_DISPLACEMENT_FILE_PATH "data/textures/displacement/"

#define MESH_OPEN_FILE_PATH    "data/models/*"
#define MESH_FILE_PATH         "data/models/"

#define LEVEL_FILES_PATH       "data/levels/*"
#define LEVEL_PATH             "data/levels/"
#define LEVEL_DEFAULT_NAME     "level_name"

#include "basic/string.h"

// const string values:
const string level_directory_path  = const_string(LEVEL_PATH);
      
const string texture_directory_path  = const_string(TEXTURE_FILE_PATH);
const string texture_directory_open_path  = const_string(TEXTURE_OPEN_FILE_PATH);
const string texture_directory_mesh_path  = const_string(TEXTURE_MESH_FILE_PATH);
const string texture_directory_displacement_path = const_string(TEXTURE_DISPLACEMENT_FILE_PATH);

#endif // !CONSTANT_H
