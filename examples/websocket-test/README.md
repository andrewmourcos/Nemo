# Dependencies
## Ubuntu
```bash
sudo apt-get install libboost-all-dev
```
## Mac
Homebrew should be fine.

# Build code
```bash
mkdir build 
cd build
cmake ..
```

# Run code
```bash
./websocket_test echo.websocket.org 443 "Hello, world!"
```