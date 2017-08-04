
#add_definitions( -DCLIENT_ )
include_directories(${Boost_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
link_directories ( ${Boost_LIBRARY_DIR} )





add_executable(
 license-client 
 license-client.cpp
)


target_link_libraries(
     license-client
     tools_base
     CryptoClient
     license++_static	  
     ${EXTERNAL_LIBS}
     ${Boost_LIBRARIES}
     ${OPENSSL_LIBRARIES}     

)

#target_link_libraries(
#    license-client
#    license-generator_lib
#)
#add_dependencies( license-client private_key )

