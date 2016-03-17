// This is the original, submitted work to Canvas.
// I thought about cleaning it, but decided not to because it feels like rewriting history.
// It has preprocessor directives up the wazoo (/engineertalk) because I thought we're presenting our code.
// Grading TA did not allow me to refer to the code at all
// so.. hope this thing littered with "speaker's notes" won't bother them too bad..(??)

//// COMPILE FLAGS
////////
#define DBG_ENABLED // Comment out to disable debug mode entirely.
#ifdef DBG_ENABLED
	#define DBG_PRINT_TOKEN_ID // As each token is read in, print what it has been identified as.
	#define DBG_PRINT_EACH_OUTQUEUE // As each token from out_queue is read, print the resulting token.
	#define DBG_PRINT_SEPARATOR // Makes verbose output easier to read.
#endif

//WARN
// ufnix g++ is outdated BY 0.1 VERSION DIFFERENCE
//#define REGEX_COMPILER_SUPPORTED

//NOTE//WARN *Seems* to work
#define PA1_USE_MY_HASHMAP // Comment out to quickly revert to <unordered_map>



//// INCLUDES
////////
#include <iostream>
#ifdef REGEX_COMPILER_SUPPORTED
	#include <regex> // Tokenises user input
#else
	#include <list> // Used in tokenising user input
	#define __LIST_H_INCLUDED__
#endif
#include <string>
#include <queue> 
#include <stack> 
#include <stdexcept>
#include <cmath> // std::pow(), std::abs()

#ifdef PA1_USE_MY_HASHMAP
	#include <functional> // std::hash()()
	#include <vector> // Used in personal hashmap implementation
	#ifndef __LIST_H_INCLUDED__
		#include <list>
	#endif
#else
	#include <unordered_map>
#endif



//// NOTES, REMARKS
////////
// Sources:
// Reverse Polish notation ("postfix") evaluation : https://en.wikipedia.org/w/index.php?title=Reverse_Polish_notation&oldid=706043003#Postfix_algorithm
// Converting infix to postfix : https://en.wikipedia.org/w/index.php?title=Shunting-yard_algorithm&oldid=705794803#The_algorithm_in_detail
// 

/* Normally, the code would be more modular and include a .h header file,
* but you requested "a single cpp file."
*
* INPUT VALIDATION/ERROR HANDLING IS NOT EXTENSIVE
* due to strict specification on input/output.
* Any deviance from specified input might lead to UNDEFINED BEHAVIOUR.
*/

//NOTE This will be demonstrated to graders. Leave debug flags for demonstration.

//WARN This implementation assumes absolutely no syntax errors, as per project assignment .pdf detail. For this reason, error handling is not extensive.

/* Things to probably show off during the presentation:
 * * Error-checking against user input and my own code.
 * * The debug flags at the top of this code file.
 * * How <regex> is amazing.. if supported.
 * * Too many comments: extensive code documentation.
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



//// FORWARD DECLARATIONS
////////
//TODO This. Also, rearranging definitions.
struct equation_token
{
#ifdef DBG_PRINT_TOKEN_ID
private:
	//NOTE Array initialisation might be convoluted due to Visual Studio non-compliance with the C++ standard.
	/* Reason: Array definition rewritten in response to rumours of instructor using Visual Studio. */
	static const std::string* ASTR_ENUM_TOKEN_TYPE;
	static const std::string* ASTR_ENUM_OPERATOR_TYPE;
	static const std::string* ASTR_ENUM_KEYWORDS;
#endif

public:
	enum enum_token_type : int
	{
		ERR_UNKNOWN = 0, // No token must ever have this value.
		OPERATOR = 1,
		KEYWORD,
		VARIABLE,
		NUMBER
	};

	//MULT["OpOrderDep"]//WARN Co-tagged code is dependent on the ordering of this enum!
	enum enum_operator_type : int
	{
		ERR_OPERATOR_UNINIT = 0, // Accessing this value is the result of inappropriate context (wrong ::TOKEN_TYPE()?).
		ADD = 1,
		SUB,
		MULT,
		DIV,
		EXP,
		LPAREN,
		RPAREN,
		EQUALS
	};

	enum enum_keywords : int
	{
		// Commented out due to redefinition.
		ERR_KEYWORD_UNINIT = 0, // Accessing this value is the result of inappropriate context (wrong ::TOKEN_TYPE()?).
		LET = 1,
		QUIT
	};

private: // Made private for readonly access; cannot use initialiser list.
	std::string m_STRING;
		/* Normally, this can be const. But this would complicate use of '=' operator.*/
	equation_token::enum_token_type m_TOKEN_TYPE = equation_token::enum_token_type::ERR_UNKNOWN;
	/// <summary>For OPERATOR and KEYWORD token types, stores the specialised identifier</summary>
	int m_PREDEF_TOKEN_ID = 0;
	double m_NUMBER;

public:
	equation_token(std::string str);

	inline std::string STRING() const
		{ return this->m_STRING; }

	inline equation_token::enum_token_type TOKEN_TYPE() const
		{ return this->m_TOKEN_TYPE; }

	/// <summary>Warning: Only for KEYWORD token types. Check ::TOKEN_TYPE() before using this accessor.</summary>
	inline equation_token::enum_keywords get_keyword() const
		{ return (equation_token::enum_keywords)this->m_PREDEF_TOKEN_ID; }

	/// <summary>Warning: Only for NUMBER token types. Check ::TOKEN_TYPE() before using this accessor.</summary>
	inline double get_number() const
		{ return this->m_NUMBER; }

	/// <summary>Warning: Only for OPERATOR token types. Check ::TOKEN_TYPE() before using this accessor.</summary>
	inline equation_token::enum_operator_type get_operator_type() const
		{ return (equation_token::enum_operator_type)this->m_PREDEF_TOKEN_ID; }

