# WetlingRealsenseES

## Application Preview
![image](https://user-images.githubusercontent.com/21957042/49209140-acb65380-f3f4-11e8-92a2-5fd1b3859b1f.png)

## Documentation
Official Realsense C++ Documentation: https://intelrealsense.github.io/librealsense/doxygen/annotated.html

## Installation Guide
#### Adding Realsense SDK
1. Create new Empty Visual C++ Project
2. Go to View > Other Windows > Property Manager
3. Click on "Add Existing Property Sheet" icon
4. Navigate to "C:\Program Files (x86)\Intel RealSense SDK 2.0"
5. Add intel.realsense.props, now #include <librealsense2/rs.h> can be used

#### Adding Custom Libraries
6. Copy folder [realsense] into Project's File Explorer
7. Copy folder [imgui] into Project's File Explorer
(NOTE: Copy the above files from Realsense\third-party\imgui instead of the official imgui release)
8. Create new filters [realsense] & [imgui] and Add > Existing Items > Include all items in respective folders created earlier in steps 6 and 7

#### Adding OpenGL/GLFW Libraries
9. Properties > VC++ > Include Directories > C:\Users\efoo1\Desktop\glfw-3.2.1.bin.WIN32\include
(NOTE: 64bit: C:\Users\efoo1\Desktop\glfw-3.2.1.bin.WIN64\include)
10. Properties > VC++ > Library Directories > C:\Users\efoo1\Desktop\glfw-3.2.1.bin.WIN32\lib-vc2015
(NOTE: 64bit: C:\Users\efoo1\Desktop\glfw-3.2.1.bin.WIN64\lib-vc2015)
11. Linker > Input > Additional Dependencies > glfw3.lib + opengl32.lib

#### Adding OpenCV Library
12. Verify Visual Studio toolbar has Solution Configurations set to "Debug", and Solution Platforms to "x64"
(NOTE: New versions of OpenCV only supports x64 binaries)
13. Project > Properties (Active(Debug)) > Configuration Properties > VC++ Directories > Include Directories > Add > C:\OpenCV-3.4.3\opencv\build\include
14. Project > Properties (Active(Debug)) > Configuration Properties > VC++ Directories > Library Directories > Add > C:\OpenCV-3.4.3\opencv\build\x64\vc15\lib
15. Open Windows File Explorer > C:\OpenCV-3.4.3\opencv\build\x64\vc15\lib > Copy the filename "opencv_world343d.lib"
16. Visual Studio > Project > Properties > Configuration Properties > Linker > Input > Additional Dependencies > Edit > opencv_world343d.lib

#### If "fopen" error warning occurs, do the following to fix compiler:
1. Select Project and click "Properties" from the context menu
2. Go to Configuration Properties > C/C++ > Preprocessor
3. Add ;_CRT_SECURE_NO_WARNINGS in the ProprocessorDefinitions field

## GUI Progression
![1](https://user-images.githubusercontent.com/21957042/49271286-e5fbcb80-f4a7-11e8-8096-f938aaba84fa.png)

![2](https://user-images.githubusercontent.com/21957042/49271290-e85e2580-f4a7-11e8-9590-21949dfd1683.png)

![3](https://user-images.githubusercontent.com/21957042/49271294-ea27e900-f4a7-11e8-8e9e-177ba37e288b.png)

![4](https://user-images.githubusercontent.com/21957042/49271298-ec8a4300-f4a7-11e8-9a76-292c7ed3ab8f.png)

![5](https://user-images.githubusercontent.com/21957042/49271301-ee540680-f4a7-11e8-9796-bed18984862b.png)
