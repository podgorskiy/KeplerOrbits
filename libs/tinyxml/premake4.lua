#!lua

   project "tinyxml"
      kind "StaticLib"
      language "C++"
      files { "*.h", "*.cpp" }
      excludes { "xmltest*" }

      targetdir("lib")
	  
	  flags {  "StaticRuntime", "FloatFast" }

      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }


