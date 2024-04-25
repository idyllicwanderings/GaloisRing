mkdir build && cd build
cmake ..
make

# TODO
cmake -S . -B build
cmake --build build
cd build && ctest