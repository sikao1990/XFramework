#include "../XPublic.h"
#include <stdio.h>
#include <unistd.h>

int main()
{
	ProcessEvent event;
	if(!event.CreateEvent("hello")){
		puts("create sem failed!");
		perror("error:");
		return 0;
	}
	while(1){
		sleep(2);
		event.Post();
	}
	return 0;
}

