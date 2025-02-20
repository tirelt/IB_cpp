# IB_cpp

## Config

- Darwin Mac 24.1.0 Darwin Kernel Version 24.1.0: Thu Nov 14 18:19:02 PST 2024; root:xnu-11215.41.3~13/RELEASE_ARM64_T8132 arm64
- Linux ssd 6.8.0-53-generic #55-Ubuntu SMP PREEMPT_DYNAMIC Fri Jan 17 15:37:52 UTC 2025 x86_64 x86_64 x86_64 GNU/Linux Linux Mint 22.1 (Xia)

## Download

```Bash
git https://github.com/tirelt/IB_cpp.git
cd IB_cpp
```
The projects comes with `tws_api`, to update it: 
```Bash
mkdir tws_api
cd tws_api
wget https://interactivebrokers.github.io/downloads/twsapi_macunix.1019.04.zip #check latest version online
unzip twsapi_macunix.1019.04.zip
rm twsapi_macunix.1019.04.zip
```

## TWS

The API requires TWS which can be intalled with 
```Bash
brew install --cask trader-workstation
``` 
In Global `Configuration > API Settings` tick the option `Enable ActiveX and Socket Clients`. If you connect remotely you also need to untick `Allow connections from localhost only` at the bottom.

## Install Intel(R) Decimal Floating-Point Math Library

The API uses [Intel(R) Decimal Floating-Point Math Library](https://www.intel.com/content/www/us/en/developer/articles/tool/intel-decimal-floating-point-math-library.html). So we need to download the library, compile it and change the `makefile` of the API to make it use our library.

To download the library follow:
```Bash
wget http://www.netlib.org/misc/intel/IntelRDFPMathLib20U2.tar.gz
tar -xzf IntelRDFPMathLib20U3.tar.gz
```
To compile:
```Bash
cd LIBRARY
make CC=gcc CALL_BY_REF=0 GLOBAL_RND=0 GLOBAL_FLAGS=0 UNCHANGED_BINARY_FLAGS=0 # cf Readme.md for argument definition
```
It creates `libbid.a` then create a `lib` directory at the root of the project and put it there. Then we can simplify update the `makefile` of the API as such:
from:
```Makefile
LIB_DIR=lib
LIB_NAME=libbid.so
```
to:
```Makefile
LIB_DIR=./../../../../../lib # relative path to lib, this is the one from the IB_cpp/tws_api/IBJts/source/cppclient/client
LIB_NAME=bid
``
PS: We only build the static libbid.a. TWSApi FINALLY provides documentation to build the Intel library c.f. IB_cpp/tws_api/IBJts/source/Intel_lib_build.txt

## Compile `libTwsSocketClient.so`
To compile update the make file in `IB_cpp/tws_api/IBJts/source/cppclient/client` as per above and use `make`. This library is useful to not recompile the API everytime and just recompile the project. 

## Compile `TestCppClient`

To build the test app provide by the Api, update the makefile in `IB_cpp/tws_api/IBJts/samples/Cpp/TestCppClient` as per above.
```Bash
make TestCppClient # to recompile API
./TestCppClientStatic <Host> <port> #default on local host:7497. You can change in main.cpp the default.
```
We get the below followed by market data errors (we are using a paper trading account so this is expected).

```Bash
Start of C++ Socket Client Test 0
Attempt 1 of 50
Connecting to <Host>:<Port> clientId:0
Connected to <Host>:<Port> clientId:0
Account List: <AccountNumber>
Next Valid Id: 1
```
##  Compile `MyApp`

We created a custom `Makefile` to only recompile changed files and link the shared objects `libTwsSocketClient.so`. Simply run `make`.
Use `make run` to run the app.

## Doc

- [api-introduction](https://ibkrcampus.com/ibkr-api-page/twsapi-doc/#api-introduction)
- [api download](https://interactivebrokers.github.io/#)
- [TwsApiCpp](https://github.com/JanBoonen/TwsApiCpp/tree/master)
- [Fix bug sample API](https://groups.io/g/twsapi/topic/build_problem_with_cppclient/99520064)
