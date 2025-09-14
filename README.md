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
    2. `cd PseudoServer`
    3. `./TaifexOrderServer`
    4. open another terminal
    5. `cd PseudoClient`
    6. `./TaifexOrderClient`
    7. Input order according to the following instruction
        |Order|Code|What you should input|what will receive|
        |--|--|--|--|
        |heart beat|0|Heartbeat|nothing|
        |test request|1|TestRequest|heart beat from server|
        |logout|5|Logout|logout message from server|
        |new order|D|NewOrder|ExecutionReport or OrderCancelReject|
        |order cancel request|F|OrderCancelRequest|ExecutionReport or OrderCancelReject|
        |order cancel|G|OrderCancel|ExecutionReport or OrderCancelReject|
        |order status|H|OrderStatus|ExecutionReport or OrderCancelReject|
        |||any other|nothing will be send. You should input another order again.|

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
