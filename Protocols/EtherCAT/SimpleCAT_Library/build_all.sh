#===============
# BUILD LIBRARY
#===============
SIMPLECAT_PATH="$(pwd)"

cd ${SIMPLECAT_PATH}
mkdir build
cd build
cmake ..
make


#============================
# BUILD EXAMPLE APPLICATIONS
#============================
SIMPLECAT_EXAMPLE_PATH=${SIMPLECAT_PATH}/examples

cd ${SIMPLECAT_EXAMPLE_PATH}/beckhoff_analog_read
mkdir build
cd build
cmake ..
make

cd ${SIMPLECAT_EXAMPLE_PATH}/beckhoff_analog_read_write
mkdir build
cd build
cmake ..
make

cd ${SIMPLECAT_EXAMPLE_PATH}/beckhoff_quad_encoder_read
mkdir build
cd build
cmake ..
make

cd ${SIMPLECAT_EXAMPLE_PATH}/beckhoff_ssi_encoder
mkdir build
cd build
cmake ..
make

cd ${SIMPLECAT_EXAMPLE_PATH}/elmo_gold_whistle
mkdir build
cd build
cmake ..
make

#cd ${SIMPLECAT_EXAMPLE_PATH}/technosoft_ipos3604
#mkdir build
#cd build
#cmake ..
#make

cd ${SIMPLECAT_EXAMPLE_PATH}/shared_memory/control_loop
mkdir build
cd build
cmake ..
make

cd ${SIMPLECAT_EXAMPLE_PATH}/shared_memory/data_logger
mkdir build
cd build
cmake ..
make

#cd ${SIMPLECAT_EXAMPLE_PATH}/tcp_server/control_loop
#mkdir build
#cd build
#cmake ..
#make

#cd ${SIMPLECAT_EXAMPLE_PATH}/tcp_server/data_logger
#mkdir build
#cd build
#cmake ..
#make


