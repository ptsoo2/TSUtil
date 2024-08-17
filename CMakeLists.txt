cmake_minimum_required(VERSION 3.10)

# 전달이 필요한 인자
# BUILD_MODE: DEBUG|RELEASE


# 상수
SET(PROJECT_NAME TSUtil)
SET(OUTPUT_FILE_NAME .)
string(TOLOWER ${PROJECT_NAME} OUTPUT_FILE_NAME)


# 프로젝트 이름 및 버전
project(${PROJECT_NAME} VERSION 1.0.0)


# C++20 표준 사용 설정
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# 최적화 설정
set(CMAKE_BUILD_TYPE Release)

if(BUILD_MODE STREQUAL "DEBUG")
	set(CMAKE_BUILD_TYPE Debug)
elseif(BUILD_MODE STREQUAL "RELEASE")
else()
	message(WARNING "not provided or unknown mode, adjust default(${CMAKE_BUILD_TYPE})")
endif()


# 메시지 출력
message(STATUS ">> CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")


# 소스 추가
file(GLOB_RECURSE HEADER_FILES 
                  "include/*.hpp"
                  "include/*.h")
file(GLOB_RECURSE SOURCE_FILES 
                  "src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})


# 폴더 구조 반영
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    source_group(TREE "${CMAKE_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADER_FILES})
    source_group(TREE "${CMAKE_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SOURCE_FILES})
endif()


# 헤더 파일 디렉터리 설정
target_include_directories(${PROJECT_NAME}
                           PUBLIC 
                           ${CMAKE_CURRENT_SOURCE_DIR}/include)


# 출력 파일 이름 설정
set_target_properties(${PROJECT_NAME} PROPERTIES
                      PREFIX ""
                      OUTPUT_NAME "${OUTPUT_FILE_NAME}"
                      )


# 출력 경로 설정
set_target_properties(${PROJECT_NAME} PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY
                      "${CMAKE_BINARY_DIR}/output"
                      )


# 컴파일 옵션 설정
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /Wall 
                                                   /wd4625  # 복사 생성자가 암시적으로 삭제된 것으로 정의되었습니다.
                                                   /wd4626  # 대입 연산자가 암시적으로 삭제된 것으로 정의되었습니다.
                                                   /wd5026  # 이동 생성자가 암시적으로 삭제된 것으로 정의되었습니다.
                                                   /wd5027  # 대입 연산자가 암시적으로 삭제된 것으로 정의되었습니다.
                                                   /wd4820  # 바이트 패딩이 데이터 멤버 '##' 다음에 추가됨
    )
elseif(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall
                                                   -Wextra
    )
endif()
