/*
 * Datum.h
 *
 *  Created on: 31 Jul 2017
 *      Author: julianporter
 */

#ifndef DATUM_HPP_
#define DATUM_HPP_

#include "Vector.hpp"
#include "Ellipsoid.hpp"
#include "Helmert.hpp"
#include <tuple>
#include <initializer_list>


namespace mapping {



class Datum {
	friend bool operator==(const Datum &l,const Datum &r);
	friend bool operator!=(const Datum &l,const Datum &r);
public:
	enum class Name {
			WGS84,
			NTF,
			NAD83,
			OSGB36,
			ED50,
			Irl1975,
			TokyoJapan
		};
private:
	Ellipsoid e;
	Helmert helmert;

public:
	Datum(const Ellipsoid::Name & model,const util::Vector &t,const util::Vector &r,const double s);
	Datum(const Ellipsoid::Name & model,const Helmert &h);

	Datum(const Datum &o) : e(o.e), helmert(o.helmert) {};
	virtual ~Datum() = default;
	
	const Ellipsoid & ellipsoid() const { return e; }
	util::Vector operator()(const util::Vector &v) const;
	util::Vector operator()(util::Vector &&v) const;
	util::Vector apply(const util::Vector &v) const;
	util::Vector invert(const util::Vector &v) const;


	static Datum get(const Name &key);



};

bool operator==(const Datum &l,const Datum &r);
bool operator!=(const Datum &l,const Datum &r);

} /* namespace mapping */




#endif /* DATUM_HPP_ */



