//Simple file to help test our engine.
// Exists mainly to check if linking succeeds or fails.
#include "SFML_Engine.hpp"


int main(int argc, char** argv)
{
	//Simple game loop
	init_sfml(800, 600, 32);

	int res = 0;
	while (!res) {
	    res = sfml_handle_events();
	    demo_display();
	    sfml_display();
	}

	//Cleanup
	close_sfml();

	return 0;
}


