
 //TODO
 //X1. Comment code and figure out workflow.
 //X1.5. Write a README.md. Don't forget to mention that we make no guarantees that this autogen will generate test cases that can be processed in this century.
 // 1.8. Perform naive benchmarking to estimate when the worst test case *might* finish.
 //X2. Restructure as necessary.
 //X3. Change file output to console output
 //X (This way, if people want to make a file, they can just redirect output to their own specified file.)
 // 4. Implement program arguments to allow user to specify maximum end list.
 // 4.3. Support -h and --help arguments
 // 4.7. Validate and inform user if their arguments might lead to an invalid list
 // 4.7.5 Allow users to force creation of potentially invalid lists with -f or --force

#include <stdlib.h>
#include <ctime>
#include <iostream>

int main(int argc, char** argv) {
	// Initialisation
	std::srand(std::time(0));

	int INIT_SIZE_MAX = 100,
		ADDL_COUNT_MAX = 20,
		ADDL_STEP_MAX = 5,
		ADDL_SIZE_MAX = 19;

	int i_anylist_element = 0;

	// "the number of elements in the initial list"
	int init_size = (std::rand() % INIT_SIZE_MAX) + 1; // [1,INIT_SIZE_MAX]
	std::cout << init_size << std::endl;

	// "the elements in the initial list"
	std::cout << ++i_anylist_element;
	// i = 1 because first element already printed.
	for (int i = 1; i < init_size; ++i)
		std::cout << " " << ++i_anylist_element;
	std::cout << std::endl;

	// "number of additional lists"
	int addl_count = (std::rand() % ADDL_COUNT_MAX) + 1; // [1,ADDL_COUNT_MAX]
	std::cout << addl_count;
	
	for (int i = 0; i < addl_count; i++) {
		// Additional List: "step"
		int addl_list_step = (std::rand() % ADDL_STEP_MAX) + 1;
		std::cout << std::endl << addl_list_step << std::endl;

		// Additional List: "number of elements"
		// fmin() limits ADDL_SIZE_MAX if it exceeds (init_size / addl_list_step)
		int addl_list_size = std::rand() % (int)std::fmin( 
			ADDL_SIZE_MAX, 
			init_size / addl_list_step
			); 
		std::cout << addl_list_size << std::endl;

		// Additional List: "the elements"
		std::cout << ++i_anylist_element;
		// i = 1 because first element already printed.
		for (int i = 1; i < addl_list_size; ++i)
			std::cout << " " << ++i_anylist_element;
	}
	std::cout << std::endl;

	return 0;
}
