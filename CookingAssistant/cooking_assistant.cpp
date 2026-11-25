#include "ui.h"

#include <windows.h>

int main() {
  SetConsoleOutputCP(CP_UTF8);
  User user;
  UI::menu(user);
  return 0;
}
