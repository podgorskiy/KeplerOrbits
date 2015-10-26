#[KeplerOrbits](https://github.com/podgorskiy/KeplerOrbits) - Tool that calculates positions of celestial bodies using their orbital elements.
============================================================================

Provides view of the solar system from different sides and provides view from the observer on Earth (you need to specify your location coordinates).

Runs on multiple platforms, including Asm.JS (Emscripten)

[Web Demo](http://podgorskiy.com/KeplerOrbits/KeplerOrbits.html)

#Some info:

Emscripten cflags: -s TOTAL_MEMORY=50000000 --llvm-lto 1 -s DISABLE_EXCEPTION_CATCHING=1 --closure 1 -s NO_FILESYSTEM=1  -s NO_EXIT_RUNTIME=1 
Emscripten link flags: -s TOTAL_MEMORY=50000000 -s EXPORTED_FUNCTIONS="['_main', '_resizeModule', '_setGeoLocation']" 

#Links

http://ssd.jpl.nasa.gov/?ss_outer

http://ssd.jpl.nasa.gov/?horizons

http://www.fourmilab.ch/cgi-bin/Yoursky

Orbital elements of celestial bodies were taken from here: 
http://nssdc.gsfc.nasa.gov/planetary/factsheet/ 

License
-------

KeplerOrbits is licensed under the MIT License, see LICENSE for more information.
