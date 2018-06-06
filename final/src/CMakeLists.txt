#cmake -DCUDA_USE_STATIC_CUDA_RUNTIME=OFF .
cmake_minimum_required(VERSION 2.8)
project( montecarlo )
find_package( CUDA REQUIRED )
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -O3; -arch=compute_52)
cuda_add_executable( montecarlo montecarlo.cu )
target_link_libraries( montecarlo ${OpenCV_LIBS} )