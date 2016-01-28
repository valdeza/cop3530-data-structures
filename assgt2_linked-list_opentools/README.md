# testcase_autogen
*A test case generator for that thing due Fries Day (29 Jan), because auto-grader paranoia and there's no way the graders are going to reveal their hand*

Initial author: C. Martin  
Documentation: A. Valdez

## Warnings
* There is currently no guarantee that testcase_autogen will create test cases that can be processed in a reasonable time  
(i.e. This might make test cases that are too large)
* Test cases *may or may not be* 

## How to Use
1. Run the code to generate a file.  
(Make sure you're able to write to input.txt in the current working directory! Soon(tm): Ability for you to specify your own file name)
2. Use the resulting file as follows:

	./prgm_name < input.txt

## Output Specification
* Currently, you cannot define the maximum possible test case size.
* Following each line of numbers is a line break; no whitespace.
* Each line is not preceded with a tab, the documentation only does so for comments readability.
* Below, all comments are denoted by "//" and will not appear in the actual output.
  * Lines beginning with "//" are only notes and no line is printed to the file at all.

\>input.txt (File name currently not modifiable)

		n//n:[0,100) "number of elements in initial list"
		m_0 m_1 m_2 ... m_n//$n space-delimited "elements in the initial list", each set to m_i:[0,200)
		a//a:[0,20) "number of additional lists"
	//repeat section $n times
		s//s:[0,4] Additional List: "step"
		b//b:[0,19) Additional List: "number of elements"
		l_0 l_1 l_2 ... l_b//Additional List: $b space-delimited "elements", each set to l_i:[0,200)
	//end-repeat