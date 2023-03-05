#include "application.hpp"

#include <iostream>

using namespace M3D;

int main( int argc, char * argv[] )
{
	try
	{
		Application::getInstance().start();
		return EXIT_SUCCESS;
	}
	catch ( const std::exception & e )
	{
		std::cerr << "Exception caught: " << std::endl << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
