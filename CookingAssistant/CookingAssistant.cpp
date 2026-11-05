#include "ui.h"
#include "user.h"
int main(){
	User* user = new User();
	UI::menu(*user);
	return 0;
}