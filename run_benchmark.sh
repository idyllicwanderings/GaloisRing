cd src/
python3 run_bm_gr.py 
cd ../
cmake -B build 
cmake --build build
./build/grbenchmark