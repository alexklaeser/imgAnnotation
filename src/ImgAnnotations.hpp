#ifndef ImgAnnotation_HPP
#define ImgAnnotation_HPP

#include "ImgAnnotations.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>


template<typename T>
T IA::PropertyMap::getAs(const std::string& property) const
{
	std::string lowProperty = boost::algorithm::to_lower_copy(property);
	StrStrMap::const_iterator i = _properties.find(lowProperty);
	if (i == _properties.end())
		return T();
	else
		return boost::lexical_cast<T>(i->second);
}

template<typename T>
void IA::PropertyMap::set(const std::string& property, const T& value)
{
	assert(std::string::npos == property.find(":")); 
	std::string lowProperty = boost::algorithm::to_lower_copy(property);
	_properties[lowProperty] = boost::lexical_cast<std::string>(value); 
}

#endif