#ifdef DBG_PRINT_TOKEN_ID
	inline std::string TOKEN_TYPE_STR() const
		{ return ASTR_ENUM_TOKEN_TYPE[this->m_TOKEN_TYPE]; }

	/// <summary>Warning: Only for KEYWORD token types. Check ::TOKEN_TYPE() before using this accessor.</summary>
	inline std::string get_keyword_str() const
		{ return ASTR_ENUM_KEYWORDS[this->m_PREDEF_TOKEN_ID]; }

	/// <summary>Warning: Only for OPERATOR token types. Check ::TOKEN_TYPE() before using this accessor.</summary>
	inline std::string get_operator_type_str() const
		{ return ASTR_ENUM_OPERATOR_TYPE[this->m_PREDEF_TOKEN_ID]; }
#endif

	/// <summary>
	/// Returns the relative precedence of <paramref name="tok_op1" /> in relation to <paramref name="tok_op2" /> a la standard operator precedence rules.
	/// <para>If of the same precedence priority, false will be returned.</para>
	/// <para>Comparing an LPAREN, RPAREN, or EQUALS will throw an exception.</para>
	/// </summary>
	/// <returns>
	/// -1 - tok_op1 is of greater precedence than tok_op2
	///  0 - tok_op1 is of the same precedence of tok_op2
	///  1 - tok_op1 is of lesser precedence than tok_op2
	/// </returns>
	/// <exception cref="System.ArgumentException">Thrown when using an LPAREN, RPAREN, EQUALS, or otherwise nonexistent enum member.</exception>
	/// <remarks>//MULT["OpOrderDep"]//WARN Dependent on the element ordering of equation_token::enum_operator_type!</remarks>
	static int compare_operator_precedence(const equation_token::enum_operator_type top_op1, const equation_token::enum_operator_type tok_op2);

	/// <summary>
	/// Only meant to be used to evaluate ADD, SUB, MULT, DIV, or EXP.
	/// <para>An exception will be thrown, otherwise.</para>
	/// </summary>
	/// <exception cref="System.ArgumentException">Thrown when using an unspecified enum member.</exception>
	/// <remarks>
	/// //MULT["OpOrderDep"]//WARN Dependent on the element ordering of equation_token::enum_operator_type!
	/// Aside from bounds checking, basically only queries if <paramref name="op" /> is EXP and stating "right associative", if so.
	/// </remarks>
	static bool is_left_associative_operator(const equation_token::enum_operator_type op);
};



//// DEFINITIONS
////////
static const int PERMITTED_VARNAME_CHAR_COUNT = 52; // 26 lower + 26 upper
static const int ASCII_CODE_A = 65;
static const int ASCII_CODE_Z = 90;
static const int ASCII_CODE_a = 97;
static const int ASCII_CODE_z = 122;

#ifdef PA1_USE_MY_HASHMAP
struct pa1_strtod_hashmap_node
{
private:
	std::string m_key;
	double* m_value;
public:

	pa1_strtod_hashmap_node(std::string key, double value) : m_key(key), m_value(new double(value)){};
	//inline ~pa1_strtod_hashmap_node()
	//	{ delete m_value; }
	////dbg//NOTE//WARN Why does this destructor cause errors?

	inline std::string key() const
		{ return this->m_key; }

	inline double& value() const { return *this->m_value; }
};

/// <summary>Personal implementation of a hashmap for project assignment 1. Features rehashing to attempt efficiency maintenance.</summary>
/// <remarks>Written to implement the bare minimum necessary for this codefile to "use" an STL unordered_map-like class.</remarks>
class pa1_strtod_hashmap
{
public:
	static const int DEFAULT_BUCKET_COUNT = 10;
	static const int BUCKET_GROWTH_RATE = 2;
private:
	std::vector<pa1_strtod_hashmap_node>* m_buckets = new std::vector<pa1_strtod_hashmap_node>[DEFAULT_BUCKET_COUNT];
	size_t m_bucket_count = this->DEFAULT_BUCKET_COUNT;
	float m_max_load_factor = 1.0;
	size_t m_size = 0;

	inline int hash(const std::string s) const
		{ return std::hash<std::string>()(s) % this->bucket_count(); }

	inline double* const insert__post_duplicate_check(pa1_strtod_hashmap_node n)
	{
		this->m_buckets[this->hash(n.key())].push_back(n);
		++this->m_size;

		if (this->load_factor() > this->max_load_factor())
			this->rehash(this->size() * 2);

		return this->find(n.key());
	}
public:
	~pa1_strtod_hashmap()
	{
		delete[] this->m_buckets;
	}

	inline size_t bucket_count() const
		{ return this->m_bucket_count; }

	inline float max_load_factor() const 
		{ return this->m_max_load_factor; }
	inline void max_load_factor(const float new_val) 
		{ this->m_max_load_factor = new_val; }

	inline size_t size() const 
		{ return this->m_size; }

	inline float load_factor() const
		{ return (float)this->size() / (float)this->bucket_count(); }

