#include <iostream>
#include <cstdlib>

/* Normally, the code would be more modular and include a .h header file, 
 * but you requested "a single cpp file."
 */

// Note to self: DO NOT STRAY FROM EXPECTED OUTPUT. 

int main(int argc, char** argv)
{
	////////////
	////// SETUP
	////////////

	// Accepts 11 chars to parse into an int. 12 specified to make room for '\0'.
	// INT_MAX ~ 2.15e9
	const int MAX_EXPECTED_INT_INPUT = 12;
	char cstr_in[MAX_EXPECTED_INT_INPUT];

	// Get number of ints to accept.
	std::cin.getline(cstr_in, MAX_EXPECTED_INT_INPUT, '\n');
	int array_size = std::atoi(cstr_in);
	int* int_array = new int[array_size]; /* Allocated on the heap to enable variable-length arrays */

	// Accept $array_size ints.
	for ( int i = 0 ; i < array_size ; i++ )
	{
		std::cin.getline(cstr_in, MAX_EXPECTED_INT_INPUT, '\n');
		if (std::cin.fail())
		{
			std::cerr << "Invalid input! (Number too large?)" << std::endl;
			// Bad input acknowledged, reset failbit.
			std::cin.clear();
		}
		int_array[i] = std::atoi(cstr_in);
	}
	




	////////////
	////// WORK
	////////////
	
	//////
	/// Perform insertion sort
	//////

	/* If MOSS flags this code as similar to https://github.com/valdeza/cop3530-data-structures/ ,
	 * 1. valdeza is my user handle
	 * 2. I essentially did tiny translates from Java to C++, keeping most of the structure the same while adapting code style.
	 */
	// Last element (first to check) skipped; serves as initial comparison.
	for ( int probe = array_size - 2; probe >= 0; probe-- )
	{
		if (int_array[probe] <= int_array[probe+1])
			continue;
		//else
		// Prepare to reposition element.
		bool is_end_reached = true; // Assume true unless following for loop exits early.
		int temp = int_array[probe];
		// Sentinel condition ends one element early to avoid ArrayIndexOutOfBoundsException. 
		for ( int probe_reposition = probe + 1; probe_reposition < array_size - 1; probe_reposition++ )
		{
			// Shift current second probe to examine the next.
			int_array[probe_reposition-1] = int_array[probe_reposition];
			if (temp <= int_array[probe_reposition+1])
			{ // Proper sorted place found.
				int_array[probe_reposition] = temp;
				is_end_reached = false;
				break;
			}
		}
		if (is_end_reached)
		{ // Place $temp at the end.
			int idx_last_elem = array_size - 1;
			int_array[idx_last_elem-1] = int_array[idx_last_elem];
			int_array[idx_last_elem] = temp;
		}
	}
	


	//////
	/// Print sorted array
	//////
	for ( int i = 0 ; i < array_size ; i++ )
		std::cout << int_array[i] << std::endl;





	////////////
	////// CLEANUP
	////////////
	delete[] int_array;
}