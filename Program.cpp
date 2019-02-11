#include <iostream>
#include <deque>
#include <sstream>
#include "Parser.h"

//static functions

static void ignoreSpace(std::istream& in);
static char getChar(std::istream& in);


static void ignoreSpace(std::istream& in) {
	while (isspace(in.peek())) {
		in.get();
	}
}


static char getChar(std::istream& in) {
	ignoreSpace(in); // remove all preceding space
	char ret = in.get();
	ignoreSpace(in); // remove all following space
	return ret;
}

Production::~Production() {}


//ParseError
const char* ParseError::what() const throw() {
	return "a parsing error occured";
}

//Number
Number::Number(std::istream& in) {
	ignoreSpace(in); // remove all preceding space
	in >> value;
	if (!in) {
		throw ParseError();
		return;
	}
	ignoreSpace(in); // remove all following space
}

double Number::getValue() {
	return value;
}

//Factor
Factor::Factor(std::istream& in) {
	ignoreSpace(in); // remove all preceding space
	if (in.peek() == '(') { // check to see if a paren was used
		in.get();
		expr = new Expr(in);
		ignoreSpace(in); // remove all following space
		if (in.peek() != ')') { 
			throw ParseError();
		}
		else {
			in.get();
		}
	}
	else { 
		expr = new Number(in);
	}
}

Factor::~Factor() {
	delete expr;
}

double Factor::getValue() {
	return expr->getValue();
}

//Unary
Unary::Unary(std::istream& in) {
	sign = 1; 
	ignoreSpace(in); 
	while (in.peek() == '-' || in.peek() == '+') { // while we have an operator to parse
		if (getChar(in) == '-') { 
			sign = -sign;
		}
	}
	value = new Factor(in); // parse the factor following the unary operators
}

Unary::~Unary() {
	delete value;
}

double Unary::getValue() {
	return sign * value->getValue();
}

//Term
Term::Term(std::istream& in) {
	values.push_back(new Unary(in)); // construct the first value
	ignoreSpace(in); 
	while (in.peek() == '*' || in.peek() == '/') {
		ops.push_back(getChar(in)); 
		values.push_back(new Unary(in)); 
	}
}

Term::~Term() {
	for (unsigned int i = 0; i < values.size(); ++i) {
		delete values[i];
	}
}

double Term::getValue() {
	double ret = values[0]->getValue(); // get the first value
	for (unsigned int i = 1; i < values.size(); ++i) { 
		if (ops[i - 1] == '*') { 
			ret *= values[i]->getValue();
		}
		else {
			ret /= values[i]->getValue();
		}
	}
	return ret;
}

//Expr
Expr::Expr(std::istream& in) {
	ignoreSpace(in);
	values.push_back(new Term(in));
	while (in.peek() == '+' || in.peek() == '-') {
		ops.push_back(getChar(in));
		values.push_back(new Term(in));
	}
}

Expr::~Expr() {
	for (unsigned int i = 0; i < values.size(); ++i) {
		delete values[i];
	}
}

double Expr::getValue() {
	double ret = values[0]->getValue();
	for (unsigned int i = 1; i < values.size(); ++i) {
		if (ops[i - 1] == '+') {
			ret += values[i]->getValue();
		}
		else {
			ret -= values[i]->getValue();
		}
	}
	return ret;
}

int main() {
	std::string line;
	while (!std::cin.eof()) {
		std::getline(std::cin, line);
		
		if (line.length() > 0) {
			try {
				std::stringstream ss;
				ss << line;
				Expr expr(ss);
				std::cout << "Result: " << expr.getValue() << std::endl;
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
			}
		}
	}
	return 0;
}