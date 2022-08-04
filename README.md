DrawSpace
=========

My C/C++ graphic rendering engine including features for 3D spaces simulation, especially procedural planets generation & rendering,
including accurate atmospheric effects, 3D clouds, water rendering, etc...

Full GPLv3 text is available in the ./licenses directory

DrawSpace currently rely on some 3rd-parts libraries:

	** Direct3D 9 SDK : 	      http://www.microsoft.com/en-us/download/details.aspx?id=6812
	** Direct3D 11 SDK :          https://www.microsoft.com/en-us/download/details.aspx?id=6812
	** FW1FontWrapper for D3D11 : https://fw1.codeplex.com                  
	** Bullet physics 2.82 :      http://bulletphysics.org/wordpress/
	** Assimp :                   http://www.assimp.org/
	** Lua 5.1 :                  http://www.lua.org/
	** PhysicsFS 2.0.3 :          https://icculus.org/physfs/
	** CEGUI 0.8.7 :              http://cegui.org.uk/
	** JSMN JSON parser	      https://github.com/zserge/jsmn
	** RSA Data Security, Inc. MD5 Message-Digest Algorithm
	** Wombat efficient texture-free procedural noise library for shaders : https://github.com/BrianSharpe/Wombat
	

1/ cmake -G"Visual Studio 16 2019" -A Win32 

2/ duplicate 'config' directory to 'config_deployment'

3/ open project with visual studio : ds_engine.sln

4/ under VS IDE : compile all by right-click -> 'generate' on ALL_BUILD project and/or 'INSTALL' (Debug and/or Release)

5/ right-click on project 'rt' -> 'properties' : go on 'Debugging' section :
	-> 'Command' : set value to "<drawspace_install>\apps\Debug\rt.exe"
	-> 'Command args' : set to module to run : gameroom,waterdemo,impostorsdemo,nebulaeluademo,planetluademo,etc... 
	-> 'Working directory' : set to "<drawspace_install>\apps\Debug"


Animated water rendering:

![alt tag](https://github.com/NevilClavain/DrawSpace/blob/develop/screens/Image4.jpg)
![alt tag](https://github.com/NevilClavain/DrawSpace/blob/develop/screens/Image5.jpg)

Bones-based animations execution :

![alt tag](https://github.com/NevilClavain/DrawSpace/blob/develop/screens/Image1.jpg)

Material shaders:

![alt tag](https://github.com/NevilClavain/DrawSpace/blob/develop/screens/Image3.jpg)

Procedural planet:

![alt tag](https://github.com/NevilClavain/DrawSpace/blob/develop/screens/Image6.jpg)
![alt tag](https://github.com/NevilClavain/DrawSpace/blob/develop/screens/Image7.jpg)
![alt tag](https://github.com/NevilClavain/DrawSpace/blob/develop/screens/Image9.jpg)
![alt tag](https://github.com/NevilClavain/DrawSpace/blob/develop/screens/Image11.jpg)
![alt tag](https://github.com/NevilClavain/DrawSpace/blob/develop/screens/Image13.jpg)
![alt tag](https://github.com/NevilClavain/DrawSpace/blob/develop/screens/Image15.jpg)


