#include "application.hpp"

#include "input_output/log/console.hpp"

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
		console.printError(e.what());
		return EXIT_FAILURE;
	}
}
