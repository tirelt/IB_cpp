# IB_cpp

## Config

Linux raspberrypi 5.10.103-v7l+ #1529 SMP Tue Mar 8 12:24:00 GMT 2022 armv7l GNU/Linux

## Download

```Bash
git https://github.com/tirelt/IB_cpp.git
cd IB_cpp
```

## Install

```Bash
mkdir tws_api
cd tws_api
wget https://interactivebrokers.github.io/downloads/twsapi_macunix.1019.04.zip
unzip twsapi_macunix.1019.04.zip
rm twsapi_macunix.1019.04.zip

```

##  Compile `TestCppClient`

The Api comes with a test app `/IBJts/samples/Cpp/TestCppClient` which packs a mmakefile.

```Bash
cd tws_api/IBJts/samples/Cpp/TestCppClient
make
```
On our system we get the issue 
```Bash
g++: error: ../../../source/cppclient/client/lib/libbid.a: No such file or directory
```
The issue has been raised and solved [here](https://groups.io/g/twsapi/topic/build_problem_with_cppclient/99520064).
We need to install another big decimal library. IB comes with their own with is not compatible with `ld`.


Edit the makefile so it uses /libbidgcc000.a instead of libbid.a
```
$(TARGET)Static:
-	$(CXX) $(CXXFLAGS) $(INCLUDES) $(BASE_SRC_DIR)/*.cpp ./*.cpp $(BASE_SRC_DIR)/lib/libbid.a -o$(TARGET)Static
+	$(CXX) $(CXXFLAGS) $(INCLUDES) $(BASE_SRC_DIR)/*.cpp ./*.cpp -lbidgcc000 -o$(TARGET)Static
```
```Bash
sudo apt-get install libintelrdfpmath-dev
make clean
make
```
On our system `apt-get` wouldn't find `libintelrdfpmath-dev` so we installed manually. We run on a `armhf` architecture. You can find all the versions [here](https://packages.debian.org/sid/libintelrdfpmath-dev).

```Bash
wget http://ftp.hk.debian.org/debian/pool/main/i/intelrdfpmath/libintelrdfpmath-dev_2.0u3-1_armhf.deb
sudo apt install ./libintelrdfpmath-dev_2.0u3-1_armhf.deb
rm libintelrdfpmath-dev_2.0u3-1_armhf.deb
dpkg -L libintelrdfpmath-dev #to check where it is installed.
```
If you're getting `/usr/bin/ld: cannot find -l<nameOfTheLibrary> you can follow [this](https://stackoverflow.com/questions/16710047/usr-bin-ld-cannot-find-lnameofthelibrary#) to manually manually create the symlink.

To run the test app
```Bash
./TestCppClientStatic <Host> <port> #default on local host:7497. You can change in main.cpp the default
```
We get the below followed by market data errors (using the Paper Trading account this is normal).

```Bash
Start of C++ Socket Client Test 0
Attempt 1 of 50
Connecting to <Host>:<Port> clientId:0
Connected to <Host>:<Port> clientId:0
Account List: <AccountNumber>
Next Valid Id: 1
```

We 
## Doc

- [api-introduction](https://ibkrcampus.com/ibkr-api-page/twsapi-doc/#api-introduction)
- [api download](https://interactivebrokers.github.io/#)
- [TwsApiCpp](https://github.com/JanBoonen/TwsApiCpp/tree/master)
- [Fix bug sample API](https://groups.io/g/twsapi/topic/build_problem_with_cppclient/99520064)
