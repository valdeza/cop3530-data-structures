// Assignment 3 flags - Cuts corners to reduce execution time. Comment out to disable.
#if 1
	#define A3_FLUSH_LESS
	#define A3_NO_MEMFREE_AT_END
#endif

#if 0
	#define DBG_COUNT_ZERO_SUMS
#endif

#include <iostream>

//WARN To meet strict time requirements, memory allocated by building linked lists WILL NOT BE FREED.
//NOTE stdout will only flush as the operating systemme sees fit. (i.e. std::/(endl|flush)/ is not used until the programs conclusion).

/* Normally, the code would be more modular and include a .h header file,
* but you requested "a single cpp file."
*
* THERE IS MINIMAL INPUT VALIDATION/ERROR HANDLING
* due to strict specification on input/output.
* Any deviance from specified input will lead to UNDEFINED BEHAVIOUR.
* Note to self: This also means no assertions.
*/

/* All variable names in all-caps beginning with "RT_" are pseudoconstants;
 * these constants are defined at RunTime and are not intended to be modified throughout their lifetime.
 * To help differentiate from true constants, actual constants using keyword "const" are denoted with a prepended "C_".
 */

// Other notes to self:
// DO NOT STRAY FROM EXPECTED OUTPUT. 
/* //4CAT Definitions
 * //TODO
 * //NOTE Important implementation notes
 * //WARN Potentially dangerous code
 * //MULT Note specified in multiple locations throughout source file.
 *	Removing one //MULT might necessitate removing the others.
 */

class d2_int_node_data
{
private:
	int row, col;

public:
	/// <remarks>No checks performed. Made public for easier access (and avoiding possibly confusing operator overloading).</remarks>
	int int_val;

	d2_int_node_data(int init_row, int init_col, int init_val) :
		row(init_row), col(init_col), int_val(init_val)
	{}

	int get_row() const
		{ return this->row; }
	int get_col() const
		{ return this->col; }
};

template <typename T>
/// <summary>
/// "Linked pointer node"<br />
/// (get_data() will return *by address*)
/// </summary>
class linked_ptr_node
{
private:
	bool mfree_data_on_dtor;
	T* data;
	linked_ptr_node<T>* next;

public:
	/// <param name="node_responsible_for_memfree">Upon deleting this node, if param node_responsible_for_memfree is true, the data pointer will be deleted. (i.e. If you wish to manage memory yourself, specify false)</param>
	linked_ptr_node(bool node_responsible_for_memfree, T* init_data, linked_ptr_node<T>* init_next = NULL) :
		mfree_data_on_dtor(node_responsible_for_memfree), data(init_data), next(init_next)
	{}

	T* get_data() const
		{ return this->data; }
	/// <summary>NOTICE: Be sure to save or delete the current data before replacing the *pointer*!</summary>
	void set_data(T* const new_data)
		{ this->data = new_data; }
	linked_ptr_node<T>* get_next() const
		{ return this->next; }
	void set_next(linked_ptr_node<T>* const new_next)
		{ this->next = new_next; }

	~linked_ptr_node()
	{
		if (this->mfree_data_on_dtor)
			delete this->data;
	}
};

typedef linked_ptr_node< linked_ptr_node<d2_int_node_data> > d2_linked_ptr_node;

