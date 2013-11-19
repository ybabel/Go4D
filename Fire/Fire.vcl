<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: Fire - Win32 (WCE SH3) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "c:\windows\TEMP\RSP5080.TMP" with contents
[
/nologo /W3 /O2 /I "..\Go4D" /I "..\Go4D\Tools" /I "..\Go4D\Graph2D" /I "..\Go4D\Graph3D" /I "..\Go4D\Platform" /I "..\Go4D\XML" /I "..\Go4D\GUI" /I "..\Go4D\Physic" /D "NDEBUG" /D "SHx" /D "SH3" /D "_SH3_" /D UNDER_CE=300 /D _WIN32_WCE=300 /D "WIN32_PLATFORM_PSPC" /D "UNICODE" /D "_UNICODE" /D "usefixed" /Fo"SH3Rel/" /Oxs /MC /c 
"C:\Mes documents\Mes Programmes\All Platform Projects\Fire\FireDemo.cpp"
"C:\Mes documents\Mes Programmes\All Platform Projects\Fire\FireTest.cpp"
]
Creating command line "shcl.exe @c:\windows\TEMP\RSP5080.TMP" 
Creating temporary file "c:\windows\TEMP\RSP5081.TMP" with contents
[
Go4D.lib aygshell.lib commctrl.lib coredll.lib gx.lib /nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"WinMainCRTStartup" /incremental:no /pdb:"SH3Rel/Fire.pdb" /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib /nodefaultlib:oldnames.lib" /out:"SH3Rel/Fire.exe" /libpath:"..\Go4D\Sh3Rel" /subsystem:windowsce,3.00 /MACHINE:SH3 
".\SH3Rel\FireDemo.obj"
".\SH3Rel\FireTest.obj"
"\Mes documents\Mes Programmes\All Platform Projects\Go4D\SH3Rel\Go4D.lib"
]
Creating command line "link.exe @c:\windows\TEMP\RSP5081.TMP"
<h3>Output Window</h3>
Compiling...
FireDemo.cpp
C:\Mes documents\Mes Programmes\All Platform Projects\Fire\FireDemo.cpp(104) : warning C4244: '=' : conversion from 'const double' to 'int', possible loss of data
FireTest.cpp
Generating Code...
Linking...



<h3>Results</h3>
Fire.exe - 0 error(s), 1 warning(s)
</pre>
</body>
</html>
