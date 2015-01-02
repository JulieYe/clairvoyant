1. Download boost_1_51_0.tar.bz2.

2. In the directory where you want to put the Boost installation, execute
$ tar --bzip2 -xf /path/to/boost_1_51_0.tar.bz2

3. Easy build and install,
$ cd path/to/boost_1_51_0
$ ./bootstrap.sh
$ ./b2 install
$ sudo ldconfig

4. Compile and run hello.cpp
$ g++ -I/usr/local/include/ -ohello hello.cpp -lboost_unit_test_framework -L/usr/local/lib
$ ./hello