	void rehash(const size_t new_bucket_count)
	{
		std::list<pa1_strtod_hashmap_node> holding;
		// Insert every element of the buckets into this temporary holding list.
		for (size_t i = 0; i < this->bucket_count(); ++i)
			holding.insert<std::vector<pa1_strtod_hashmap_node>::const_iterator>(holding.end(), this->m_buckets[i].cbegin(), this->m_buckets[i].cend());

		// Reinitialise buckets.
		delete[] this->m_buckets;
		this->m_size = 0;
		this->m_bucket_count = new_bucket_count;
		this->m_buckets = new std::vector<pa1_strtod_hashmap_node>[this->bucket_count()];

		// Reinsert every element.
		for (std::list<pa1_strtod_hashmap_node>::const_iterator iter = holding.cbegin(); iter != holding.cend(); ++iter)
			this->operator[](iter->key()) = iter->value();
	}

	double* find(const std::string s) const // Pointer is constant, pointee is not.
	{
		std::vector<pa1_strtod_hashmap_node>& target = this->m_buckets[this->hash(s)];
		for (std::vector<pa1_strtod_hashmap_node>::iterator iter = target.begin(); iter != target.end(); ++iter)
			if (iter->key().compare(s) == 0)
				return &iter->value();
		//else s not found
		return this->end();
	}

	/// <summary>Returns a reference to the value that is mapped to a key equivalent to <paramref name="s" />.</summary>
	double& operator[](std::string s)
	{ 
		double* find_result = this->find(s);
		if (find_result == this->end())
			find_result = this->insert__post_duplicate_check(pa1_strtod_hashmap_node(s, 0.0));
		return *find_result;
	}

	/// <summary>Inserts a new element into this hashmap. No duplicate keys allowed.</summary>
	/// <exception cref="System.ArgumentException">Thrown when duplicate key insertion is attempted.</exception>
	double* insert(pa1_strtod_hashmap_node n)
	{
		if (this->find(n.key()) != this->end())
			throw std::invalid_argument("ERROR: Duplicate key inserted into hashmap!");
		else
			return insert__post_duplicate_check(n);
	}

	inline double* end() const 
		{ return NULL; }
};
#endif

/// <param name="d">Some double to evaluate whether "close enough" to zero</param>
inline bool is_near_zero(const double d)
{
	static const double epsilon = 1e-10;
	return std::abs(d) <= epsilon;
}

#ifdef DBG_PRINT_TOKEN_ID
static const std::string ASTR_ENUM_TOKEN_TYPE_DEF[] = {
		"ERR_UNKNOWN",
		"OPERATOR",
		"KEYWORD",
		"VARIABLE",
		"NUMBER"
	};
const std::string* equation_token::ASTR_ENUM_TOKEN_TYPE = ASTR_ENUM_TOKEN_TYPE_DEF;

static const std::string ASTR_ENUM_OPERATOR_TYPE_DEF[] = {
		"ERR_UNINIT",
		"ADD",
		"SUB",
		"MULT",
		"DIV",
		"EXP",
		"LPAREN",
		"RPAREN",
		"EQUALS"
	};
const std::string* equation_token::ASTR_ENUM_OPERATOR_TYPE = ASTR_ENUM_OPERATOR_TYPE_DEF;

static const std::string ASTR_ENUM_KEYWORDS_DEF[] = {
		"ERR_UNINIT",
		"LET",
		"QUIT"
	};
const std::string* equation_token::ASTR_ENUM_KEYWORDS = ASTR_ENUM_KEYWORDS_DEF;
#endif

equation_token::equation_token(std::string str) : m_STRING(str)
{
	//NOTE The premature assumptions below are partially to avoid typing the same line of code multiple times.
	char c_probe = str.at(0);

	// Premature assumption: ::OPERATOR, will be overwritten if false.
	this->m_TOKEN_TYPE = equation_token::enum_token_type::OPERATOR;
	switch (c_probe)
	{
		case '+': 
		{ // Handles unary operator appendage.. just in case.
			if (str.length() == 1)
			{
				this->m_PREDEF_TOKEN_ID = equation_token::enum_operator_type::ADD;
				return;
			}
			//else
			c_probe = str.at(1);
			break; // Re-evaluate. This might be a number.
		}
		case '-': 
		{ // Handles unary operator appendage.. just in case.
			if (str.length() == 1)
			{
				this->m_PREDEF_TOKEN_ID = equation_token::enum_operator_type::SUB;
				return;
			}
			//else
			c_probe = str.at(1);
			break; // Re-evaluate. This might be a number.
		}
		case '*': 
		{
			this->m_PREDEF_TOKEN_ID = equation_token::enum_operator_type::MULT;
			return;
		}
		case '/': 
		{
			this->m_PREDEF_TOKEN_ID = equation_token::enum_operator_type::DIV;
			return;
		}
		case '^': 
		{
			this->m_PREDEF_TOKEN_ID = equation_token::enum_operator_type::EXP;
			return;
		}
		case '(': 
		{
			this->m_PREDEF_TOKEN_ID = equation_token::enum_operator_type::LPAREN;
			return;
		}
		case ')': 
		{
			this->m_PREDEF_TOKEN_ID = equation_token::enum_operator_type::RPAREN;
			return;
		}
		case '=': 
		{
			this->m_PREDEF_TOKEN_ID = equation_token::enum_operator_type::EQUALS;
			return;
		}
		default: break;
	}

	if (48 <= c_probe && c_probe <= 57) // Is c_probe a digit?
	{ // str is a number
		this->m_TOKEN_TYPE = equation_token::enum_token_type::NUMBER;
		this->m_NUMBER = std::stod(this->m_STRING);
		return;
	}
	//else

	if ((ASCII_CODE_A <= c_probe && c_probe <= ASCII_CODE_Z) || (ASCII_CODE_a <= c_probe && c_probe <= ASCII_CODE_z)) // Is c_probe a letter?
	{ // str is either a variable or keyword
		// Premature assumption: ::KEYWORD, will be overwritten if false.
		this->m_TOKEN_TYPE = equation_token::enum_token_type::KEYWORD;
		if (str.compare("let") == 0)
		{
			this->m_PREDEF_TOKEN_ID = equation_token::enum_keywords::LET;
			return;
		}
		else if (str.compare("quit") == 0)
		{
			this->m_PREDEF_TOKEN_ID = equation_token::enum_keywords::QUIT;
			return;
		}
		//else
		this->m_TOKEN_TYPE = equation_token::enum_token_type::VARIABLE;
		return;
	}
	//else

	// If this point has been reached, the promise to never have syntax errors has probably been broken.
	this->m_TOKEN_TYPE = equation_token::enum_token_type::ERR_UNKNOWN;
}


