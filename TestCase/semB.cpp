#include "../XPublic.h"
#include <stdio.h>

int main()
{
	ProcessEvent event;
	if(!event.OpenEvent("hello")){
		puts("Open Event failed!");
		return 0;
	}
	while(1){
		event.Wait();
		puts("Open Event");
	}
	return 0;
}
