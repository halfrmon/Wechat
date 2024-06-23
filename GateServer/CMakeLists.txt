cmake_minimum_required(VERSION 3.10)
project(GateServer LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置 Protobuf 和 gRPC 的路径
set(GRPC_DIR /usr)
set(PROTOBUF_DIR /usr)

# 手动指定 JSON 库的包含路径和链接库
set(JsonCpp_INCLUDE_DIRS "/usr/include/json")
set(JsonCpp_LIBRARIES "jsoncpp")

# 包含和链接 JSON 库
include_directories(${JsonCpp_INCLUDE_DIRS})


# 找到 Protobuf 包
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})


# 找到 gRPC 包
find_package(gRPC REQUIRED)
include_directories(${gRPC_INCLUDE_DIRS})

# 包含 Boost.Filesystem 包
find_package(Boost REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})

# 生成的 Protobuf 和 gRPC 文件
set(GENERATED_PROTO_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/message.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/message.grpc.pb.cc
)
#直接手动设置redis的安装目录
set(HIREDIS_INCLUDE_DIRS "/usr/include/hiredis")
set(HIREDIS_LIBRARLIES "hiredis")
include_directories(${HIREDIS_INCLUDE_DIRS})

# 手动包含 MySQL 头文件目录
# 查找 MySQL Connector/C++ 库
include_directories(/usr/include/mysql /usr/include/cppconn)






# 源文件列表
set(SOURCES
    main.cpp
    VerifyGrpcClient.cpp
    LogicSystem.cpp
    ConfigMgr.cpp
    CServer.cpp
    VerifyGrpcClient.cpp
    AsioIOServicePool.cpp
    RedisMgr.cpp
    MysqlMgr.cpp
    MysqlDao.cpp
    HttpConnection.cpp
    ${GENERATED_PROTO_FILES}
)

# 包含当前目录中的头文件
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接库
target_link_libraries(${PROJECT_NAME}
    ${Protobuf_LIBRARIES}
    Boost::filesystem
    gRPC::grpc++
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    mysqlcppconn
    
)
target_link_libraries(${PROJECT_NAME} ${JsonCpp_LIBRARIES} ${HIREDIS_LIBRARLIES})

# 设置 RPATH 以便在运行时找到库
set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

