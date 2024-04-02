
#include "MenuItem.h"
#include <string>

#ifndef INCLUDE_SRC_MENUITEM_CPP_
#define INCLUDE_SRC_MENUITEM_CPP_

int MenuItem::Execute() {
  this->callback();
  return 0;
}

DisplayText MenuItem::getDisplayText() { return this->displayText; }

DisplayText MenuItem::setDisplayText(std::string text) {
  this->displayText = text;
  return this->displayText;
}

#endif // INCLUDE_SRC_MENUITEM_CPP_
