# os-labs

### Build

You can use make or CMake to build the project. If you want to use make just run one:
```
$ make
```

To use CMake you need more steps than with make:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## Lab 1: Monitor

Just run a binary and relax:
```
$ ./monitor
```

## Lab 2: Signals

Firstly you need to run the server:
```
$ ./signal
```

After launch you will see something like this (server's PID and listening port):
```
Server's PID: 33084
Server listen at 0.0.0.0:1337
```

So we can connect to the server with nc to send any message (send `quit` for disconnect):
```
$ nc localhost 1337
```
Or send SIGHUP to stop the server:
```
$ kill -HUP 33084
```
