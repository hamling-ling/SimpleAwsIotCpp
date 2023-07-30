# SimpleAwsIotCpp
Simple and practical implementation of AWS IoT Device SDK C++ v2.

# Prerequisites

Linux (Ubuntu20.04)

# Build AWS IoT Device SDK C++ v

Build the SDK and install to artifact directory.

```
cd vendor/aws
mkdir build
cmake -DCMAKE_INSTALL_PREFIX=../../../artifact \
-DCMAKE_BUILD_TYPE="Debug" \
-DBUILD_SHARED_LIBS=ON \
../aws-iot-device-sdk-cpp-v2

cmake --build . --target install
```

In a case of Jetson Orin Nano, you may edit toolchain/jetson-l4t-toolchain.cmake and execute following command.
```
cd vendor/aws
mkdir build
cmake -DCMAKE_INSTALL_PREFIX=../../../artifact \
-DCMAKE_BUILD_TYPE="Debug" \
-DCMAKE_TOOLCHAIN_FILE=./jetson-l4t-toolchain.cmake \
-DUSE_OPENSSL=ON \
-DBUILD_SHARED_LIBS=ON \
../aws-iot-device-sdk-cpp-v2
```

# Write .env File

```
nano .env
```

# Build App

```
cd src
./build.sh -t iotapp
```

# Run App

```
cd src
./run.sh -t iotapp
```

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
