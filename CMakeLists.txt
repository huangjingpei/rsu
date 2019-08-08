cmake_minimum_required(VERSION 3.8)

project(rsu LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)
#ADD_DEFINITIONS(-g -isystem /opt/mywork/home/ubuntu/mywork/thirdparty/boost_1_69_0 -DDEBUG -D_WEBSOCKETPP_CPP11_STL_ -DNDEBUG -DUSE_UDEV -DUSE_AURA=1 -DUSE_NSS_CERTS=1 -DUSE_OZONE=1 -DFULL_SAFE_BROWSING -DSAFE_BROWSING_CSD -DSAFE_BROWSING_DB_LOCAL -DCHROMIUM_BUILD -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D_FORTIFY_SOURCE=2 -DCR_SYSROOT_HASH=e7c53f04bd88d29d075bfd1f62b073aeb69cbe09 -DNDEBUG -DNVALGRIND -DDYNAMIC_ANNOTATIONS_ENABLED=0 -DWEBRTC_ENABLE_PROTOBUF=0 -DWEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE -DRTC_ENABLE_VP9 -DHAVE_SCTP -DWEBRTC_USE_H264 -DWEBRTC_LIBRARY_IMPL -DWEBRTC_NON_STATIC_TRACE_EVENT_HANDLERS=0 -DWEBRTC_POSIX -DWEBRTC_LINUX -DABSL_ALLOCATOR_NOTHROW=1 -DHAVE_SCTP)
ADD_DEFINITIONS(-g)
# Set version number.
set(broadcaster_VERSION_MAJOR 0)
set(broadcaster_VERSION_MINOR 1)

# C++ standard requirements.
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
ADD_DEFINITIONS("-std=c++11")

# Add some compile flags to our source files.
set_source_files_properties(${SOURCE_FILES}
	PROPERTIES COMPILE_FLAGS -Wall -Wextra -Wpedantic)
link_directories(${PROJECT_SOURCE_DIR}/deps/websocketpp/lib)
link_directories(${PROJECT_SOURCE_DIR}/deps/webrtc/out/m74/obj)

# Create target.
add_executable(${PROJECT_NAME} ${SOURCES})

# Extra libs required in order to use Apple media devices.
if(APPLE)
	find_library(APPLICATION_SERVICES ApplicationServices)
	find_library(AUDIO_TOOLBOX AudioToolbox)
	find_library(AV_FOUNDATION AVFoundation)
	find_library(CORE_AUDIO CoreAudio)
	find_library(CORE_FOUNDATION Foundation)
	find_library(CORE_MEDIA CoreMedia)
	find_library(CORE_VIDEO CoreVideo)
	find_library(CORE_SERVICES CoreServices)

	target_link_libraries(${PROJECT_NAME} PUBLIC
		${APPLICATION_SERVICES}
		${AUDIO_TOOLBOX}
		${AV_FOUNDATION}
		${CORE_AUDIO}
		${CORE_FOUNDATION}
		${CORE_MEDIA}
		${CORE_VIDEO}
		${CORE_SERVICES}
		stdc++
	)
endif(APPLE)

target_sources(${PROJECT_NAME} PRIVATE
	src/Broadcaster.cpp
	src/main.cpp
	src/peerConnectionUtils.cpp
	src/VideoCapturer.cpp
	src/VcmCapturer.cpp
	src/RoomClient.cpp
)

# Private (implementation) header files.
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Source Dependencies
add_subdirectory(deps/libmediasoupclient "${CMAKE_CURRENT_BINARY_DIR}/libmediasoupclient")
add_subdirectory(deps/cpr "${CMAKE_CURRENT_BINARY_DIR}/cpr")

# Public (interface) headers from dependencies.
target_include_directories(${PROJECT_NAME} PUBLIC
	"${PROJECT_SOURCE_DIR}/deps/libmediasoupclient/include"	
	"${PROJECT_SOURCE_DIR}/deps/websocketpp"
	${CPR_INCLUDE_DIRS}

)

# Public (interface) dependencies.
target_link_libraries(${PROJECT_NAME} PUBLIC
	mediasoupclient
	${CPR_LIBRARIES}
	wssclient
	pthread
	rt
	
)



