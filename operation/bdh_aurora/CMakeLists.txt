cmake_minimum_required(VERSION 2.8.3)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(bdh_aurora)

find_package(catkin REQUIRED COMPONENTS 
roscpp 
std_msgs 
da_msg
)

find_package(Qt5Widgets REQUIRED)
#find_package(Qt5Charts REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5SerialPort REQUIRED)


#qt5_wrap_cpp(gui_SRC gui.h gui.cpp)



INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5SerialPort_DEFINITIONS})

catkin_package(
  INCLUDE_DIRS include
#  LIBRARIES vaurora
  CATKIN_DEPENDS roscpp
  CATKIN_DEPENDS roscpp std_msgs da_msg
#   slave_display
#   DEPENDS system_lib
)


include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${Qt5SerialPort_INCLUDE_DIRS}
)



set(sources
    src/main.cpp
    src/mainwindow.cpp
    src/vaurora.cpp
    src/vserialcomm.cpp
    src/Thread.cpp
)

set(headers
    src/mainwindow.h
    src/apistructures.h
    src/vaurora.h
    src/Thread.h
)

set(forms
    src/mainwindow.ui
)


QT5_WRAP_CPP(HEADERS_MOC ${headers})
QT5_WRAP_UI(FORMS_HEADERS ${forms})
qt5_wrap_cpp(sources ${headers})

ADD_LIBRARY(bdh_aurora_CONFIG ${HEADERS_MOC} ${FORMS_HEADERS})
QT5_USE_MODULES(bdh_aurora_CONFIG Widgets Core SerialPort)


add_executable(${PROJECT_NAME}_node ${sources} ${headers} ${forms} ${bdh_aurora_CONFIG})


target_link_libraries(${PROJECT_NAME}_node
  ${catkin_LIBRARIES}
  ${QT_LIBRARIES}
  Qt5::PrintSupport
  Qt5::SerialPort
  #vaurora
)
#target_link_libraries(mainwindow vaurora)
