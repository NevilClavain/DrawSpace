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



![alt tag](https://2.bp.blogspot.com/-R6beb0Ym7wY/XJF6boDwcjI/AAAAAAAADZ8/im06Bwg1hxQ6kEWG0X8txeGmMprtuIzUQCLcBGAs/s1600/xf.jpg)

Animated water rendering:

![alt tag](https://github.com/NevilClavain/DrawSpace/blob/develop/screens/Image4.jpg)
![alt tag](https://github.com/NevilClavain/DrawSpace/blob/develop/screens/Image5.jpg)

Bones-based animations execution :

![alt tag](https://1.bp.blogspot.com/-R393UDVXHaA/XioOQcla87I/AAAAAAAADhA/WwpU6mMBuEU_ANKKJgfSprLoc7cCoxzxACLcBGAsYHQ/s1600/anims.jpg)

Material shaders:

![alt tag](https://1.bp.blogspot.com/-aqQActz5-Cw/W1TiUw-LkHI/AAAAAAAADW0/kPXzccgRmcIDiRSuflPfqITef64f6Lr5ACLcBGAs/s1600/bumpmap.jpg)

Procedural planet:

![alt tag](https://1.bp.blogspot.com/-0dPNeTT4lYw/V7NZ6Uve5HI/AAAAAAAABuY/lr7re1lYz0MLvO8uhMdNMGfGetvLT3nCwCLcB/s1600/Image59.png)

![alt tag](https://3.bp.blogspot.com/-82kYw9Zkzpk/VzjFhWsdqKI/AAAAAAAAAvg/AH5CPYrEQGoTS-qJMEPsXcybr7YJxDPLQCLcB/s640/Image21.png)

![alt tag](https://1.bp.blogspot.com/-TXkQyEqbvQE/VzjFmt84yLI/AAAAAAAAAvs/tAJBUYeRUeQdi7zz491lY2OKjA36RL3XgCLcB/s640/Image29.png)

![alt tag](https://1.bp.blogspot.com/-LqGrp_Crt-M/VzjFor4VDBI/AAAAAAAAAvw/PlyM94rhHhUR0_-Wb7kudygHVKAAh2KtACLcB/s640/Image31.png)

![alt tag](https://4.bp.blogspot.com/-jmefk176pi4/V9ZWFlQ6jhI/AAAAAAAAB2Q/FKWtm9D_6yYqMar5TLqPrNW6Mr35_GMGQCLcB/s640/Image17.png)


