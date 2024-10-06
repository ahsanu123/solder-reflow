idf_component_register(
  SRCS

  main.cpp

  ./device/ADCDevice.h
  ./device/Device.h
  ./device/GPIODevice.h

  ./fsm/Transitions.h
  ./fsm/Transitions.cpp
  ./fsm/SimpleFSM.h
  ./fsm/SimpleFSM.cpp
  ./fsm/State.h
  ./fsm/State.cpp

  INCLUDE_DIRS ""
)
component_compile_options(-Wno-void-pointer-to-enum-cast)

