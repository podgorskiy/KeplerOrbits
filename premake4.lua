#!lua
 
newoption {
	trigger = "em",
	description = "emscripten",
}


if _OPTIONS["em"] then
	if not os.getenv("EMSCRIPTEN") then
		print("Set EMSCRIPTEN enviroment variables.")
	end
	premake.gcc.cc   = "$(EMSCRIPTEN)/emcc"
	premake.gcc.cxx  = "$(EMSCRIPTEN)/em++"
	premake.gcc.ar   = "$(EMSCRIPTEN)/emar"
end

BGFX_DIR = path.getabsolute("libs/bgfx")
BX_DIR = path.getabsolute(path.join(BGFX_DIR, "../bx"))
	
solution "KeplerOrbits"
	configurations {
		"Debug",
		"Release",
	}

	if _ACTION == "xcode4" then
		platforms {
			"Universal",
		}
	else
		platforms {
			"x32",
			"x64",
--			"Xbox360",
			"Native", -- for targets where bitness is not specified
		}
	end
	
	language "C++"
	
	location ("projects/" .. _ACTION)
	
	startproject "KeplerOrbits"
	
	flags { "StaticRuntime",  "FloatFast"}	
	
	
	if _OPTIONS["em"] then
		buildoptions  { 
			"-U_WIN32", 
			"-U__MINGW32__",
			"-isystemC:/Emscripten/emscripten/1.34.1/system/include",
			"-isystemC:/Emscripten/emscripten/1.34.1/system/include/libc",
			"-isystemC:/Emscripten/emscripten/1.34.1/system/include/emscripten",
			"-Wunused-value",
			"-Wundef"
		}
		defines {"__STDC_CONSTANT_MACROS", "__EMSCRIPTEN__", "EMSCRIPTEN", "__unix__"}	
	
	end
	
	configuration {"Debug"}	
		flags { "Symbols" }
	configuration {"Release"}
		flags { "Optimize" }	

	--targetextension ".bc"

function copyLib()
end

dofile "libs/bgfx/scripts/bgfx.lua"
dofile "libs/bgfx/scripts/example-common.lua"	
	
bgfxProject("", "StaticLib", {})	
	
include "libs/tinyxml"
	
project "KeplerOrbits"	
	kind "ConsoleApp"	
	language "C++"	
	files { "src/**.h", "src/**.cpp" }	

	targetdir("")	

	includedirs {	
		"src",	
		"libs/eigen",	
		"libs/boost",	
		"libs/tinyxml",	
		"libs/glm",			
		"libs/bgfx/3rdparty",
		"libs/bx/include",
		"libs/bgfx/include",	
		"libs/bx/include",
		"libs/bgfx/examples/common"
	}	
  	
		
	links { 
		"tinyxml",  "opengl32",  "bgfx", "example-common", "psapi", "Comdlg32" }	
  	
	if _OPTIONS["em"] then
		buildoptions  { 
			"-mno-sse"
		}
	end

	configuration {"Debug"}	
		debugdir "."	
		debugargs { "" }	
		defines { "DEBUG",  "_LIB" }	
		flags { "Symbols"}	

	configuration {"Release"}	
		defines { "NDEBUG",  "_LIB" }	
		flags { "Optimize" }
	