/// <summary>
/// Returns the relative precedence of <paramref name="tok_op1" /> in relation to <paramref name="tok_op2" /> a la standard operator precedence rules.
/// <para>If of the same precedence priority, false will be returned.</para>
/// <para>Comparing an LPAREN, RPAREN, or EQUALS will throw an exception.</para>
/// </summary>
/// <returns>
/// -1 - tok_op1 is of greater precedence than tok_op2
///  0 - tok_op1 is of the same precedence of tok_op2
///  1 - tok_op1 is of lesser precedence than tok_op2
/// </returns>
/// <exception cref="System.ArgumentException">Thrown when using an LPAREN, RPAREN, EQUALS, or otherwise nonexistent enum member.</exception>
/// <remarks>//MULT["OpOrderDep"]//WARN Dependent on the element ordering of equation_token::enum_operator_type!</remarks>
int equation_token::compare_operator_precedence(const equation_token::enum_operator_type tok_op1, const equation_token::enum_operator_type tok_op2)
{
	/* ints used to emphasise enum ordering*/

	// Precondition: tok_op1 and tok_op2 not LPAREN, RPAREN, EQUALS, or out of bounds.
	if (tok_op1 == 0 || (6 <= tok_op1 && tok_op1 <= 8))
		throw std::invalid_argument("tok_op1 is invalid for precedence evaluation.");
	if (tok_op1 < 0 || 8 < tok_op1)
		throw std::out_of_range("ERROR: tok_op1 out of enum_operator_type bounds!!");
	if (tok_op2 == 0 || (6 <= tok_op2 && tok_op2 <= 8))
		throw std::invalid_argument("tok_op2 is invalid for precedence evaluation.");
	if (tok_op2 < 0 || 8 < tok_op2)
		throw std::out_of_range("ERROR: tok_op2 out of enum_operator_type bounds!!");
	//else
	// Postcondition: tok_op1 and tok_op2 guaranteed to be valid. No need for range checking.


	// Begin comparisons.

	if (tok_op1 == 5) // tok1 is an EXP
	{
		if (tok_op2 < 5) // and tok2 is below EXP
			return -1;
		else if (tok_op2 == 5) // and tok2 is an EXP, too
			return 0;
	}
	//else

	if (tok_op1 == 4 || tok_op1 == 3) // tok1 is a DIV or MULT
	{
		if (tok_op2 < 3) // and tok2 is below DIV and MULT
			return -1;
		else if (tok_op2 == 3 || tok_op2 == 4) // and tok2 is DIV or MULT, too
			return 0;
		else if (tok_op2 == 5) // and tok2 is an EXP
			return 1;
	}
	//else

	if (tok_op1 == 2 || tok_op1 == 1) // tok1 is a SUB or ADD
	{
		if (tok_op2 == 2 || tok_op2 == 1) // tok2 is a SUB or ADD, too
			return 0;
		else if (tok_op2 > 2) // tok2 is above SUB and ADD
			return 1;
	}

	// This point should never be reached.
	throw std::logic_error("UNKNOWN ERROR: Failure to evaluate/validate tok_op1 or tok_op2 in equation_token::compare_operator_precedence(...)!!");
}

/// <summary>
/// Only meant to be used to evaluate ADD, SUB, MULT, DIV, or EXP.
/// <para>An exception will be thrown, otherwise.</para>
/// </summary>
/// <exception cref="System.ArgumentException">Thrown when using an unspecified enum member.</exception>
/// <remarks>
/// //MULT["OpOrderDep"]//WARN Dependent on the element ordering of equation_token::enum_operator_type!
/// Aside from bounds checking, basically only queries if <paramref name="op" /> is EXP and stating "right associative", if so.
/// </remarks>
bool equation_token::is_left_associative_operator(const equation_token::enum_operator_type op)
{
	/* ints used to emphasise enum ordering*/

	// Precondition: op not LPAREN, RPAREN, EQUALS, or out of bounds.
	if (op == 0 || (6 <= op && op <= 8))
		throw std::invalid_argument("tok_op1 is invalid for precedence evaluation.");
	if (op < 0 || 8 < op)
		throw std::out_of_range("ERROR: tok_op1 out of enum_operator_type bounds!!");
	//else
	// Postcondition: op guaranteed to be valid. No need for range checking.


	// Begin heavy lifting.
	return op != equation_token::enum_operator_type::EXP;

	// This point should never be reached.
	throw std::logic_error("UNKNOWN ERROR: Failure to evaluate/validate op in equation_token::is_left_associative_operator(...)!!");
}

