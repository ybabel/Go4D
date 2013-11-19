<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: Go4D - Win32 (WCE SH3) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "c:\windows\TEMP\RSPE174.TMP" with contents
[
/nologo /W3 /I "Graph2D" /I "Graph3D" /I "Tools" /I "Platform" /I "Collision" /I "XML" /I "GUI" /I "Physic" /D "SHx" /D "SH3" /D "_SH3_" /D "NDEBUG" /D _WIN32_WCE=300 /D "WIN32_PLATFORM_PSPC" /D UNDER_CE=300 /D "UNICODE" /D "_UNICODE" /D "_LIB" /D "usefixed" /Fo"SH3Rel/" /Oxs /MC /c 
"C:\Mes documents\Mes Programmes\All Platform Projects\Go4D\Graph3D\Cube.cpp"
"C:\Mes documents\Mes Programmes\All Platform Projects\Go4D\Graph3D\Object.cpp"
"C:\Mes documents\Mes Programmes\All Platform Projects\Go4D\Graph3D\Plotter.cpp"
"C:\Mes documents\Mes Programmes\All Platform Projects\Go4D\Tools\Base.cpp"
]
Creating command line "shcl.exe @c:\windows\TEMP\RSPE174.TMP" 
Creating temporary file "c:\windows\TEMP\RSPE175.TMP" with contents
[
/nologo /out:"SH3Rel\Go4D.lib" 
".\SH3Rel\Clut.obj"
".\SH3Rel\Convert.obj"
".\SH3Rel\Drawer.obj"
".\SH3Rel\Drawer2D.obj"
".\SH3Rel\FileImage.obj"
".\SH3Rel\Filter.obj"
".\SH3Rel\Font.obj"
".\SH3Rel\Image.obj"
".\SH3Rel\ImageFormat.obj"
".\SH3Rel\Palette.obj"
".\SH3Rel\PlugDrawer.obj"
".\SH3Rel\WriteDrawer.obj"
".\SH3Rel\Bump.obj"
".\SH3Rel\Camera.obj"
".\SH3Rel\Cube.obj"
".\SH3Rel\Face.obj"
".\SH3Rel\FaceDrawer.obj"
".\SH3Rel\Filler.obj"
".\SH3Rel\Light.obj"
".\SH3Rel\Mapper.obj"
".\SH3Rel\Object.obj"
".\SH3Rel\Pixeler.obj"
".\SH3Rel\Plot.obj"
".\SH3Rel\Plotter.obj"
".\SH3Rel\Scene.obj"
".\SH3Rel\Shader.obj"
".\SH3Rel\TemplateFaceDrawer.obj"
".\SH3Rel\apstring.obj"
".\SH3Rel\Base.obj"
".\SH3Rel\Common.obj"
".\SH3Rel\Fixed.obj"
".\SH3Rel\FString.obj"
".\SH3Rel\Geom.obj"
".\SH3Rel\List.obj"
".\SH3Rel\Random.obj"
".\SH3Rel\Streams.obj"
".\SH3Rel\Tcuexc.obj"
".\SH3Rel\Tcurtti.obj"
".\SH3Rel\Tcuxhapi.obj"
".\SH3Rel\Variant.obj"
".\SH3Rel\BinFile.obj"
".\SH3Rel\Device_CE.obj"
".\SH3Rel\Files.obj"
".\SH3Rel\Viewer.obj"
".\SH3Rel\Editable3D.obj"
".\SH3Rel\EditablePhysic.obj"
".\SH3Rel\Editor.obj"
".\SH3Rel\GUI.obj"
".\SH3Rel\GUIInput.obj"
".\SH3Rel\GUILoader.obj"
".\SH3Rel\GUIOutput.obj"
".\SH3Rel\Actions.obj"
".\SH3Rel\Particle.obj"
".\SH3Rel\xmlparse.obj"
".\SH3Rel\XMLParser.obj"
".\SH3Rel\xmlrole.obj"
".\SH3Rel\xmltok.obj"
]
Creating command line "link.exe -lib @c:\windows\TEMP\RSPE175.TMP"
<h3>Output Window</h3>
Compiling...
Cube.cpp
C:\Mes documents\Mes Programmes\All Platform Projects\Go4D\Graph3D\Cube.cpp(219) : warning C4243: type cast conversion from 'class Go4D::Obsolete::TransluentFaceDrawer *' to 'class Go4D::Obsolete::AFlatFaceDrawer *' exists, but is inaccessible
Object.cpp
Plotter.cpp
Base.cpp
Generating Code...
Creating library...
<h3>
--------------------Configuration: Physic - Win32 (WCE SH3) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "c:\windows\TEMP\RSPE215.TMP" with contents
[
/nologo /W3 /I "..\Go4D" /I "..\Go4D\Tools" /I "..\Go4D\Graph2D" /I "..\Go4D\Graph3D" /I "..\Go4D\Platform" /I "..\Go4D\XML" /I "..\Go4D\GUI" /I "..\Go4D\Physic" /D "SHx" /D "SH3" /D "_SH3_" /D "NDEBUG" /D UNDER_CE=300 /D _WIN32_WCE=300 /D "WIN32_PLATFORM_PSPC" /D "UNICODE" /D "_UNICODE" /D "usefixed" /Fp"SH3Rel/Physic.pch" /YX /Fo"SH3Rel/" /Oxs /MC /c 
"C:\Mes documents\Mes Programmes\All Platform Projects\Physic\PhysicDemo.cpp"
"C:\Mes documents\Mes Programmes\All Platform Projects\Physic\PhysicTest.cpp"
]
Creating command line "shcl.exe @c:\windows\TEMP\RSPE215.TMP" 
Creating temporary file "c:\windows\TEMP\RSPE216.TMP" with contents
[
commctrl.lib coredll.lib aygshell.lib Go4D.lib gx.lib /nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"WinMainCRTStartup" /incremental:no /pdb:"SH3Rel/Physic.pdb" /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib /nodefaultlib:oldnames.lib" /out:"SH3Rel/Physic.exe" /libpath:"..\Go4D\Sh3Rel" /subsystem:windowsce,3.00 /MACHINE:SH3 
".\SH3Rel\PhysicDemo.obj"
".\SH3Rel\PhysicTest.obj"
"\Mes documents\Mes Programmes\All Platform Projects\Go4D\SH3Rel\Go4D.lib"
]
Creating command line "link.exe @c:\windows\TEMP\RSPE216.TMP"
<h3>Output Window</h3>
Compiling...
PhysicDemo.cpp
PhysicTest.cpp
Linking...



<h3>Results</h3>
Physic.exe - 0 error(s), 1 warning(s)
</pre>
</body>
</html>