int main(int argc, char** argv)
{
	std::ios::sync_with_stdio(false); // Instructor suggestion. Desyncs standard streams. Different I/O method calls are no longer expected to run in the expected order.

	const int C_MATRIX_COUNT = 2; // Input will always be two matrices. Constant used for code extensibility.

	int RT_NUM_ROWS; // "Number of rows"
		// All matrices will have the same number of rows. This can be checked for non-matching values to warn about invalid input.
	std::cin >> RT_NUM_ROWS;

	d2_linked_ptr_node* sparse_row_head = NULL;



	////////
	//// Set up initial matrix.
	////////
	d2_linked_ptr_node* curr_row_head_probe = sparse_row_head;

	for (int ie_row = 0; ie_row < RT_NUM_ROWS; ++ie_row)
	{
		int RT_NUM_COLS; // "Number of elements in the $nth row"
		std::cin >> RT_NUM_COLS;

		if (RT_NUM_COLS <= 0)
			continue; // No elements in this row. Skip.
		// else (RT_NUM_COLS > 0)

		d2_linked_ptr_node* new_row_head =
			new d2_linked_ptr_node(false, NULL, NULL);
		if (curr_row_head_probe == NULL)
		{ // Create first-ever row.
			sparse_row_head = new_row_head;
			curr_row_head_probe = sparse_row_head;
		}
		else
		{ // Add new row at end of current linked list.
			curr_row_head_probe->set_next(new_row_head);
			curr_row_head_probe = curr_row_head_probe->get_next();
		}

		linked_ptr_node<d2_int_node_data>* curr_col_probe;

		// At this point, there is guaranteed to be at least one element in this row. This first element performs assigns to the linked list (creates head) rather than appending to it. Since this is only done once, this is made separate from the for loop to avoid re-evaluating an if statement potentially many times.
		// Also, "RT_" constants placed in scope to enforce write-only-once constraint.
		{
			int
				RT_IN_COL_IDX, // Column index of current element
				RT_IN_VAL; // Value of current element
			std::cin
				>> RT_IN_COL_IDX
				>> RT_IN_VAL;

			curr_col_probe = new linked_ptr_node<d2_int_node_data>(true, new d2_int_node_data(ie_row, RT_IN_COL_IDX, RT_IN_VAL));

			// Link row head to these row elements.
			curr_row_head_probe->set_data(curr_col_probe);
		}

		// Start ie_col at 1 (not 0) because the first element has already been added.
		for (int ie_col = 1; ie_col < RT_NUM_COLS; ++ie_col)
		{
			int
				RT_IN_COL_IDX, // Column index of current element
				RT_IN_VAL; // Value of current element
			std::cin
				>> RT_IN_COL_IDX
				>> RT_IN_VAL;

			curr_col_probe->set_next(new linked_ptr_node<d2_int_node_data>(true, new d2_int_node_data(ie_row, RT_IN_COL_IDX, RT_IN_VAL)));
			curr_col_probe = curr_col_probe->get_next();
		}
	}



	////////
	//// Add n additional matrices to initial matrix.
	////////

	// Start i_sparse at 1 (not 0) because the first was already counted (setting up initial matrix)
	for (int i_sparse = 1; i_sparse < C_MATRIX_COUNT; ++i_sparse)
	{
		// Reinitialise current probe pointers:
		d2_linked_ptr_node* curr_row_head_probe = sparse_row_head;
		int RT_NUM_ROWS; // "Number of rows"
			// All matrices will have the same number of rows. This can be checked for non-matching values to warn about invalid input.
		std::cin >> RT_NUM_ROWS;

		// ie = "iterated element"
		for (int ie_row = 0; ie_row < RT_NUM_ROWS; ++ie_row)
		{
			int RT_NUM_COLS; // All matrices will have the same number of columns. This can be checked for non-matching values to warn about invalid input.
			std::cin >> RT_NUM_COLS;

			if (RT_NUM_COLS <= 0)
				continue; // No elements in this row. Skip.
			// else (RT_NUM_COLS > 0)

			// The above checkpoint is important to prevent the following code from creating blank (NULL) rows, just in case memory complexity is being checked.

			/* //NOTE//MULT["RCSeek"] Base implementation for persistent seeking through a linked list. Updates to this implementation should be reflected to others sharing this "RCSeek" tag.\
			 * Also, the "RCSeek" code be extracted to a method? But it would require the appropriate parametres (pointers used, probably not a problem) and passing a function pointer to take care of "minute" differences.
			 */
			// Move row probe to current row, if possible.
			while (curr_row_head_probe->get_next() != NULL && curr_row_head_probe->get_next()->get_data()->get_data()->get_row() <= ie_row)
				curr_row_head_probe = curr_row_head_probe->get_next();

			int curr_row_head_idx = curr_row_head_probe->get_data()->get_data()->get_row();

			if (curr_row_head_idx < ie_row) // Are there elements to add to an empty row?
			{ // Insert a new row head to "add" (dump) the read-in elements into.
				curr_row_head_probe->set_next(
					new d2_linked_ptr_node(false, NULL, curr_row_head_probe->get_next()));
				curr_row_head_probe = curr_row_head_probe->get_next();
			}
			else if (curr_row_head_idx == ie_row)
			{
				// OK.
			}
			else if (curr_row_head_idx > ie_row)
			{
				std::cerr << "\nERROR: Current row head probe positioned too far!" << std::endl;
			}
			else
			{
				std::cerr << "\nERROR: Unable to determine current row head probe position!" << std::endl;
			}

			linked_ptr_node<d2_int_node_data>* curr_col_probe = curr_row_head_probe->get_data();

			// At this point, there is guaranteed to be at least one element in this column. This first element performs assigns to the linked list (creates head) rather than appending to it. Since this is only done once, this is made separate from the for loop to avoid re-evaluating an if statement potentially many times.
			// Also, "RT_" constants placed in scope to enforce write-only-once constraint.
			{
				int
					RT_IN_COL_IDX, // Column index of current element
					RT_IN_VAL; // Value of current element
				std::cin
					>> RT_IN_COL_IDX
					>> RT_IN_VAL;

				if (curr_col_probe == NULL) // If this is a new inserted row
				{
					curr_col_probe = new linked_ptr_node<d2_int_node_data>(true, new d2_int_node_data(ie_row, RT_IN_COL_IDX, RT_IN_VAL));

					// Link column head to these column elements.
					curr_row_head_probe->set_data(curr_col_probe);
				}
				else
				{ // if (curr_col_probe != NULL)
					// Seek appropriate column (or get as close to it as possible)
					/* //NOTE//MULT["RCSeek"] Similar implementation as column seeking above.\
					 * Also, the "RCSeek" code be extracted to a method? But it would require the appropriate parametres (pointers used, probably not a problem) and passing a function pointer to take care of "minute" differences.
					 */
					while (curr_col_probe->get_next() != NULL && curr_col_probe->get_next()->get_data()->get_col() <= RT_IN_COL_IDX)
						curr_col_probe = curr_col_probe->get_next();

					int curr_col_idx = curr_col_probe->get_data()->get_col();

					if (curr_col_idx < RT_IN_COL_IDX)
					{ // Insert read-in element after current column.
						curr_col_probe->set_next(
							new linked_ptr_node<d2_int_node_data>(
								true, 
								new d2_int_node_data(ie_row, RT_IN_COL_IDX, RT_IN_VAL), 
								curr_col_probe->get_next()
								));
					}
					else if (curr_col_idx == RT_IN_COL_IDX)
					{ // Add read-in element directly to current column.
						curr_col_probe->get_data()->int_val += RT_IN_VAL;
					}
					else if (curr_col_idx > RT_IN_COL_IDX)
					{ // Insert read-in element before the current (first) 
						curr_row_head_probe->set_data(
							new linked_ptr_node<d2_int_node_data>(
								true, 
								new d2_int_node_data(ie_row, RT_IN_COL_IDX, RT_IN_VAL), 
								curr_row_head_probe->get_data()
								));
						curr_col_probe = curr_row_head_probe->get_data();
					}
					else // ???
					{
						std::cerr << "\nERROR: Unable to determine column position for matrix addition pre-procedure!" << std::endl;
					}
				}
			}

			// Start ie_col at 1 (not 0) because the first element has already been added.
			for (int ie_col = 1; ie_col < RT_NUM_COLS; ++ie_col)
			{
				int
					RT_IN_COL_IDX, // Column index of current element
					RT_IN_VAL; // Value of current element
				std::cin
					>> RT_IN_COL_IDX
					>> RT_IN_VAL;

				// Seek appropriate column (or get as close to it as possible)
				/* //NOTE//MULT["RCSeek"] Similar implementation as column seeking above.\
				 * Also, the "RCSeek" code be extracted to a method? But it would require the appropriate parametres (pointers used, probably not a problem) and passing a function pointer to take care of "minute" differences.
				 */
				while (curr_col_probe->get_next() != NULL && curr_col_probe->get_next()->get_data()->get_col() <= RT_IN_COL_IDX)
					curr_col_probe = curr_col_probe->get_next();

				int curr_col_idx = curr_col_probe->get_data()->get_col();

				if (curr_col_idx < RT_IN_COL_IDX)
				{ // Insert read-in element after current column.
					curr_col_probe->set_next(
						new linked_ptr_node<d2_int_node_data>(
							true,
							new d2_int_node_data(ie_row, RT_IN_COL_IDX, RT_IN_VAL),
							curr_col_probe->get_next()
							));
				}
				else if (curr_col_idx == RT_IN_COL_IDX)
				{ // Add read-in element directly to current column.
					curr_col_probe->get_data()->int_val += RT_IN_VAL;
				}
				else if (curr_col_idx > RT_IN_COL_IDX)
				{ // This should not be possible after the pre-procedure above this for-loop.
					std::cerr << "\nERROR: Current row head probe positioned too far for matrix addition procedures!" << std::endl;
				}
				else // ???
				{
					std::cerr << "\nERROR: Unable to determine column position for matrix addition procedures!" << std::endl;
				}
			}//end-for: Adding columns after the first.
		}//end-for: Iterating through rows of added matrices (only one).
	}//end-for: Iterating through matrices to add (only one).



	////////
	//// QUICK FIX: Purge matrix of zeroes.
	////////

	d2_linked_ptr_node* curr_row_head_probe_prev = NULL;
	while (curr_row_head_probe != NULL)
	{
		bool do_delete_head = false;
		linked_ptr_node<d2_int_node_data> 
			*curr_col_probe_prev = NULL, 
			*curr_col_probe = curr_row_head_probe->get_data();

		while (curr_col_probe != NULL)
		{
			if (curr_col_probe->get_data()->int_val == 0)
			{ // Assume the identity of the next probe.
				linked_ptr_node<d2_int_node_data>* curr_col_probe_next = curr_col_probe->get_next();

				// Look back.
				// If there is no _prev, this column is a head that might be deleted!
				if (curr_col_probe_prev == NULL)
				{
					// Look forward.
					// If there is no replacement, mark head for deletion. 
					// Otherwise, assume identity of next node to preserve nodes pointing to this.
					if (curr_col_probe_next == NULL)
					{
						do_delete_head = true;
					}
					else // (curr_col_probe_next != NULL)
					{
						curr_col_probe->set_data(curr_col_probe_next->get_data());
						curr_col_probe->set_next(curr_col_probe_next->get_next());
#ifndef A3_NO_MEMFREE_AT_END
						delete curr_col_probe_next;
#endif

						// Since this node could be preserved, unmark for deletion, if it's a head node.
						do_delete_head = false;
					}//end if (curr_col_probe_next == NULL)
				}
				else // (curr_col_probe_prev != NULL)
				{
					curr_col_probe_prev->set_next(curr_col_probe_next);
					// Revert to previous node to avoid skipping the next node.
#ifndef A3_NO_MEMFREE_AT_END
					delete curr_col_probe;
#endif
					curr_col_probe = curr_col_probe_prev;
				}//end if (curr_col_probe_prev == NULL)
			}//end-if: Delete current matching index pair.

			curr_col_probe_prev = curr_col_probe;
			curr_col_probe = curr_col_probe->get_next();
		}//end-while: Current examined column

		bool do_shift_curr_head_probe = true;

		if (do_delete_head)
		{
			d2_linked_ptr_node* curr_row_head_probe_next = curr_row_head_probe->get_next();

			// Reset curr_col_probe
			curr_col_probe = curr_row_head_probe->get_data();

			if (curr_col_probe->get_next() == NULL)
			{ // Proceed with straight deleting head.
				if (curr_row_head_probe_prev == NULL)
				{
					if (curr_row_head_probe_next == NULL)		// This is the last element of this matrix.
					{
						// There is nothing to search.
#ifndef A3_NO_MEMFREE_AT_END
						delete curr_col_probe;
						delete curr_row_head_probe;
#endif
						return 0; // Exit main();
					}
					else // (curr_row_head_probe_next != NULL)	// The head can be reassigned to the next 
					{
						// Since there is no previous node, reassign sparse_row_head to the next node.
						sparse_row_head = curr_row_head_probe_next;
#ifndef A3_NO_MEMFREE_AT_END
						delete curr_col_probe;
						delete curr_row_head_probe;
#endif
						curr_row_head_probe = curr_row_head_probe_next;
						do_shift_curr_head_probe = false;
					}//end if (curr_row_head_probe_next == NULL)
				}
				else // (curr_row_head_probe_prev != NULL)		// Delete head and link previous to the next row head.
				{
					curr_row_head_probe_prev->set_next(curr_row_head_probe_next);
#ifndef A3_NO_MEMFREE_AT_END
					delete curr_col_probe;
					delete curr_row_head_probe;
#endif
					curr_row_head_probe = curr_row_head_probe_next;
					do_shift_curr_head_probe = false;
				}//end if (curr_row_head_probe_prev == NULL)
			}
			else
			{ // Transfer row head to the next node. Delete old head.
				curr_row_head_probe->set_data(curr_col_probe->get_next());
#ifndef A3_NO_MEMFREE_AT_END
				delete curr_col_probe;
#endif
			}

			
		}
		
		if (do_shift_curr_head_probe)
		{ // Only shift when the head has not been deleted.
			curr_row_head_probe_prev = curr_row_head_probe;
			curr_row_head_probe = curr_row_head_probe->get_next();
		}

		do_shift_curr_head_probe = true;
	}//end-while: Current examined row



	////////
	//// Search through matrix.
	////////

	int RT_SEARCH_COUNT;
	std::cin >> RT_SEARCH_COUNT;

#ifdef DBG_COUNT_ZERO_SUMS
	int dbg_zero_count = 0;
#endif

	for (int i_search = 0; i_search < RT_SEARCH_COUNT; ++i_search)
	{
		int RT_SEARCH_VAL; // "The [value] to search for"
		std::cin >> RT_SEARCH_VAL;

		bool is_first_match = true;

		// Reinitialise probe pointer
		curr_row_head_probe = sparse_row_head;
		while (curr_row_head_probe != NULL)
		{
			linked_ptr_node<d2_int_node_data>* curr_col_probe = curr_row_head_probe->get_data();

			while (curr_col_probe != NULL)
			{
#ifdef DBG_COUNT_ZERO_SUMS
				if (curr_col_probe->get_data()->int_val == 0)
					++dbg_zero_count;
#endif

				if (curr_col_probe->get_data()->int_val == RT_SEARCH_VAL)
				{
					// For all matches aside from the first, print a space before outputting the index pair.
					/* This is to avoid having extra whitespaces. */
					if (is_first_match)
					{
						is_first_match = false;
					}
					else
					{
						std::cout << ' ';
#ifndef A3_FLUSH_LESS
						std::cout.flush();
#endif
					}
					std::cout
						<< curr_col_probe->get_data()->get_row()
						<< " "
						<< curr_col_probe->get_data()->get_col();
#ifndef A3_FLUSH_LESS
					std::cout.flush();
#endif
				}//end-if: Print current matching index pair.

				curr_col_probe = curr_col_probe->get_next();
			}//end-while: Current examined column

			curr_row_head_probe = curr_row_head_probe->get_next();
		}//end-while: Current examined row

		// Matches or not, end current line.
#ifdef A3_FLUSH_LESS
		std::cout << '\n';
#else
		std::cout << std::endl;
#endif // A3_FLUSH_LESS
	}//end-for: Each input "value to search for"

#ifdef DBG_COUNT_ZERO_SUMS
	std::cerr << "\nZERO HITS: " << dbg_zero_count << std::endl;
#endif



	////////
	//// Clean up resources.
	////////
#ifndef A3_NO_MEMFREE_AT_END
	curr_row_head_probe = sparse_row_head;
	while (curr_row_head_probe != NULL)
	{
		linked_ptr_node<d2_int_node_data>* curr_col_probe = curr_row_head_probe->get_data();

		while (curr_col_probe != NULL)
		{
			linked_ptr_node<d2_int_node_data>* del_node = curr_col_probe;
			curr_col_probe = curr_col_probe->get_next();
			delete del_node;
		}//end-while: Current examined column

		d2_linked_ptr_node* del_node = curr_row_head_probe;
		curr_row_head_probe = curr_row_head_probe->get_next();
		delete del_node;
	}
#endif
}