/// <summary>Helper function for (int)main(...). Extracted to eliminate redundancy.<para>FOR USE IN IMPLEMENTATION ONLY</para></summary>
void main__process_operator(const char& ci, equation_token::enum_token_type& curr_tok_type, std::string& str_build, bool& export_str_to_listqueue, bool& suppress_iteration)
{
	if (curr_tok_type == equation_token::enum_token_type::ERR_UNKNOWN) // First character of current string build?
	{
		str_build.append(1, ci);
		curr_tok_type = equation_token::enum_token_type::OPERATOR;
	}
	else
	{
		suppress_iteration = true;
	}
	export_str_to_listqueue = true;
}

#ifdef DBG_PRINT_TOKEN_ID
/// <summary>Debug helper function for (int)main(...). Extracted to eliminate redundancy.<para>FOR USE IN IMPLEMENTATION AND DEBUGGING ONLY</para></summary>
void main__DBG_PRINT_TOKEN_ID(const equation_token& eqtok)
{
	std::clog << eqtok.STRING() << " : " << eqtok.TOKEN_TYPE_STR();
	switch (eqtok.TOKEN_TYPE())
	{
		case equation_token::enum_token_type::KEYWORD:
		{
			std::clog << "." << eqtok.get_keyword_str();
			break;
		}
		case equation_token::enum_token_type::OPERATOR:
		{
			std::clog << "." << eqtok.get_operator_type_str();
			break;
		}
		default: break;
	}
	std::clog << std::endl;
}
#endif

