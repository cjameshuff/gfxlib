Miscellaneous image processing, vector math, and other graphics and simulation related code.


A high level of C++11 support is required, and some code uses the Clang OpenCL-style vector extension. Suggested toolchain:
Minimum of Clang 3.2 (earlier may work, but is not guaranteed)
libc++ (*not* libstdc++)

At the time of this writing, Clang 3.2 isn't in many repositories (not for Mint Linux, anyway), and will likely have to be installed manually. Follow the directions here:
http://llvm.org/releases/3.2/docs/GettingStarted.html

Target a separate prefix directory to avoid problems with the package system:
`../configure --prefix=/llvm-svn --enable-optimized`

Add some environment variables to your login files and you can use an up to date version now, and remove it when a package becomes available.


libc++ is also not commonly provided at this time. Get it from here: http://libcxx.llvm.org/


mkdir libcxx_build

`cd libcxx
cd ../libcxx_build 
CC=clang CXX=clang++ cmake -G "Unix Makefiles" -DLIBCXX_CXX_ABI=libsupc++ -DLIBCXX_LIBSUPCXX_INCLUDE_PATHS="/usr/include/c++/4.7/;/usr/include/c++/4.7/x86_64-linux-gnu/" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/llvm-svn ../libcxx
make
sudo make install
`

Dynamic linking will cause problems. Setting LD_LIBRARY_PATH to include /llvm-svn/lib will work, but is obviously not ideal.


