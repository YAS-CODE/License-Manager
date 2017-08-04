
if(WIN32)
ADD_LIBRARY(
    tools_base STATIC
    CryptoHelper.cpp
    win/CryptoHelperWindows.cpp
)
else(WIN32)
ADD_LIBRARY(
    tools_base STATIC
    CryptoHelper.cpp
    linux/CryptoHelperLinux.cpp
)

ADD_LIBRARY(
    Crypto STATIC
    linux/base64.cpp
    linux/Crypto.cpp
)

#add_definitions(-DPSEUDO_CLIENT)


ADD_LIBRARY(
    CryptoClient STATIC
    linux/base64.cpp
    linux/Crypto.cpp
)
target_compile_definitions(CryptoClient PUBLIC -DPSEUDO_CLIENT)
target_link_libraries(
     CryptoClient
     ${EXTERNAL_LIBS}
)
ENDIF(WIN32)

target_link_libraries(
     tools_base
     ${EXTERNAL_LIBS}
)

target_link_libraries(
     Crypto
     ${EXTERNAL_LIBS}
)



target_link_libraries(
     CryptoClient
     ${EXTERNAL_LIBS}
)
