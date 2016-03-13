#include <iostream>

/* Normally, the code would be more modular and include a .h header file, 
 * but you requested "a single cpp file."
 *
 * THERE IS MINIMAL INPUT VALIDATION/ERROR HANDLING
 * due to strict specification on input/output.
 * Any deviance from specified input will lead to UNDEFINED BEHAVIOUR.
 * Note to self: This also means no assertions.
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

class single_linked_list_node 
{
private:
	int data; // Designated read-only.
public:
	single_linked_list_node* next;

	single_linked_list_node(int param_data, single_linked_list_node* param_next = NULL): 
		data(param_data), next(param_next){};

	int get_data() const { return this->data; };
};

int main(int argc, char** argv)
{
	// Expected input: "the number of elements in the initial list"
	int init_list_size;
	std::cin >> init_list_size;

	// Expected input: "the elements in the initial list"
	single_linked_list_node* init_head;
	if (init_list_size <= 0) // If, for some reason, there will be no elements in the initial list, 
	{ // Acknowledge an empty linked list.
		//TODO//NOTE//WARN//MULT What happens with empty lists? Is a blank line input or not?
		init_head = NULL;
	}
	else // Initial list stated to contain elements.
	{ // Input list of elements.
		int usrin_int;
		std::cin >> usrin_int;
		init_head = new single_linked_list_node(usrin_int);
		single_linked_list_node* init_tail = init_head;
		// init_list_size - 1 because first element already recorded.
		for (int i = 0; i < init_list_size - 1; ++i)
		{
			std::cin >> usrin_int;
			init_tail->next = new single_linked_list_node(usrin_int);
			init_tail = init_tail->next;
		}
	}

	// Expected input: "number of additional lists"
	int addl_list_count;
	std::cin >> addl_list_count;

	for (int i_addlist = 0; i_addlist < addl_list_count; ++i_addlist)
	{
		// Expected input: "Additional List #i: step"
		int addl_list_step;
		std::cin >> addl_list_step;

		// Expected input: "Additional List #i: number of elements"
		int addl_list_size;
		std::cin >> addl_list_size;

		// Expected input: "Additional List #i: the elements"
		//NOTE Guaranteed precondition: Valid Additional List size & step that will fit into the initial list.
		if (addl_list_size <= 0) // If, for some reason, there will be no elements in the Additional List, 
		{
			//TODO//NOTE//WARN//MULT What happens with empty lists? Is a blank line input or not?
			continue; // There is nothing to do.
		}
		// else Additional List stated to contain elements
		int usrin_int;
		if (init_head == NULL) // If, for some reason, there were no elements in the initial list, 
		{ // Write Additional List directly to the empty initial list.
			init_head = new single_linked_list_node(usrin_int);
			single_linked_list_node* init_tail = init_head;
			// addl_list_size - 1 because first element already recorded.
			for (int i = 0; i < addl_list_size - 1; ++i)
			{
				std::cin >> usrin_int;
				init_tail->next = new single_linked_list_node(usrin_int);
				init_tail = init_tail->next;
			}
		}
		else // Initial list contains elements.
		{
			single_linked_list_node* init_probe = init_head;
			for (int i = 0; i < addl_list_size; ++i)
			{
				// "addl_list_step - 1" used since init_probe is always set to the first "step" prior to each loop iteration.
				//NOTE//WARN addl_list_step == /(0|1)/ TREATED THE SAME.
					// Remedy: Do not perform above "first 'step'" initialisation
					//NOTE Discussion Quote: (Fardad Jalili Talchegah@25 Jan 4:16p EST) "Your codes are not going to be tested with ... step equal to zero."
				for (int i_step = 0; i_step < addl_list_step - 1; ++i_step)
					init_probe = init_probe->next;

				std::cin >> usrin_int;
				init_probe->next = new single_linked_list_node(usrin_int, init_probe->next);
				init_probe = init_probe->next->next;
			}
		}
	}



	// Output: Post-interleave operation list
	single_linked_list_node* init_probe = init_head;
	if (init_probe != NULL)
	{
		std::cout << init_probe->get_data();
		init_probe = init_probe->next;
		while (init_probe != NULL)
		{
			std::cout << " " << init_probe->get_data();
			init_probe = init_probe->next;
		}
	}
	std::cout << std::endl;



	// Cleanup (by repeatedly deleting the head node)
	single_linked_list_node* init_dtor = init_head->next;
	while (init_head != NULL)
	{
		delete init_head;
		init_head = init_dtor;
		if (init_dtor != NULL) init_dtor = init_dtor->next;
	}
	return 0;
}