#include <istream>
#include <deque>
#include <exception>

class Production;
class Expr;
class Term;
class Unary;
class Number;
class Factor;
class ParseError;

//the base class for all of our productions
class Production {
public:
	virtual ~Production(); 
	virtual double getValue() = 0; 
};

//error processing
class ParseError : public std::exception {
	const char* what() const throw();
};

//parse a number 
class Number : public Production {
private:
	double value;
public:
	Number(std::istream& in);
	double getValue();
};

//parse a factor 
class Factor : public Production {
private:
	Production* expr; 
public:
	Factor(std::istream& in);
	~Factor();
	double getValue();
};

//parse unary operators 
class Unary : public Production {
private:
	int sign;
	Factor* value;
public:
	Unary(std::istream& in);
	~Unary();
	double getValue();
};

//parse binary operators(* and /)
class Term : public Production {
private:
	std::deque<Unary*> values;
	std::deque<char> ops;
public:
	Term(std::istream& in);
	~Term();
	double getValue();
};

//parse binary operators(+ and -)
class Expr : public Production {
private:
	std::deque<Term*> values;
	std::deque<char> ops;
public:
	Expr(std::istream& in);
	~Expr();
	double getValue();
};

