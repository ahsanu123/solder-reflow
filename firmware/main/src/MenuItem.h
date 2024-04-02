#include <string>

typedef std::string DisplayText;
typedef void (*Callback)();

class MenuItem {
  Callback callback;
  DisplayText displayText;

public:
  int Execute();
  DisplayText setDisplayText(std::string text);
  DisplayText getDisplayText();
};