int main(int argc, char** argv)
{
#ifdef DBG_ENABLED
	std::clog << "DEBUG MODE ENABLED! ACTIVE FLAGS:\n";
#ifdef DBG_PRINT_TOKEN_ID
	std::clog << "DBG_PRINT_TOKEN_ID\n";
#endif
#ifdef DBG_PRINT_EACH_OUTQUEUE
	std::clog << "DBG_PRINT_EACH_OUTQUEUE\n";
#endif
#ifdef DBG_PRINT_SEPARATOR
	std::clog << "DBG_PRINT_SEPARATOR\n";
#endif
	std::clog << std::endl;
#endif

#ifdef REGEX_COMPILER_SUPPORTED
	// Define token regex definition
	const std::regex TOKENDEF("\\+|\\-|\\*|\\/|\\^|\\(|\\)|=|[a-zA-Z]+|\\d+(\\.\\d+)?");
		/* Explanation:
		 *	* Each expected token format is separated by |, meaning OR
		 *	* All backslashes are doubled to include a backslash as part of the string. In regex, this escapes the succeeding normally-reserved character.
		 *	* Part of the beauty of regex is that whitespace/line breaks do not matter unless you specify so.
		 
		 * \\+|\\-|\\*|\\/|\\^|\\(|\\)|=|[a-zA-Z]+|\\d+(\\.\\d+)?
		 * -----------------------------|---------|----\---------
		 *              (1)                 (2)     (3)     (4)
		 *	(1) Tokenise on operators '+', '-', '*', '/', '^', '(', ')', '='
		 *	(2) Tokenise on case-insensitive words ([a-zA-Z]) of at least one letter (+).
		 *		> (This includes "quit" and "let", but those can be tested for exclusively)
		 *	(3) Tokenise on numbers (\\d) of at least one digit (+).
		 *	(4) But if a decimal point is encountered (\\.), include the following digits as part of the number (\\d+). Decimals are optional, though (?).
		 */
#endif

#ifdef PA1_USE_MY_HASHMAP
	pa1_strtod_hashmap usrdef_var_storage;
#else
	std::unordered_map<std::string, double> usrdef_var_storage;
#endif

	while (!std::cin.eof())
	{
#ifdef DBG_PRINT_SEPARATOR
		std::clog << "\n";
		for (int i = 0; i < 30; ++i)
			std::clog << "=";
		std::clog << std::endl;
#endif

		////
		//// INITIALISE VARIABLES
		////

		std::string usrin_currline;
		std::getline(std::cin, usrin_currline);
		std::queue<equation_token> out_queue;
		std::stack<equation_token> op_stack;
		// true - Pure expression evaluation
		// false - evaluation will be assigned to variable.
		bool is_var_assign_mode = false; 
		std::string var_to_assign = "";
		bool is_premature_line_parse_break = false;
#ifdef REGEX_COMPILER_SUPPORTED
		std::sregex_iterator token_iterator(usrin_currline.begin(), usrin_currline.end(), TOKENDEF);
		std::sregex_iterator token_iterator_end;
#else
		std::list<std::string> tok_list; // std::list used to retain iterator-using code.



		//// 
		//// TOKENISE INPUT
		////
		// (if <regex> unsupported)
		std::string str_build = "";
		equation_token::enum_token_type curr_tok_type = equation_token::enum_token_type::ERR_UNKNOWN;
		bool suppress_iteration = false;
		bool export_str_to_listqueue = false;
		for (std::string::const_iterator ci = usrin_currline.cbegin(); export_str_to_listqueue || ci != usrin_currline.cend(); suppress_iteration ? ci : ++ci) // std::list supports ++ AND -- operators (backpedalling).
		{
			if (export_str_to_listqueue)
			{
				tok_list.push_back(str_build);

				// Break if this is the final token.
				if (ci == usrin_currline.cend()) break;

				// Reset temporary holding variables.
				str_build = "";
				curr_tok_type = equation_token::enum_token_type::ERR_UNKNOWN;
				export_str_to_listqueue = false;
			}

			suppress_iteration = false;

			if (*ci == ' ') // If whitespace, 
				continue; // ignore

			// Operators are guaranteed single-character tokens during input phase
			switch (*ci)
			{
				case '+':
				case '-':
				case '*':
				case '/':
				case '^':
				case '(':
				case ')':
				case '=':
				{
					main__process_operator(*ci, curr_tok_type, str_build, export_str_to_listqueue, suppress_iteration);
					continue;
				}
				default: break;
			}//end-switch: Operator tokenisation

			bool
				is_token_id_captured = false,
				encounteredPeriod = false;
			while ((48 <= *ci && *ci <= 57) || *ci == '.') // Try token as a number
			{
				is_token_id_captured = true;

				if (*ci == '.')
				{
					if (encounteredPeriod)
					{
						std::cerr << "Syntax error: Unexpected '.'" << std::endl;
						is_premature_line_parse_break = true;
						break;
					}
					else
					{
						encounteredPeriod = true;
						str_build.append(".");
					}
				}
				else
				{
					str_build.append(1, *ci);
				}

				if (++ci == usrin_currline.cend()) break; // Input stream exhausted.
			}
			if (is_premature_line_parse_break)
				break;
			if (is_token_id_captured)
			{
				suppress_iteration = true;
				export_str_to_listqueue = true;
				continue;
			}

			while ((ASCII_CODE_A <= *ci && *ci <= ASCII_CODE_Z) || (ASCII_CODE_a <= *ci && *ci <= ASCII_CODE_z)) // Try token as a string.
			{
				is_token_id_captured = true;
				str_build.append(1, *ci);
				if (++ci == usrin_currline.cend()) break; // Input stream exhausted.
			}
			if (is_token_id_captured)
			{
				suppress_iteration = true;
				export_str_to_listqueue = true;
				continue;
			}

			// Ignore '\r', '\f', '\n', '\0'
			switch (*ci)
			{
				case '\r':
				case '\f':
				case '\n':
				case '\0': continue;
				default: // Otherwise unknown character?
				{
					std::cerr << "Syntax error: Unknown character '" << *ci << "'" << std::endl;
					break;
				}
			}
		}
		if (is_premature_line_parse_break)
			continue;
		std::list<std::string>::const_iterator
			token_iterator = tok_list.cbegin(),
			token_iterator_end = tok_list.cend();
#endif



		////
		//// BEGIN LOOP: HANDLE LINE INPUT
		////
#ifdef DBG_PRINT_TOKEN_ID
		std::clog << "\nTOKEN STREAM:" << std::endl;
#endif
#ifdef REGEX_COMPILER_SUPPORTED
		for (std::sregex_iterator i = token_iterator; i != token_iterator_end; ++i)
		{
			equation_token eqtok((*i).str());
#else
		for (std::list<std::string>::const_iterator i = token_iterator; i != token_iterator_end; ++i)
		{
			equation_token eqtok(*i);
#endif

#ifdef DBG_PRINT_TOKEN_ID
			main__DBG_PRINT_TOKEN_ID(eqtok);
#endif



			////
			//// CONVERT INFIX TO POSTFIX
			////

			// For every infix token, build postfix via Dijkstra's shunting-yard algorithm
			if (eqtok.TOKEN_TYPE() == equation_token::enum_token_type::NUMBER) // "If the token is a number"
			{ // "then add it to the output queue."
				out_queue.push(eqtok);
				continue; // Fetch next token.
			}
			//else
			if (eqtok.TOKEN_TYPE() == equation_token::enum_token_type::VARIABLE) // Treat variables similarly to numbers.
			{
				// Check if variable has been defined yet
				auto var_search_itr = usrdef_var_storage.find(eqtok.STRING());
				if (var_search_itr == usrdef_var_storage.end()) // If not, 
				{ // Notify user and skip line.
					std::cout << "Undeclared-Variable" << std::endl;
					is_premature_line_parse_break = true;
					break;
				}
				//else

				// "then add [the 'number'] to the output queue."
				out_queue.push(eqtok);
				continue; // Fetch next token.
			}
			//else

			if (eqtok.TOKEN_TYPE() == equation_token::enum_token_type::OPERATOR)
			{
				if (eqtok.get_operator_type() == equation_token::enum_operator_type::LPAREN) // If the token is a left parenthesis
				{ // then push it onto the stack.
					op_stack.push(eqtok);
				}
				else if (eqtok.get_operator_type() == equation_token::enum_operator_type::RPAREN) // If the token is a right parenthesis
				{
					while (!op_stack.empty() && op_stack.top().get_operator_type() != equation_token::enum_operator_type::LPAREN) // "Until the token at the top of the stack is a left parenthesis,"
					{
						// "pop operators off the stack onto the output queue."
						out_queue.push(op_stack.top());
						op_stack.pop();
					}

					// "If the stack runs out without finding a left parenthesis,"
					if (op_stack.empty())
					{ // "then there are mismatched parentheses."
						std::cerr << "Syntax error: Parentheses mismatch (missing '(')" << std::endl;
						continue; // Continue reading tokens.. but parsing the rest of the line now results in undefined behaviour.
					}
					//else
					// Top of stack must be a left parenthesis.

					op_stack.pop(); // "Pop the left parenthesis from the stack, but not onto the output queue."
				}
				else if (eqtok.get_operator_type() == equation_token::enum_operator_type::EQUALS)
				{
					/* Sorry, I've been raised to never trust the user */
					std::cerr << "Syntax error: Unexpected '='" << std::endl;
					continue;  // Continue reading tokens.. but parsing the rest of the line now results in undefined behaviour.
				}
				else if (eqtok.get_operator_type() == equation_token::enum_operator_type::ERR_OPERATOR_UNINIT)
				{
					/* and not to trust myself, either. */
					throw std::logic_error("ERROR: Encountered uninitialised operator!!");
				}
				else // "If the token is an operator, o1, then:"
				{
					while (!op_stack.empty() 
						&& (op_stack.top().get_operator_type() != equation_token::enum_operator_type::LPAREN && op_stack.top().get_operator_type() != equation_token::enum_operator_type::RPAREN) // "while there is an operator o2, at the top of the operator stack and either"
						&& ((equation_token::is_left_associative_operator(eqtok.get_operator_type()) // "o1 is left-associative"
						&& equation_token::compare_operator_precedence(eqtok.get_operator_type(), op_stack.top().get_operator_type()) >= 0 // "and its precedence is less than or equal to that of o2,"
						) || ( // "or"
						!equation_token::is_left_associative_operator(eqtok.get_operator_type()) // "o1 is right-associative"
						&& equation_token::compare_operator_precedence(eqtok.get_operator_type(), op_stack.top().get_operator_type()) > 0 // "and has precedence less than that of o2."
						)))
					{
						out_queue.push(op_stack.top());
						op_stack.pop();
					}

					// "at the end of the iteration push o1 onto the operator stack."
					op_stack.push(eqtok);
				}
				continue; // Fetch next token.
			}
			//else

			//NOTE Positioned after shunting-yard algorithm due to the relatively minimal chance of encountering a keyword.
			if (eqtok.TOKEN_TYPE() == equation_token::enum_token_type::KEYWORD)
			{
				//NOTE This if-block can be extracted out of while-loop, but is kept to retain DBG_PRINT_TOKEN_ID functionality.
				if (eqtok.get_keyword() == equation_token::enum_keywords::LET)
				{
					// Acknowledge variable assignment
					is_var_assign_mode = true;
					
					// Try to safely advance iterator
					if (++i == token_iterator_end)
					{
						std::cerr << "Syntax error: Variable name expected after 'let'" << std::endl;
						is_premature_line_parse_break = true;
						break; // Cannot continue: variable assignment initialisation flow unstable.
					}
#ifdef REGEX_COMPILER_SUPPORTED
					eqtok = equation_token((*i).str());
#else
					eqtok = equation_token(*i);
#endif
#ifdef DBG_PRINT_TOKEN_ID
					main__DBG_PRINT_TOKEN_ID(eqtok);
#endif
					if (eqtok.TOKEN_TYPE() != equation_token::enum_token_type::VARIABLE)
					{
						std::cerr << "Syntax error: Variable name expected after 'let'" << std::endl;
						is_premature_line_parse_break = true;
						break; // Cannot continue: variable assignment initialisation flow unstable.
					}
					// Read variable name.
					var_to_assign = eqtok.STRING();

					// Try to safely advance iterator
					if (++i == token_iterator_end)
					{
						std::cerr << "Syntax error: '=' operator expected after variable name" << std::endl;
						is_premature_line_parse_break = true;
						break; // Cannot continue: variable assignment initialisation flow unstable.
					}
#ifdef REGEX_COMPILER_SUPPORTED
					eqtok = equation_token((*i).str());
#else
					eqtok = equation_token(*i);
#endif
#ifdef DBG_PRINT_TOKEN_ID
					main__DBG_PRINT_TOKEN_ID(eqtok);
#endif
					if (eqtok.TOKEN_TYPE() != equation_token::enum_token_type::OPERATOR
						|| eqtok.get_operator_type() != equation_token::enum_operator_type::EQUALS)
					{
						std::cerr << "Syntax error: '=' operator expected after variable name" << std::endl;
						is_premature_line_parse_break = true;
						break; // Cannot continue: variable assignment initialisation flow unstable.
					}
					// Do nothing. This just disposes of operator.

					continue; // Resume with normal expression evaluation.
				}
				else if (eqtok.get_keyword() == equation_token::enum_keywords::QUIT)
				{
					return 0; // Finished program execution signalled.
				}
				else
				{
					throw std::logic_error("ERROR: Uninitialised keyword token encountererd!!");
				}
			}
			//else

			// This point should never be reached.
			throw std::logic_error("ERROR: Unidentifiable token encountered!!");
		} // end-for: All tokens in this line has been read.
		
		if (is_premature_line_parse_break)
			continue; // Suppress additional messages for grading purposes in the event of a (probably intentional) user-caused logic error.

		// "While there are still operator tokens in the stack:"
		while (!op_stack.empty())
		{
			// If the operator token on top of the stack is a parenthesis, 
			if (op_stack.top().get_operator_type() == equation_token::enum_operator_type::LPAREN
				|| op_stack.top().get_operator_type() == equation_token::enum_operator_type::RPAREN)
			{ // "then there are mismatched parentheses."
				std::cerr << "Syntax error: Parenthesis mismatch" << std::endl;
				op_stack.pop();
			}
			else
			{ // "Pop the operator onto the output queue."
				out_queue.push(op_stack.top());
				op_stack.pop();
			}
		}

		if (out_queue.empty())
		{
			std::cerr << "Syntax error: Empty expression" << std::endl;
			continue; // Nothing to work with. Reprompt user.
		}



		////
		//// EVALUATE POSTFIX QUEUE
		////
		// Implements Postfix algorithm
		//NOTE At this point, all user-defined variables must have been defined, as all Undeclared-Variables have been vetted when tokenising user input.
#ifdef DBG_PRINT_EACH_OUTQUEUE
		std::clog << "\nOUTPUT QUEUE REPORT:" << std::endl;
#endif
		while (!out_queue.empty())
		{
			equation_token curr_tok = out_queue.front();

#ifdef DBG_PRINT_EACH_OUTQUEUE
			std::clog << "out_queue.front(): " << out_queue.front().STRING() << std::endl;
#endif

			// "If the token is a value"
			if (curr_tok.TOKEN_TYPE() == equation_token::enum_token_type::NUMBER
				|| curr_tok.TOKEN_TYPE() == equation_token::enum_token_type::VARIABLE) // Variables to be treated as numbers.
			{
				op_stack.push(curr_tok); // "Push it onto the stack."
			}
			else if (curr_tok.TOKEN_TYPE() == equation_token::enum_token_type::OPERATOR) // "Otherwise, the token is an operator"
			{
				// "It is already known that the operator takes N arguments"
				const int N = 2; 
					//NOTE All operators used in the assignment are binary operators.

				// "If there are fewer than N values on the stack"
				if (op_stack.size() < N)
				{
					// "*(Error)* The user has not input sufficient values in the expression."
					std::cerr << "Syntax error: Insufficient values for " << curr_tok.STRING() << " operator" << std::endl;
					continue; // Continue reading tokens.. but parsing the rest of the line now results in undefined behaviour.
				}
				//else

				// "Pop the top N values from the stack."
				double* op_args = new double[N];
				for (int i = N - 1; i >= 0; --i)
				{
					switch (op_stack.top().TOKEN_TYPE()) 
					{
						case equation_token::enum_token_type::NUMBER: 
						{
							op_args[i] = op_stack.top().get_number();
							break;
						}
						case equation_token::enum_token_type::VARIABLE: 
						{
							op_args[i] = usrdef_var_storage[op_stack.top().STRING()];
							break;
						}
						default: 
						{
							throw std::logic_error("ERROR: Encountered operator argument that is neither number nor variable!!");
							break; // Continue reading tokens.. but parsing the rest of the line now results in undefined behaviour.
						}
					}
					op_stack.pop();
				}
				
				// "Evaluate the operator, with the values as arguments."
				// "Push the returned results ... back onto the stack."
				switch (curr_tok.get_operator_type())
				{
					case equation_token::enum_operator_type::ADD: 
					{
						op_stack.push(equation_token(std::to_string(op_args[0] + op_args[1])));
						break;
					}
					case equation_token::enum_operator_type::SUB: 
					{
						op_stack.push(equation_token(std::to_string(op_args[0] - op_args[1])));
						break;
					}
					case equation_token::enum_operator_type::MULT: 
					{
						op_stack.push(equation_token(std::to_string(op_args[0] * op_args[1])));
						break;
					}
					case equation_token::enum_operator_type::DIV: 
					{
						if (is_near_zero(op_args[1]))
						{
							std::cout << "Division-By-Zero" << std::endl;
							is_premature_line_parse_break = true;
							break; // Avoid next push statement from causing error.
						}
						op_stack.push(equation_token(std::to_string(op_args[0] / op_args[1])));
						break;
					}
					case equation_token::enum_operator_type::EXP: 
					{
						op_stack.push(equation_token(std::to_string(std::pow(op_args[0], op_args[1]))));
						break;
					}
					default: {
						throw std::logic_error("ERROR: Encountered unknown operator!!");
						break;
					}
				}//end switch (curr_tok.get_operator_type())

				delete[] op_args;

				if (is_premature_line_parse_break)
					break; // Cannot continue: Division-By-Zero error
			}//end if (curr_tok.TOKEN_TYPE() == equation_token::enum_token_type::OPERATOR)

			out_queue.pop();
		}//end-while: Done evaluating output queue.

		if (is_premature_line_parse_break)
			continue; // Suppress additional messages for grading purposes in the event of a (probably intentional) user-caused logic error.

		// "If there is only one value in the stack"
		if (op_stack.size() == 1)
		{ // "That value is the result of the calculation."
			double d;
			switch (op_stack.top().TOKEN_TYPE())
			{
				case equation_token::enum_token_type::NUMBER: 
				{
					d = op_stack.top().get_number();
					break;
				}
				case equation_token::enum_token_type::VARIABLE: 
				{
					d = usrdef_var_storage[op_stack.top().STRING()];
					break;
				}
				default: 
				{
					throw std::logic_error("ERROR: Expression evaluated into non-number!!");
					break;
				}
			}

			if (is_var_assign_mode)
				usrdef_var_storage[var_to_assign] = d; // Assign variable, print nothing.
			else
				std::cout << d << std::endl;

			op_stack.pop();
		}
		else if (op_stack.size() > 1) // "Otherwise, there are more values in the stack"
		{
			// *(Error)* The user input has too many values.
			std::cerr << "Syntax error: Too many input values" << std::endl;
			continue;
		}
		else // (op_stack.size() < 1)
		{
			std::logic_error("ERROR: Post-expression evaluation op_stack in an invalid state!! (No elements)");
		}
	}//end-while: Done working with current line input.
}