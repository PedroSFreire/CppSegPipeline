#include <iostream>
#include "Pipeline.h"


int main() {
    // Ask user for the file name

	Pipeline pipeline;
	if(pipeline.fullPipeline())
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;

}