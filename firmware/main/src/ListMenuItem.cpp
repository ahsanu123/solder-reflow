
#include "ListMenuItem.h"
#include <algorithm>
#include <list>

#ifndef INCLUDE_SRC_LISTMENUITEM_CPP_
#define INCLUDE_SRC_LISTMENUITEM_CPP_

MenuItem ListMenuItem::GetCurrent() { return this->current; }

MenuItem ListMenuItem::Next() {
  std::list<MenuItem>::iterator iterator = std::find_if(
      this->menuList.begin(), this->menuList.end(),
      [&](MenuItem const &item) { return item.id == this->current.id; });

  return *std::next(iterator, 1);
}

MenuItem ListMenuItem::Prev() {
  std::list<MenuItem>::iterator iterator = std::find_if(
      this->menuList.begin(), this->menuList.end(),
      [&](MenuItem const &item) { return item.id == this->current.id; });

  return *std::prev(iterator, 1);
}

void ListMenuItem::Execute() {
  auto current = this->GetCurrent();
  current.Execute();
}

#endif // INCLUDE_SRC_LISTMENUITEM_CPP_
