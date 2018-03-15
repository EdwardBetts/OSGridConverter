/*
 * LatitudeLongitude.cpp
 *
 *  Created on: 31 Jul 2017
 *      Author: julianporter
 */

#include "LatitudeLongitude.hpp"
#include "Cartesian.hpp"
#include "Parser.hpp"
#include "OSGrid.hpp"
#include "mapping/OSGB36.hpp"
#include <stdexcept>
#include <iomanip>



namespace coordinates {

using Axis = Cartesian::Axis;

std::pair<double,double> gridToLatLong(const OSGrid &grid,Tag tag) {

	double dN=double(grid.N()-mapping::OSGB36::N0);
	double M=0.0;
	double phi=mapping::OSGB36::phi0;

	do {
		phi+=(dN-M)/mapping::OSGB36::aF0();
		M=mapping::OSGB36::meridional(phi);
	} while(dN-M >= 1.0e-5);

	mapping::OSGB36 v(phi,grid.E());
	auto pair=v.toLatLong();

	auto ph=degrees(std::get<0>(pair));
	auto lambda=degrees(std::get<1>(pair));

	if(tag==Tag::OSGB36) {
		return std::make_pair(ph,lambda);
	}
	else {
		LatitudeLongitude point(ph,lambda,Tag::OSGB36);
		point=point.transform(tag);
		return std::make_pair(point.Latitude(),point.Longitude());
	}
}

LatitudeLongitude::LatitudeLongitude(const Cartesian &cartesian,Tag tag) : LatitudeLongitude(cartesian.vec(),tag) {};

LatitudeLongitude::LatitudeLongitude(const std::string &str,Tag tag) : datum(mapping::Datum::get(tag)), dTag(tag) {
	Parser p(str);
	auto kind=p.parse();
	switch(kind) {
	case Parser::Kind::LatLong: {
		auto coordinates=p();
		latitude=coordinates.lat;
		longitude=coordinates.lon;
		break;
	}
	case Parser::Kind::Grid: {
		auto coordinates=p();
		OSGrid g(coordinates.N,coordinates.E);
		auto pair=gridToLatLong(g,tag);
		latitude=pair.first;
		longitude=pair.second;
		break;
	}
	default:
		throw std::runtime_error("Invalid string representation for latitude / longitude type");
	}
}

LatitudeLongitude::LatitudeLongitude(const mapping::Vector &v,Tag tag) : datum(mapping::Datum::get(tag)), dTag(tag) {
	mapping::Ellipsoid e=ellipsoid();

	auto p=hypot(v(0),v(1));
	auto t=(1.0+(e.eccentricity2()*e.minorAxis()/boost::numeric::ublas::inner_prod(v,v)))*e.minorAxis()*v(2)/(e.majorAxis()*p);
	auto s=t/sqrt(1.0+t*t);
	auto c=s/t;

	double phi = std::isnan(c) ? 0 : atan2(v(2)+e.eccentricity2()*e.minorAxis()*pow(s,3),p-e.eccentricity1()*e.majorAxis()*pow(c,3));
	double lambda = atan2(v(1),v(0));

	latitude=degrees(phi);
	longitude=degrees(lambda);
}

LatitudeLongitude::LatitudeLongitude(const OSGrid &grid,Tag tag) : datum(mapping::Datum::get(tag)), dTag(tag) {
	auto pair=gridToLatLong(grid,tag);
	latitude=pair.first;
	longitude=pair.second;
}

LatitudeLongitude  LatitudeLongitude::transform( Tag newTag) const {
	auto newDatum=mapping::Datum::get(newTag);
	if(datum==newDatum) return *this;
	else {
		Cartesian c(*this);
		auto c2=newDatum(datum.invert(c.vec()));
		return LatitudeLongitude(c2,newTag);
	}
}

} /* namespace coordinates */

std::ostream & operator<<(std::ostream &o,const coordinates::LatitudeLongitude &ll) {
	char ns=(ll.Latitude()>=0.0) ? 'N' : 'S';
	char ew=(ll.Longitude()>=0.0) ? 'E' : 'W';

	o << std::fixed << std::setprecision(5) << fabs(ll.Latitude()) << ns << "," << fabs(ll.Longitude()) << ew;
	return o;
}





