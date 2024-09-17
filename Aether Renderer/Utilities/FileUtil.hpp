#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

static std::string ReadShaderFromFile(std::string Path) {
     std::string fileContent;

     std::ifstream file;
     file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
     try
     {
         // open files
         file.open(Path);
         std::stringstream stream;
         stream << file.rdbuf();
         file.close();
         // convert stream into string
         fileContent = stream.str();
     }
     catch (std::ifstream::failure e)
     {
         std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
     }
     return fileContent;
}

static std::string GetImagePath(std::string name) {
    return (fs::current_path() / fs::path("Resources/" + name)).string();
}

static std::string GetShaderPath(std::string shaderName) {
    return (fs::current_path() / fs::path("Core/Shaders/" + shaderName)).string();
}