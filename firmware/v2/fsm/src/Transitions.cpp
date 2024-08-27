/////////////////////////////////////////////////////////////////
#include "Transitions.h"
/////////////////////////////////////////////////////////////////

int AbstractTransition::_next_id = 0;

/////////////////////////////////////////////////////////////////

AbstractTransition::AbstractTransition() {
  id = _next_id;
  _next_id++;
}

/////////////////////////////////////////////////////////////////

void AbstractTransition::setGuardCondition(GuardCondition f) {
  guard_cb = f;
}

/////////////////////////////////////////////////////////////////

String AbstractTransition::getName() const {
  return name;
}

/////////////////////////////////////////////////////////////////

void AbstractTransition::setName(String name) {
  this->name = name;
}

/////////////////////////////////////////////////////////////////

void AbstractTransition::setOnRunHandler(CallbackFunction f) {
  on_run_cb = f;
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// Transition
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

int Transition::getEventID() const {
  return event_id;
}

/////////////////////////////////////////////////////////////////

int Transition::getID() const {
  return id;
}

/////////////////////////////////////////////////////////////////

Transition::Transition() : event_id(0) {}

/////////////////////////////////////////////////////////////////

Transition::Transition(State* from, State* to, int event_id, CallbackFunction on_run /* = NULL */, String name /* = "" */, GuardCondition guard /* = NULL */) {
  setup(from, to, event_id, on_run, name, guard);
}

/////////////////////////////////////////////////////////////////

void Transition::setup(State* from, State* to, int event_id, CallbackFunction on_run /* = NULL */, String name /* = "" */, GuardCondition guard /* = NULL */) {
  this->from = from;
  this->to = to;
  this->event_id = event_id;
  this->on_run_cb = on_run;
  this->name = name;
  this->guard_cb = guard;
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// TimedTransition
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

TimedTransition::TimedTransition() : start(0), interval(0) {}

/////////////////////////////////////////////////////////////////

TimedTransition::TimedTransition(State* from, State* to, int interval, CallbackFunction on_run /* = NULL */, String name /* = "" */, GuardCondition guard /* = NULL */) : TimedTransition() {
  setup(from, to, interval, on_run, name, guard);
}

/////////////////////////////////////////////////////////////////

void TimedTransition::setup(State* from, State* to, int interval, CallbackFunction on_run /* = NULL */, String name /* = "" */, GuardCondition guard /* = NULL */) {
  this->from = from;
  this->to = to;
  this->interval = interval;
  this->on_run_cb = on_run;
  this->name = name;
  this->guard_cb = guard;
}

/////////////////////////////////////////////////////////////////

int TimedTransition::getInterval() const {
  return interval;
}

/////////////////////////////////////////////////////////////////

int TimedTransition::getID() const {
  return id;
}

/////////////////////////////////////////////////////////////////

void TimedTransition::reset() {
  start = 0;
}

/////////////////////////////////////////////////////////////////