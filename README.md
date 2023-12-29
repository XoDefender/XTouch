# Build dependencies:
gcc10-c++
gcc-c++
freetype2-devel
tk
tk-devel
libtk
fontconfig-devel
libMesaOpenCl
libGLW-devel
libGLEW-devel
libgtkmm3-devel
libmysqlcppconn-devel
freeglut
libglfw3-devel
tklib
MySQL
libpcre-devel
libmysqlclient-devel
cmake
git

# Dependencies installation
sudo apt-get install gcc10-c++ gcc-c++ freetype2-devel tk tk-devel libtk fontconfig-devel libMesaOpenCL libGLEW-devel libgtkmm3-devel libmysqlcppconn-devel freeglut libglfw3-devel tklib MySQL libpcre-devel libmysqlclient-devel cmake

# OpenCascade installation:
1. Execute "git clone https://github.com/Open-Cascade-SAS/OCCT" in OCCT active dirrectory
2. Add "cmake_policy (SET CMP0054 NEW)" in CMakeLists.txt
3. In the cloned OCCT folder create the "build" dir and execute "cmake ../" , "sudo make install"