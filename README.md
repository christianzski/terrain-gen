# Terrain Generator
## Introduction
A very simple procedural terrain generator, created as a weekend project with C++/OpenGL. Terrains can be updated in real-time or through a configuration file. Afterwards, terrain meshes can be exported to OBJ files.
    
## Images
<p align="center">
  <img src="https://drive.google.com/uc?id=1D65JS0yUnHyeay2Ub4abQY4jYtNMG4fj">
</p>

<p align="center">
  <img src="https://drive.google.com/uc?id=1JOSGePYOdD01vijqJBQQ8ACjBkNgFIMl">
</p>

## Features
### Current Features
- Procedural terrain generation w/ perlin noise
- Color multiple terrain types by height
- Export mesh to OBJ (no materials)
- Default settings configurable with config file

### Future Enhancements
- Textures w/ triplanar mapping
- Chunking w/ LOD
- Normal maps
- PBR shading
- Better controls; e.g., sliders, displaying noise textures

## Getting Started
### Compilation
1. Clone the repository
2. Generate [GLAD loader files](https://glad.dav1d.de)
3. Build with CMake

### Building
Currently only Linux builds have been tested. You must have the `glm` and `glfw3` packages installed.
Configure the project with CMake, and then compile the source:

```bash
mkdir build
cd build
cmake ..
make
```

### Configuration
Please see the sample `config.txt` for examples on how to configure the default terrain settings.

## Usage
### Key Bindings
| Keys | Action              |
|------|---------------------|
| 1/2  | Adjust Amplitude    |
| 3/4  | Adjust Frequency    |
| 5/6  | Adjust Persistance  |
| 7/8  | Adjust Lacunarity   |
| Z    | Stop camera rotation|
| F1   | Toggle wireframe    |
| F2   | Toggle flat shading |
| F5   | Refresh config.txt  |
| F12  | Export OBJ mesh     |
