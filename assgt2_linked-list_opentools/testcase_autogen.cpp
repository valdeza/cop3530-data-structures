
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

#include <algorithm>	// min()
#include <cstdlib>	// (s)rand()
#include <ctime>	// time()
#include <cstring>	// strcmp()
#include <cmath>	// sqrt()
#include <iostream>

void print_help(bool print_short) {
	if (print_short)
		std::cout << "Usage: ./testcase-autogen [-h | --help | ia | i1 i2 i3 i4]" << std::endl;
	else
		std::cout
			<< "Usage:\n"
			<< "   ./testcase-autogen [-h | --help | ia | i1 i2 i3 i4]\n\n"

			<< "Options:\n"
			<< "   -h     Short help. Displays just the Usage section.\n"
			<< "   --help Long help. Displays the message you're reading right now.\n\n"

			<< "Parametres:\n"
			<< "   ./testcase-autogen\n"
			<< "      (Running program without arguments)\n"
			<< "      Essentially invokes: ./testcase-autogen 100 20 5 19\n\n"

			<< "   ./testcase-autogen ia\n"
			<< "      ia - (int) Maximum size of the post-interleave operation list.\n\n"

			<< "   ./testcase-autogen i1 i2 i3 i4\n"
			<< "      i1 - (int) Maximum size of the supplied initial list.\n"
			<< "	  i2 - (int) Maximum number of additional lists.\n"
			<< "	  i3 - (int) Maximum step value of an additional list.\n"
			<< "	  i4 - (int) Maximum size of an additional list.\n\n"

			<< "      Important notice: The arguments you supply will not be validated.\n"
			<< "	  For instance, \n"
			<< "	  ./testcase-autogen 5 100 100 100\n"
			<< "	  will almost always give you an invalid test case that the graders\n"
			<< "	  would never use on us, at least for now. But hey. Useful for debugging.\n\n"

			<< "Examples:\n"
			<< "   ./testcase-autogen 100\n"
			<< "   ./testcase-autogen 100 20 5 19"
			<< std::endl;
	// Thank regex for not having to manually make plaintext into a series of strong streamed strings
}

int main(int argc, char** argv) {
	// 0 - No-argument start.
	// 1 - Maximum post-interleave op size specified.
	// 2 - Testcase composition specified.
	int start_code = 0;
	//NOTE When writing CLI output, keep in mind that MS Command Prompt is 80char wide
	if (argc == 2) {
		if (std::strcmp(argv[1], "-h") == 0) { // Short help (usage)
			print_help(true);
			return 0;
		}
		else if (std::strcmp(argv[1], "--help") == 0) { // Long help (extensive "manpage"-like output)
			print_help(false);
			return 0;
		}
		else { //WARN Assuming this is a number
			start_code = 1;
		}
	}
	else if (argc == 5) {
		start_code = 2;
	}
	else if (argc != 1) {
		std::cerr << "Invalid arguments." << std::endl;
		print_help(true);
	}

	// Initialisation
	std::srand(std::time(0));

	int 
		INIT_SIZE_MAX, 
		ADDL_COUNT_MAX, 
		ADDL_STEP_MAX, 
		ADDL_SIZE_MAX;

	// Initialises above "constants".
	switch (start_code) {
		case 0: {
			INIT_SIZE_MAX = 100;
			ADDL_COUNT_MAX = 20;
			ADDL_STEP_MAX = 5;
			ADDL_SIZE_MAX = 19;
			break;
			}
		case 1: {
			int argvtoi = std::atoi(argv[1]);
			//// Method 1: I don't even know
			//const int TOTAL_NUM_LISTS_MIN = 2; // The purpose of the assignment is to interleave, after all.
			//int total_num_lists_max = argvtoi;
			//int total_num_lists = 
			//	(std::rand() % (total_num_lists_max - TOTAL_NUM_LISTS_MIN + 1)) + TOTAL_NUM_LISTS_MIN;
			//int all_list_span = argvtoi / total_num_lists;

			// Method 2: Trying to bring the variables closer together.
			int total_num_lists = (int)std::sqrt(argvtoi);
			int all_list_span = total_num_lists;

			INIT_SIZE_MAX = all_list_span;
			ADDL_COUNT_MAX = total_num_lists - 1; // -1 because excluding initial list.
			ADDL_STEP_MAX = (int)std::sqrt(INIT_SIZE_MAX);
			ADDL_SIZE_MAX = ADDL_STEP_MAX;

			//// Method 3: This should do it?
			//// deriv: @WolframAlpha: inverse function y(x) = 2*(x^(1/2)+x^(1/4))
			//int base = (int)
			//	(0.25 * (std::pow(argvtoi, 2) + 4 * argvtoi + 2)
			//	- 0.5 * (argvtoi + 1) * std::sqrt(2 * argvtoi + 1));
			//INIT_SIZE_MAX = (int)std::sqrt(base);
			//ADDL_COUNT_MAX = INIT_SIZE_MAX;
			//ADDL_STEP_MAX = (int)std::sqrt(INIT_SIZE_MAX);
			//ADDL_SIZE_MAX = ADDL_STEP_MAX;
			//// CAN'T FIGURE OUT THAT GOLDEN NUMBER

			//whoomath
			//edit: Actually, I totally overcomplicated total_num_lists.

			std::clog
				<< "Generated testcase limits:\n"
				<< "i1, INIT_SIZE_MAX  = " << INIT_SIZE_MAX << "\n"
				<< "i2, ADDL_COUNT_MAX = " << ADDL_COUNT_MAX << "\n"
				<< "i3, ADDL_STEP_MAX  = " << ADDL_STEP_MAX << "\n"
				<< "i4, ADDL_SIZE_MAX  = " << ADDL_SIZE_MAX << "\n"
				<< std::endl;
			break; 
		}
		case 2: {
			INIT_SIZE_MAX = std::atoi(argv[1]);
			ADDL_COUNT_MAX = std::atoi(argv[2]);
			ADDL_STEP_MAX = std::atoi(argv[3]);
			ADDL_SIZE_MAX = std::atoi(argv[4]);
			break;
		}
		default: {
			std::cerr << "Unexpected error." << std::endl;
			return 1;
		}
	}

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
		int addl_list_size = std::rand() % std::min( 
			ADDL_SIZE_MAX, 
			init_size / addl_list_step
			); 
		std::cout << ++addl_list_size << std::endl;

		// Additional List: "the elements"
		std::cout << ++i_anylist_element;
		// i = 1 because first element already printed.
		for (int j = 1; j < addl_list_size; ++j)
			std::cout << " " << ++i_anylist_element;
	}
	std::cout << std::endl;

	return 0;
}
