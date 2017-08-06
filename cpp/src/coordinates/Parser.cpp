/*
 * Parser.cpp
 *
 *  Created on: 5 Aug 2017
 *      Author: julianporter
 */

#include "Parser.hpp"
#include "OSGrid.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>


namespace coordinates {

const std::regex Parser::lonlatNSEW ("^\\s*([+-]?(?:\\d*\\.)?\\d+)([ns])\\s*,\\s*([+-]?(?:\\d*\\.)?\\d+)([ew])\\s*$",std::regex::icase);
const std::regex Parser::lonlatPM   ("^\\s*([+-]?(?:\\d*\\.)?\\d+)\\s*,\\s*([+-]?(?:\\d*\\.)?\\d+)\\s*$",std::regex::icase);
const std::regex Parser::gridOS1     ("^\\s*([A-Z]{2})\\s*([0-9]+)\\s*$",std::regex::icase);
const std::regex Parser::gridOS2     ("^\\s*([A-Z]{2})\\s*([0-9]+)\\s*([0-9]+)\\s*$",std::regex::icase);
const std::string Parser::gridAlphabet="ABCDEFGHJKLMNOPQRSTUVWXYZ";

std::string toUpper(const std::string &str) {
	std::stringstream s;
	s << std::uppercase << str;
	return s.str();
}
unsigned padTo(const unsigned u,const unsigned n) {
	unsigned l=log10(u);
	return unsigned(l*pow(10.0,n-l-1));
}

bool Parser::isLatLong() {
	try {
		if (std::regex_match(text,m,Parser::lonlatNSEW)) {
			double lat = std::stod(m[1]);
			auto ns =m[2];
			if(ns=="s" || ns=="S") lat*=-1.0;

			double lon = std::stod(m[3]);
			auto ew =m[4];
			if(ew=="w" || ew=="W") lon*=-1.0;

			parameters=std::make_pair(lat,lon);
			return true;
		}
		else if (std::regex_match(text,m,Parser::lonlatPM)) {
			double lat = std::stod(m[1]);
			double lon = std::stod(m[2]);
			parameters=std::make_pair(lat,lon);
			return true;
		}
	}
	catch(...) {}
	return false;
}

bool Parser::completeGrid(const unsigned e,const unsigned n) {
	std::string letters=m[1];
	auto l1=Parser::gridAlphabet.find(letters.at(0));
	auto l2=Parser::gridAlphabet.find(letters.at(1));
	if(l1==std::string::npos || l2==std::string::npos) return false;
	int e100km=((l1-2)%5)*5 + (l2%5);
	int n100km=19-5*int(l1/5) - int(l2/5);
	if(e100km<0 || e100km>6 || n100km<0 || n100km>12) return false;

	auto ee=padTo(e,5);
	auto nn=padTo(n,5);
	parameters=std::make_pair(ee+e100km*1.0e5,nn+n100km*1.0e6);
	return true;
}

bool Parser::isGrid() {
	try {
		auto grid=toUpper(text);
		if (std::regex_match(grid,m,Parser::gridOS1)) {
			std::string numbers=m[2];
			unsigned l=numbers.size()/2;
			if(numbers.size()!=2*l) return false;
			unsigned e=std::stoul(numbers.substr(0,l));
			unsigned n=std::stoul(numbers.substr(l,l));

			return completeGrid(e,n);
		}
		else if (std::regex_match(grid,m,Parser::gridOS2)) {
			unsigned e=std::stoul(m[2]);
			unsigned n=std::stoul(m[3]);
			return completeGrid(e,n);
		}
	}
	catch(...) {}
	return false;
}

Parser::Kind Parser::parsedAs() {
	if(kind==Kind::Unparsed) {
		kind = isGrid() ? Kind::Grid :
				isLatLong() ? Kind::LatLong : Kind::Unknown;
	}
	return kind;
}

std::string Parser::toString(const OSGrid &g) const {
	try {
		auto c=g.coordinates();
		auto E=std::get<0>(c);
		auto N=std::get<1>(c);
		long e100km=E/10000;
		long n100km=N/10000;
		if(e100km<0 || e100km>6 || n100km<0 || n100km>12) return "OOR";
		auto nf=19-n100km;
		auto ef=10+e100km;

		long l1=nf-(nf%5)+(ef/5);
		long l2=(5*nf)%25 + (ef%5);
		std::stringstream s;
		s << Parser::gridAlphabet[l1] << Parser::gridAlphabet[l2];
		auto e = E%100000;
		auto n = N%100000;
		s << " " << std::setw(5) << std::setfill('0')  << std::right << e << " " << n;
		return s.str();
	}
	catch(...) {
		return "OOR";
	}
}


} /* namespace coordinates */
