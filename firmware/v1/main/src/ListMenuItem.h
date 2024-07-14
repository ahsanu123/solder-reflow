
#include "MenuItem.h"
#include <list>

typedef std::list<MenuItem> MenuList;
class ListMenuItem {

  MenuList menuList;
  MenuItem current;

public:
  MenuItem GetCurrent();
  MenuItem Next();
  MenuItem Prev();
  void Execute();

  ListMenuItem(std::list<MenuItem> MenuList) { this->menuList = MenuList; }
};
