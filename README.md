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
2. ![изображение](https://github.com/XoDefender/XTouch/assets/90552397/a1b5379d-b2c8-429d-9889-5985ac744c5d)
3. ![изображение](https://github.com/XoDefender/XTouch/assets/90552397/4a90c463-ff81-477a-9fe0-6e1530d8691f)
4. ![изображение](https://github.com/XoDefender/XTouch/assets/90552397/89fb5a25-7516-4dbf-b213-b870332ca88b)
5. ![изображение](https://github.com/XoDefender/XTouch/assets/90552397/d1b60d89-0252-4e59-80dc-cdf57e1b222f)




6. Add "cmake_policy (SET CMP0054 NEW)" in CMakeLists.txt
7. In the cloned OCCT folder create the "build" dir and execute "cmake ../" , "sudo make install"

# Demo
![изображение](https://github.com/XoDefender/XTouch/assets/90552397/9b154f22-515c-4d9a-aaf3-588c2aa525e2)
