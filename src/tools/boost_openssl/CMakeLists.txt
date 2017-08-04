find_package(PkgConfig REQUIRED)
pkg_search_module(OPENSSL REQUIRED openssl)

if( OPENSSL_FOUND )
    include_directories(${OPENSSL_INCLUDE_DIRS})
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
else()
    # Error; with REQUIRED, pkg_search_module() will throw an error by it's own
endif()

#target_link_libraries(${YOUR_TARGET_HERE} ${OPENSSL_LIBRARIES})

find_package(Boost REQUIRED COMPONENTS system)

if( SYSTEM_FOUND )
    include_directories(${SYSTEM_INCLUDE_DIRS})
  
else()
    # Error; with REQUIRED, pkg_search_module() will throw an error by it's own
endif()

MESSAGE( STATUS_BOOST "BOOST_INCLUDE_DIR:     " ${Boost_INCLUDE_DIR} )



include_directories(${Boost_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
link_directories ( ${Boost_LIBRARY_DIR} )




ADD_LIBRARY(
    session_lib STATIC
    Session.cpp
)

add_executable(
 Ssl_Server
 Ssl_Server.cpp
 Session.cpp
)

target_link_libraries(
    Ssl_Server
     ${EXTERNAL_LIBS}
     ${Boost_LIBRARIES}
     ${OPENSSL_LIBRARIES}
)

add_executable(
 Ssl_Client
 Ssl_Client.cpp
)


target_link_libraries(	
	 Ssl_Client          
     ${EXTERNAL_LIBS}
     ${Boost_LIBRARIES}
     ${OPENSSL_LIBRARIES}
)






#add_dependencies( license_manager private_key )

#SET_TARGET_PROPERTIES(license_generator PROPERTIES LINK_SEARCH_START_STATIC ON)
#SET_TARGET_PROPERTIES(license_generator PROPERTIES LINK_SEARCH_END_STATIC ON)
