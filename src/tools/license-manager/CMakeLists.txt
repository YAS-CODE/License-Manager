
include_directories(${Boost_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
link_directories ( ${Boost_LIBRARY_DIR} )


MESSAGE( STATUS_BOOST "BOOST_INCLUDE_DIR:     " ${Boost_INCLUDE_DIR} )

ADD_LIBRARY(
    license-generator_lib STATIC
    license-generator.cpp
)

target_link_libraries(
     license-generator_lib
     tools_base
     license++_static
     Crypto
	 $<$<CONFIG:Debug>:${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG}>
     $<$<NOT:$<CONFIG:Debug>>:${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE}>	 
	 $<$<CONFIG:Debug>:${Boost_SYSTEM_LIBRARY_DEBUG}>
     $<$<NOT:$<CONFIG:Debug>>:${Boost_SYSTEM_LIBRARY_RELEASE}>	 
     ${EXTERNAL_LIBS}
     ${Boost_LIBRARIES}
)

add_executable(
 license_manager
 license-manager-main.cpp
 ProductManager.cpp
)

add_executable(
 license_config
 product-manager-main.cpp
 ProductManager.cpp
)
target_link_libraries(
    license_config    
    license-generator_lib
    tools_base
     license++_static
     Crypto
	 $<$<CONFIG:Debug>:${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG}>
     $<$<NOT:$<CONFIG:Debug>>:${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE}>	 
	 $<$<CONFIG:Debug>:${Boost_SYSTEM_LIBRARY_DEBUG}>
     $<$<NOT:$<CONFIG:Debug>>:${Boost_SYSTEM_LIBRARY_RELEASE}>	 
     ${EXTERNAL_LIBS}
     ${Boost_LIBRARIES}
     ${OPENSSL_LIBRARIES}
)



target_link_libraries(
    license_manager    
    license-generator_lib
    tools_base
     license++_static
     Crypto
	 $<$<CONFIG:Debug>:${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG}>
     $<$<NOT:$<CONFIG:Debug>>:${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE}>	 
	 $<$<CONFIG:Debug>:${Boost_SYSTEM_LIBRARY_DEBUG}>
     $<$<NOT:$<CONFIG:Debug>>:${Boost_SYSTEM_LIBRARY_RELEASE}>	 
     ${EXTERNAL_LIBS}
     ${Boost_LIBRARIES}
     ${OPENSSL_LIBRARIES}
)
add_dependencies( license_manager private_key )

#SET_TARGET_PROPERTIES(license_generator PROPERTIES LINK_SEARCH_START_STATIC ON)
#SET_TARGET_PROPERTIES(license_generator PROPERTIES LINK_SEARCH_END_STATIC ON)
