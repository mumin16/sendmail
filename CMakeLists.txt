cmake_minimum_required(VERSION 3.0.0)


set(CMAKE_CXX_STANDARD 17)
if (WIN32)
        set(CMAKE_EXE_LINKER_FLAGS "-static") 
        set(SELECTDIR windows)
        set(SELECTSOURCE resource.rc)
        set(LIBRARIES mingw32 curl ssl crypto brotlidec-static brotlienc-static brotlicommon-static gsasl idn2 ssh2 nghttp2 z ws2_32 Wldap32 Crypt32)
elseif(UNIX)
        set(SELECTDIR linux)
        set(LIBRARIES dl pthread curl nghttp2 ssl crypto z)
elseif(APPLE)
        set(SELECTDIR macos)
        set(LIBRARIES iconv "-framework Metal -framework Carbon -framework IOKit -framework ForceFeedback -framework CoreVideo -framework Cocoa -framework AudioToolbox -framework CoreAudio" curl nghttp2 ssl crypto z)        
endif()
set(EXECUTABLE_OUTPUT_PATH "../bin")
add_compile_definitions(EXAMPLE)
include_directories(source include ../third_party/include ../third_party/include/${SELECTDIR})
link_directories(../third_party/lib/${SELECTDIR})

project(demo CXX)
add_executable(demo main.cpp 

        ${SELECTSOURCE}
  
        #../third_party/include/example.hpp

) #demo sonrası WIN32 konsolu kapar


target_link_libraries(demo ${LIBRARIES})

