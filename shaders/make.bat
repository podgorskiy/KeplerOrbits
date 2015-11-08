call ..\tools\bin\windows\shadercDebug.exe -f src/vs_simple.sc -i src/ --platform asm.js --bin2c -o vs_simple.h --type vertex
call ..\tools\bin\windows\shadercDebug.exe -f src/fs_colorLit.sc -i src/ --platform asm.js --bin2c -o fs_colorLit.h --type fragment
call ..\tools\bin\windows\shadercDebug.exe -f src/vs_icon.sc -i src/ --platform asm.js --bin2c -o vs_icon.h --type vertex
call ..\tools\bin\windows\shadercDebug.exe -f src/fs_icon.sc -i src/ --platform asm.js --bin2c -o fs_icon.h --type fragment