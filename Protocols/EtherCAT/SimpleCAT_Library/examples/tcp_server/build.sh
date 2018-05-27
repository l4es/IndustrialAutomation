#============================
# BUILD EXAMPLE APPLICATIONS
#============================

TCP_SERVER_EXAMPLE_PATH="$(pwd)"

cd ${TCP_SERVER_EXAMPLE_PATH}/control_loop
mkdir build
cd build
cmake ..
make -j3

cd ${TCP_SERVER_EXAMPLE_PATH}/data_logger
mkdir build
cd build
cmake ..
make -j3


