#include "ui.h"
#include "user.h"
#include <windows.h>
int main(){
	SetConsoleOutputCP(CP_UTF8);
	User* user = new User();
	ProfileUI::menu(*user);
	return 0;
}