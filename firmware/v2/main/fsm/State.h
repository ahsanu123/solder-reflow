/////////////////////////////////////////////////////////////////

#pragma once
#ifndef STATE_H
#define STATE_H

/////////////////////////////////////////////////////////////////

/*#include "Arduino.h"*/
#include <string>

/////////////////////////////////////////////////////////////////

typedef void (*CallbackFunction)();
typedef bool (*GuardCondition)();

/////////////////////////////////////////////////////////////////

class State {
  friend class SimpleFSM;

public:
  State();
  State(std::string name, CallbackFunction on_enter,
        CallbackFunction on_state = NULL, CallbackFunction on_exit = NULL,
        bool is_final = false);

  void setup(std::string name, CallbackFunction on_enter,
             CallbackFunction on_state = NULL, CallbackFunction on_exit = NULL,
             bool is_final = false);
  void setName(std::string name);
  void setOnEnterHandler(CallbackFunction f);
  void setOnStateHandler(CallbackFunction f);
  void setOnExitHandler(CallbackFunction f);
  void setAsFinal(bool final = true);

  int getID() const;
  bool isFinal() const;
  std::string getName() const;

protected:
  int id;
  static int _next_id;

  std::string name = "";
  CallbackFunction on_enter = NULL;
  CallbackFunction on_state = NULL;
  CallbackFunction on_exit = NULL;
  bool is_final = false;
};

/////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////
