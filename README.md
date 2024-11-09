# Aether Renderer

Aether Renderer is a physically-based rendering (PBR) engine built with OpenGL and C++. It is designed for real-time rendering with advanced lighting models and post-processing effects. The renderer is primarily a forward renderer, but it also supports depth pre-pass, model loading, shadow mapping, SSAO, HDR, and real-time scene editing with an in-editor interface for manipulating lights, objects, and materials.

## Features

- **Forward Rendering with Depth Pre-Pass**: Aether Renderer uses a forward rendering pipeline with optional depth pre-pass to optimize performance by reducing overdraw.
- **Model Loader**: Supports loading of 3D models in common formats like `.obj` and `.fbx` using the built-in model loader.
- **Shadow Mapping**: Simple support for shadow mapping for both directional and point lights.
- **Screen-Space Ambient Occlusion (SSAO)**: Adds ambient occlusion effects to improve shading and realism.
- **Gamma Correction**: Automatically applies gamma correction for accurate rendering of lighting and color.
- **High Dynamic Range (HDR)**: Supports HDR rendering for a more realistic and vibrant scene.
- **Real-Time Lighting**: You can add and modify directional and point lights in real-time.
- **Editor Interface**: Built-in scene hierarchy editor to interact with the scene, adjust lighting, and modify object properties.

## Libraries

Aether Renderer relies on the following libraries for its functionality:

- **GLFW**: Used for window/context creation and input management.
- **GLM**: A math library for handling transformations, vectors, matrices, and other math operations.
- **Assimp**: Used for importing 3D models in various formats such as `.obj` and `.fbx`.
- **STB Image**: A simple library for loading image files (e.g., textures).
- **ImGui**: A GUI library used for the in-editor interface, enabling real-time interaction with the scene.

## Getting Started

### 1. Clone the Repository

To get started with Aether Renderer, first, clone the repository:

```bash
git clone https://github.com/yourusername/aether-renderer.git
cd aether-renderer
```
### 2. Install Dependencies
Make sure the following libraries are available and linked in your Visual Studio project:

GLFW: For window/context creation.
GLM: For math operations.
Assimp: For model loading.
STB Image: For loading textures.
ImGui: For the editor interface.
You can download the libraries manually or use a package manager like vcpkg or conan to install them.

### 3. Open the Solution in Visual Studio
Open the AetherRenderer.sln solution file in Visual Studio.
Set the build configuration to Debug or Release depending on your needs.
Make sure that all dependencies (GLFW, GLM, Assimp, STB Image, and ImGui) are correctly linked in the project settings. You may need to configure include directories and library paths in the Visual Studio project properties.
### 4. Build the Project
To build the project in Visual Studio:

Press Ctrl+Shift+B to build the solution, or use the Build > Build Solution option in the menu.
### 5. Run the Renderer
After building the project, you can run the application by pressing F5 (or Start in the Visual Studio toolbar). Alternatively, you can navigate to the build output directory and run the executable manually:

```bash
./AetherRenderer.exe
```

## Features and Usage

### Forward Rendering and Depth Pre-Pass
Aether Renderer uses a forward rendering pipeline by default. You can enable the depth pre-pass to improve performance by reducing overdraw. This setting can be configured through the in-editor controls or modified in the configuration file.

### Model Loading
The engine supports loading models in .obj, .fbx, and other popular 3D formats. You can load models into the scene by dragging and dropping them into the scene hierarchy in the editor.

### Lighting
Aether Renderer supports two types of lights:

#### Directional Lights: 
Typically used to simulate sunlight or distant light sources. Adjust its direction and intensity in the editor.
####Point Lights:
Simulate point-source lights (e.g., light bulbs). Adjust position, intensity, and radius in real-time.
Both light types can be added, removed, and modified in real-time via the scene editor interface.

### Shadows
The renderer supports simple shadow mapping for both point and directional lights. You can enable and tweak shadow settings such as shadow resolution, bias, and range through the scene editor.

### Post-Processing Effects
#### SSAO (Screen-Space Ambient Occlusion)
Adds depth and subtle shading to the scene, especially in corners and crevices.
Gamma Correction: Ensures proper light intensity and color reproduction.
HDR (High Dynamic Range): Provides better lighting accuracy and more vibrant color representation.
#### Scene Editor
The built-in scene editor allows you to interact with the scene and modify objects and lighting in real-time. Features include:

#### Scene Hierarchy
A panel where you can organize and manage objects in your scene.

All changes made in the editor are reflected in real-time, providing an interactive and immediate feedback loop for tweaking scenes.

## Configuration
Certain settings of the renderer can be adjusted by modifying the config.ini file, located in the project root. You can configure options like:

Enable Depth Pre-Pass
Shadow Quality (Resolution, Bias, Range)
Enable/Disable SSAO
Enable/Disable HDR
Gamma Correction Settings
## Contributing
We welcome contributions to Aether Renderer! Whether you're fixing bugs, improving performance, or adding new features, your contributions are appreciated. To contribute:

Fork the repository.
Create a new branch for your feature or bug fix.
Submit a pull request with a clear description of your changes.
Please ensure your changes follow the project's coding style and include appropriate comments and documentation.

## License
Aether Renderer is licensed under the MIT License. See the LICENSE file for more details.


This should now be the full and correct content for your `README.md` file. If you have further requirements or adjustments, feel free to let me know!


