# High Frequency Stock Trading

## Sructure
```bash
.
|-- PseudoClient
|   |-- CMakeLists.txt
|   |-- build
|   |-- include
|   |   |-- ClientMessenger.h
|   |   |-- TaifexORGenerator.h
|   |   |-- TaifexOrder.h
|   |   |-- TaifexOrderClient.h
|   |   `-- TaifexOrderUtility.h
|   |-- src
|   |   |-- ClientMessenger.cpp
|   |   |-- TaifexORGenerator.cpp
|   |   |-- TaifexOrderClient.cpp
|   |   `-- TaifexOrderUtility.cpp
|-- PseudoServer
|   |-- CMakeLists.txt
|   |-- build
|   |-- include
|   |   |-- ServerMessenger.h
|   |   |-- TaifexORGenerator.h
|   |   |-- TaifexOrder.h
|   |   |-- TaifexOrderServer.h
|   |   |-- TaifexOrderUtility.h
|   |-- src
|   |   |-- ServerMessenger.cpp
|   |   |-- TaifexORGenerator.cpp
|   |   |-- TaifexOrderServer.cpp
|   |   |-- TaifexOrderUtility.cpp
|   |   `-- MarketSimulator.cpp   (fake quotes + matching engine)
|-- common
|   `-- include
|       `-- ConsoleUI.h           (shared coloured logging / FIX pretty printer)
|-- README.md
`-- docker
    |-- Dockerfile
    `-- docker-compose.yml
```

## Setup and Use
### if you have docker
1. Setup Environment
    1. open the windows terminal and clone the repo into the place you want: `git clone -b develop git@github.com:wang-hsiu-cheng/HFST.git`
    2. `cd ~/HFST/docker`
    3. `docker compose up`
    4. open a new terminal & `docker exec -it hfst /bin/bash`
2. Use
    1. open one terminal
    2. `cd PseudoServer/build`
    3. `./TaifexOrderServer`
    4. open another terminal
    5. `cd PseudoClient/build`
    6. `./TaifexOrderClient`
    7. After `Logon accepted`, type commands at the `>` prompt (see below)

### Roles
- **PseudoServer** is a software exchange. It streams fake quotes for `2330`, `2317`, `2454` and `0050` as FIX `35=W` (MarketDataSnapshotFullRefresh), runs a simple limit-order matcher (TWSE tick sizes, ±10% price limit) and answers with `35=8` ExecutionReport / `35=9` OrderCancelReject. It prints a quote board every few seconds.
- **PseudoClient** models the FPGA front end. It only encodes what you type (the "Order Out Register") into FIX packets and decodes what comes back. You play the trading algorithm; the client makes no trading decisions.

### Client commands
|Command|FIX|Description|
|--|--|--|
|`buy <symbol> <qty> <price>`|D|new limit buy order (qty in shares, 1 lot = 1000)|
|`sell <symbol> <qty> <price>`|D|new limit sell order|
|`cancel <ClOrdID>`|F|cancel an order|
|`replace <ClOrdID> <qty> <price>`|G|change quantity / price (`0` keeps the current value)|
|`status <ClOrdID>`|H|order status request|
|`quotes`|-|latest quotes received from `35=W`|
|`orders`|-|order table rebuilt from the `35=8` / `35=9` reports|
|`watch on\|off`|-|print every quote update|
|`raw on\|off`|-|show every packet as a FIX tag table|
|`verbose on\|off`|-|debug output|
|`hb` / `test` / `logout`|0 / 1 / 5|Heartbeat / TestRequest / Logout (`Heartbeat`, `TestRequest`, `Logout` also work)|
|`wait <sec>`|-|pause, useful for scripted input|
|`help`|-|command list|

Example session:
```
> quotes
> buy 2330 1000 610      # crosses the ask -> NEW, then FILLED at the ask
> buy 2330 1000 595      # rests -> NEW, FILLED later if the market trades down to it
> replace C000002 2000 0 # REPLACED, quantity 2000, price kept
> cancel C000002         # CANCELED (or 35=9 reject if it already filled)
> orders
> logout
```
Scripted run (stdin closed -> automatic logout):
`printf "buy 2330 1000 610\nwait 2\norders\n" | ./TaifexOrderClient`

### Command line options
|Program|Option|Description|
|--|--|--|
|both|`--raw`|print every FIX packet as a tag table (tag, name, value, meaning)|
|both|`--verbose`|debug output, including every `35=W` packet and decoder steps|
|both|`--no-color`|disable ANSI colours (also off automatically when output is not a terminal)|
|both|`--port N`|port (default 9000)|
|client|`--host IP`|server address (default 127.0.0.1)|
|server|`--tick-ms N`|market data interval in ms (default 1000)|
|server|`--board-sec N`|quote board interval in seconds, 0 = off (default 5)|
|server|`--sessions N`|number of client sessions to accept (default 1)|

### if you don't have docker
1. IDK ¯\_( ͡° ͜ʖ ͡°)_/¯
2. I'll find the solution as fast as possible.

## How to Edit
### Introduction to File Structure
- ClientMessenger: 
- TaifexORGenerator: 
- TaifexOrder: 
- TaifexOrderClient: 
- TaifexOrderUtility: 
### Compile
- `cd /build`
- If it's your first time to build the project: `cmake ..`
- `make`
- slove every syntax error in your program

### Coding Rule (recommend)
- function: 
    ```c++
    void Function1()
    {

    }
    ```
- macro:
    ```c++
    #define MACRO1 0
    ``` 
- global variable: 
    ```c++
    void GlobalVariable1;
    ```
- pass in function variable:
    ```c++
    void Function1(auto _passInVariable1)
    {

    }
    ```
- local variable: 
    ```c++
    void Function1()
    {
        auto localVariable1;
    }
    ```
- public variable: 
    ```c++
    public:
        auto publicVariable1;
    ```
- private variable & protected: 
    ```c++
    private:
        auto private_variable_1;
    ```
- typedef struct: 
    ```c++
    typedef struct _Struct_1
    {

    } Struct_1;
    ```
- condition & loop:
    ```c++
    if () {

    } else {

    }
    ```
