cmake_minimum_required(VERSION 2.8.3)
project(opencv)

find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  image_transport
  roscpp
  rospy
  sensor_msgs
  std_msgs
  roslint
  message_generation
)
find_package(OpenCV)


## Declare ROS messages and services

add_message_files(FILES vision_msg.msg)
add_message_files(FILES pub_msg.msg)


generate_messages(DEPENDENCIES std_msgs)

catkin_package(
  INCLUDE_DIRS 
  CATKIN_DEPENDS cv_bridge roscpp rospy
  LIBRARIES bloblabeling
  DEPENDS 
)

include_directories(
   ${catkin_INCLUDE_DIRS}
   ${OpenCV_INCLUDE_DIRS}
)


add_library(dh_cam SHARED src/dh_cam.cpp)

add_executable(main src/main.cpp)
add_dependencies(main vision_msg_generate_messages_cpp)
target_link_libraries(main ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(dh_cam ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(main dh_cam)




