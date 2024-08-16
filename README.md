# IB_cpp

## Config

Linux raspberrypi 5.10.103-v7l+ #1529 SMP Tue Mar 8 12:24:00 GMT 2022 armv7l GNU/Linux
VSCode connected in SSH 

## Download

```Bash
git https://github.com/tirelt/IB_cpp.git
cd IB_cpp
```
The projects comes with `tws_api`, to update it: 
```Bash
mkdir tws_api
cd tws_api
wget https://interactivebrokers.github.io/downloads/twsapi_macunix.1019.04.zip
unzip twsapi_macunix.1019.04.zip
rm twsapi_macunix.1019.04.zip

```

##  Compile `TestCppClient`

The Api comes with a test app `/IBJts/samples/Cpp/TestCppClient` which packs a makefile.

```Bash
cd tws_api/IBJts/samples/Cpp/TestCppClient
make
```
On our system we get the issue 
```Bash
g++: error: ../../../source/cppclient/client/lib/libbid.a: No such file or directory
```
The issue has been raised and solved [here](https://groups.io/g/twsapi/topic/build_problem_with_cppclient/99520064).
We need to install another big decimal library. IB comes with their own which is not compatible with `ld`.

We will install `libintelrdfpmath-dev` (big decimal library by intel) instead:

1. Edit the makefile so it uses /libbidgcc000.a instead of libbid.a.

```
$(TARGET)Static:
-	$(CXX) $(CXXFLAGS) $(INCLUDES) $(BASE_SRC_DIR)/*.cpp ./*.cpp $(BASE_SRC_DIR)/lib/libbid.a -o$(TARGET)Static
+	$(CXX) $(CXXFLAGS) $(INCLUDES) $(BASE_SRC_DIR)/*.cpp ./*.cpp -lbidgcc000 -o$(TARGET)Static
```
2. install the library

```Bash
sudo apt-get install libintelrdfpmath-dev
make clean
make
```
On our system `apt-get` doesn't find `libintelrdfpmath-dev` so we install it manually. You can find all the versions [here](https://packages.debian.org/sid/libintelrdfpmath-dev). We run on a `armhf` architecture so we downaload and install the appropriete version using:

```Bash
wget http://ftp.hk.debian.org/debian/pool/main/i/intelrdfpmath/libintelrdfpmath-dev_2.0u3-1_armhf.deb #manual download
sudo apt install ./libintelrdfpmath-dev_2.0u3-1_armhf.deb
rm libintelrdfpmath-dev_2.0u3-1_armhf.deb
dpkg -L libintelrdfpmath-dev #to check where it is installed.
```
If you're getting `/usr/bin/ld: cannot find -l<nameOfTheLibrary>` you can follow [this](https://stackoverflow.com/questions/16710047/usr-bin-ld-cannot-find-lnameofthelibrary#) to manually manually create the symlink.

To run the test app
```Bash
./TestCppClientStatic <Host> <port> #default on local host:7497. You can change in main.cpp the default
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

##  Compile `libTwsSocketClient.so`

Our app uses `libTwsSocketClient.so`, to compile it use the `Makefile` in tws_api/IBJts/source/cppclient/client/source.
The `Makefile` needs to be updated as per below to also use `libbidgcc000` instead of the big decimal library provided by IB. The file in the repo is already updated.

##  Compile `MyApp`

We created a custom `Makefile` to only recompile changed files and link the shared objects `libTwsSocketClient.so`. Simply run `make`.
Use `make run` to run the app.

## Doc

- [api-introduction](https://ibkrcampus.com/ibkr-api-page/twsapi-doc/#api-introduction)
- [api download](https://interactivebrokers.github.io/#)
- [TwsApiCpp](https://github.com/JanBoonen/TwsApiCpp/tree/master)
- [Fix bug sample API](https://groups.io/g/twsapi/topic/build_problem_with_cppclient/99520064)
