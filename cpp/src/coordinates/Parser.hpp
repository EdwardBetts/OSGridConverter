/*
 * Parser.hpp
 *
 *  Created on: 5 Aug 2017
 *      Author: julianporter
 */

#ifndef COORDINATES_PARSER_HPP_
#define COORDINATES_PARSER_HPP_

#include <string>
#include <regex>



namespace coordinates {

class OSGrid;

class Parser {
public:
	enum class Kind {
		LatLong,
		Grid,
		Unknown,
		Unparsed
	};
private:
	std::string text;
	std::pair<double,double> parameters;

	std::smatch m;
	Kind kind;

	static const std::regex lonlatNSEW;
	static const std::regex lonlatPM;
	static const std::regex gridOS1;
	static const std::regex gridOS2;
	static const std::string gridAlphabet;

	bool isLatLong();
	bool isGrid();
	bool completeGrid(const unsigned e,const unsigned n);

public:

	Parser(const std::string & line) : text(line), parameters(), m(), kind(Kind::Unparsed) {};
	virtual ~Parser() = default;
	
	Kind parsedAs();
	std::pair<double,double> & operator()() { return parameters; }

	static std::string toString(const OSGrid &g) const;

	
};

} /* namespace coordinates */

#endif /* COORDINATES_PARSER_HPP_ */