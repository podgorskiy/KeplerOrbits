#!lua
 
newoption {
	trigger = "windows",
	description = "windows",
}


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
  	
		
	links { "tinyxml",  "bgfx", "example-common" }	
  	
	if _OPTIONS["windows"] then
		links {
			"Comdlg32",
			"psapi",
			"opengl32"
		}
	else
		links {
			"GL",
			"X11",
			"dl",
			"pthread"
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
	
