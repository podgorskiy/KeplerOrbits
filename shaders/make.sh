#bash
../tools/bin/linux/shadercRelease -f src/vs_simple.sc -i src/ --platform asm.js --bin2c -o vs_simple.h --type vertex
../tools/bin/linux/shadercRelease -f src/fs_colorLit.sc -i src/ --platform asm.js --bin2c -o fs_colorLit.h --type fragment
../tools/bin/linux/shadercRelease -f src/vs_icon.sc -i src/ --platform asm.js --bin2c -o vs_icon.h --type vertex
../tools/bin/linux/shadercRelease -f src/fs_icon.sc -i src/ --platform asm.js --bin2c -o fs_icon.h --type fragment