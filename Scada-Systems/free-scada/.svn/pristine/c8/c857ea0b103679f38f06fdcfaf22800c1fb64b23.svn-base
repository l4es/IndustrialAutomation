
Visualizatorps.dll: dlldata.obj Visualizator_p.obj Visualizator_i.obj
	link /dll /out:Visualizatorps.dll /def:Visualizatorps.def /entry:DllMain dlldata.obj Visualizator_p.obj Visualizator_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del Visualizatorps.dll
	@del Visualizatorps.lib
	@del Visualizatorps.exp
	@del dlldata.obj
	@del Visualizator_p.obj
	@del Visualizator_i.obj
