#!lua

   project "zlib"
      kind "StaticLib"
      language "C"
      files { "*.h", "*.c" }

      targetdir("lib")

	  flags {  "StaticRuntime", "FloatFast" }
	  
      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }


