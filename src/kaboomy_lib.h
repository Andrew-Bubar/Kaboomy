#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Define
#ifdef _WIN32
#define DEBUG_BREAK() __debugbreak()
#elif __linux__
#define DEBUG_BREAK() __builtin_debugtrap()
#elif __APPLE__
#define DEBUG_BREAK() __builtin_trap()
#endif

//Logging
enum TextColor
{  
  TEXT_COLOR_BLACK,
  TEXT_COLOR_RED,
  TEXT_COLOR_GREEN,
  TEXT_COLOR_YELLOW,
  TEXT_COLOR_BLUE,
  TEXT_COLOR_MAGENTA,
  TEXT_COLOR_CYAN,
  TEXT_COLOR_WHITE,
  TEXT_COLOR_BRIGHT_BLACK,
  TEXT_COLOR_BRIGHT_RED,
  TEXT_COLOR_BRIGHT_GREEN,
  TEXT_COLOR_BRIGHT_YELLOW,
  TEXT_COLOR_BRIGHT_BLUE,
  TEXT_COLOR_BRIGHT_MAGENTA,
  TEXT_COLOR_BRIGHT_CYAN,
  TEXT_COLOR_BRIGHT_WHITE,
  TEXT_COLOR_COUNT
};


template<typename ...Args>
void _log(char* prefix, char* msg, TextColor textColor, Args... args){

    static char* TextColorTable[TEXT_COLOR_COUNT] = 
    {    
    "\x1b[30m", // TEXT_COLOR_BLACK
    "\x1b[31m", // TEXT_COLOR_RED
    "\x1b[32m", // TEXT_COLOR_GREEN
    "\x1b[33m", // TEXT_COLOR_YELLOW
    "\x1b[34m", // TEXT_COLOR_BLUE
    "\x1b[35m", // TEXT_COLOR_MAGENTA
    "\x1b[36m", // TEXT_COLOR_CYAN
    "\x1b[37m", // TEXT_COLOR_WHITE
    "\x1b[90m", // TEXT_COLOR_BRIGHT_BLACK
    "\x1b[91m", // TEXT_COLOR_BRIGHT_RED
    "\x1b[92m", // TEXT_COLOR_BRIGHT_GREEN
    "\x1b[93m", // TEXT_COLOR_BRIGHT_YELLOW
    "\x1b[94m", // TEXT_COLOR_BRIGHT_BLUE
    "\x1b[95m", // TEXT_COLOR_BRIGHT_MAGENTA
    "\x1b[96m", // TEXT_COLOR_BRIGHT_CYAN
    "\x1b[97m", // TEXT_COLOR_BRIGHT_WHITE
    };

    char formatBuffer[8192] = {};
    sprintf(formatBuffer, "%s %s %s \033[0m", TextColorTable[textColor], prefix, msg);

    char textBuffer[8912] = {};
    sprintf(textBuffer, formatBuffer, args...);

    puts(textBuffer);
}

#define SM_TRACE(msg, ...) _log("TRACE: ", msg, TEXT_COLOR_GREEN, ##__VA_ARGS__);
#define SM_WARN(msg, ...) _log("WARN: ", msg, TEXT_COLOR_YELLOW, ##__VA_ARGS__);
#define SM_ERROR(msg, ...) _log("ERROR: ", msg, TEXT_COLOR_RED, ##__VA_ARGS__);

#define SM_ASSERT(x, msg, ...)                        \
{                                                     \
  if(!(x)){                                           \
   SM_ERROR(msg, ##__VA_ARGS__);                      \
   DEBUG_BREAK();                                     \
   SM_ERROR("Assert Hit!");                           \
  }                                                   \
}                                                     \


// Bump Allcator
struct BumpAllocator{

  size_t capacity;
  size_t used;
  char* memory;
};

BumpAllocator make_bump_allocator(size_t size){

  BumpAllocator ba = {};

  ba.memory = (char*)malloc(size);

  if(ba.memory){

    ba.capacity = size;
    memset(ba.memory, 0, size);
  }else {
    SM_ASSERT(false, "Failed to allocate memory");
  }
  return ba;
}

char* bump_alloc(BumpAllocator* ba, size_t size){

  char* result = nullptr;
  size_t allingedSize = (size + 7) & ~ 7; 

  if(ba->used + allingedSize <= ba->capacity){
    result = ba->memory + ba->used;
    ba->used += allingedSize;
  }
  else {

    SM_ASSERT(false, "BumpAllocator is full!");
  }

  return result;
}

//File I/O
long long get_timestamp(char* file){

  struct stat file_stat = {};
  stat(file, &file_stat);

  return file_stat.st_mtime;
}

bool file_exists(char* filePath){

  SM_ASSERT(filePath, "No file Exists!");

  auto file = fopen(filePath, "rb");
  if(!file){
    return false;
  }else {
    fclose(file);
  }
  return true;
}

long get_file_size(char* filePath){

  SM_ASSERT(filePath, "No filePath supplied!");

  long fileSize = 0;
  auto file = fopen(filePath, "rb");

  if(!file){
    SM_ERROR("Failed to open file: %s", filePath);
    return 0;
  }

  fseek(file, 0, SEEK_END);
  fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);
  fclose(file);

  return fileSize;
}

char* read_file(char* filePath, int* fileSize, char* buffer){

  SM_ASSERT(filePath, "No Filepath!");
  SM_ASSERT(fileSize, "No filesize!");
  SM_ASSERT(buffer, "No buffer!");

  *fileSize = 0;
  auto file = fopen(filePath, "rb");
  if(!file){
    SM_ERROR("failed to open file: %s", filePath);
    return nullptr;
  }

  fseek(file, 0, SEEK_END);
  *fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);
  
  memset(buffer, 0, *fileSize + 1);
  fread(buffer, sizeof(char), *fileSize, file);

  fclose(file);
  return buffer;
}

char* read_file(char* filePath, int* fileSize, BumpAllocator* ba){

  char* file = nullptr;
  long fileSize2 = get_file_size(filePath);

  if(fileSize2){

    char* buffer = bump_alloc(ba, fileSize2 + 1);

    file = read_file(filePath, fileSize, buffer);
  }

  return file;
}

void write_file(char* filePath, char* buffer, int size){

  
}
