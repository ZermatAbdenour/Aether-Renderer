#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

static std::string ReadShaderFromFile(const std::string& Path) {
    static std::string fileContent; // Use static to keep it alive after the function returns
    fileContent.clear(); // Clear previous content

    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // Open files
        file.open(Path);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        // Convert stream into string
        fileContent = stream.str();
    }
    catch (const std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        return NULL;
    }
    return fileContent;
}

static std::string GetImagePath(std::string name) {
    return (fs::relative((fs::current_path() / fs::path("Resources/Images"), fs::path(name)))).string();
}

static std::string GetModelPath(std::string name) {
    return (fs::current_path() / fs::path("Resources/Models/")/fs::path(name)).string();
}

static std::string GetShaderPath(std::string shaderName) {
    return (fs::current_path() / fs::path("Engine/Shaders/" + shaderName)).string();
}