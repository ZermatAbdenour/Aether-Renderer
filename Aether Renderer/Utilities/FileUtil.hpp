#pragma once
#include <fstream>
#include <iostream>
#include <filesystem>
#include <Windows.h>

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
    return (fs::current_path() / fs::path("Resources/Images" / fs::path(name))).string();
}

static std::string GetRessourcesPath() {
    return (fs::current_path() / fs::path("Resources")).string();
}

static std::string GetModelPath(std::string name) {
    return (fs::current_path() / fs::path("Resources/Models/") / fs::path(name)).string();
}

static std::string GetShaderPath(std::string shaderName) {
    return (fs::current_path() / fs::path("Engine/Shaders/" + shaderName)).string();
}

static std::string GetFileExtension(const std::string& filePath) {
    size_t dotPosition = filePath.find_last_of('.');

    if (dotPosition != std::string::npos && dotPosition != 0) {
        return filePath.substr(dotPosition + 1);
    }
    return "";
}

static std::string OpenFilePicker() {
    // Initialize COMDLG (Common Dialog) structure
    OPENFILENAME ofn = { 0 };
    TCHAR szFile[260] = { 0 };
    // Initialize remaining fields of OPENFILENAME structure
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;


    if (GetOpenFileName(&ofn) == TRUE) {
        std::wstring ws(szFile);
        std::string pth(ws.begin(), ws.end());
        return pth;
    }
    else {
        return "";
    }
}