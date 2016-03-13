# testcase_autogen
*A test case generator for that thing due Fries Day (29 Jan),  
because auto-grader paranoia and there's no way the graders are going to reveal their hand*

Initial author: C. Martin  
Modified and documented by: A. Valdez

## Notes
The "default /[1-3]/.in.txt" files can be substituted for using testcase_autogen ;  
these files are the inputs we have already been given in the assignment.

Also, all commands stated are intended to be used in bash shell.  
Please refer to your shell's documentation for equivalent syntax.

## Warnings
* testcase_autogen does not check for validity of generated test cases,  
but *should usually* produce a valid test case unless you choose to specify your own limiter values.

## How to Use
### Optional Arguments
(See print_help() in testcase_autogen.cpp)

### Method 1: Direct Input
	./testcase-autogen $(args) | ./assgt2_prgm

### Method 2: Using File Buffer
\**Note: This method allows you to view your last-used test case. This is useful if your program fails to process the test case.*

	./testcase-autogen $(args) > testcase.txt; ./assgt2_prgm < testcase.txt

## Output Specification
* Following each line of numbers is a line break; no whitespace.
* Each line is not preceded with a tab, the documentation only does so for comments readability.
* Below, all comments are denoted by "//" and will not appear in the actual output.
  * Lines beginning with "//" are only notes and no line is printed to the file at all.

\>stdout

		n//n:[1,n(=100)] "number of elements in initial list"
		m_0 m_1 m_2 ... m_n//$n space-delimited "elements in the initial list"
		a//a:[1,a(=20)] "number of additional lists"
	//repeat section $n times
		s//s:[1,s(=5)] Additional List: "step"
		b//b:[1,b(=19)] Additional List: "number of elements"
		l_0 l_1 l_2 ... l_b//Additional List: $b space-delimited "elements"
	//end-repeat
