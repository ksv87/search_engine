[![CI](https://github.com/ksv87/search_engine/actions/workflows/build.yml/badge.svg)](https://github.com/ksv87/search_engine/actions/workflows/build.yml)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=ksv87_search_engine&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=ksv87_search_engine)
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=ksv87_search_engine&metric=coverage)](https://sonarcloud.io/summary/new_code?id=ksv87_search_engine)
![Platforms](https://img.shields.io/badge/platform-linux%20%7C%20windows%20%7C%20mac%20os-deepgreen) 
![License](https://img.shields.io/badge/license-MIT-deepgreen)

# search_engine

This is a simple search engine for corporation use

# the stack of technologies used

C++ 17 - language standard</br>
CMake - build system</br>
Conan - package manager</br>
Python 3 - for conan </br>
https://github.com/nlohmann/json - library for working with JSON </br>
https://github.com/google/googletest - library for testing </br>
https://www.boost.org/doc/libs/1_66_0/doc/html/boost_asio/reference/thread_pool.html - library for working with multithreading

# build from source

``` bash
git clone https://github.com/ksv87/search_engine.git
cd search_engine
mkdir build
cd build
conan install .. --build=missing
cmake ..
cmake --build .
```

for enable testing
``` bash
cmake -DENABLE_TESTS=ON ..
cmake --build .
```

for change build type
``` bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

# configuration files

.\resources\config.json - main configuration file </br>
``` bash
field max_responses - count of max responses to request
field files - files to indexing
```
.\resources\requests.json - file with requests </br>
.\resources\answers.json - file with results </br>

# run tests

``` bash
./bin/search_engine_test
```

# run

``` bash
./bin/search_engine
```
