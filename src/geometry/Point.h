#ifndef POINT_H_
#define POINT_H_

#include <iostream>

template<typename T>
class Point
{
public:
	typedef T ValueType;
	
    Point()
    : xp(0), yp(0) { }
    
    Point(T xpos, T ypos)
    : xp(xpos), yp(ypos) { }

    bool isNull() const;

    T getX() const;
    T getY() const;
    void setX(T x);
    void setY(T y);

    Point<T> &operator+=(const Point<T> &p);
    Point<T> &operator-=(const Point<T> &p);
    template<typename T2>
    Point<T> &operator*=(T2 c);
    template<typename T2>
    Point<T> &operator/=(T2 c);

	template<typename T2>
	operator Point<T2>() const;

private:
    T xp;
    T yp;
};


// some usefule operators
template<typename T>
bool operator==(const Point<T> &, const Point<T> &);

template<typename T>
bool operator!=(const Point<T> &, const Point<T> &);

template<typename T>
const Point<T> operator+(const Point<T> &, const Point<T> &);

template<typename T>
const Point<T> operator-(const Point<T> &, const Point<T> &);

template<typename T>
const Point<T> operator-(const Point<T> &);

template<typename T2, typename T3>
const Point<T2> operator*(T3, const Point<T2> &);

template<typename T2, typename T3>
const Point<T2> operator*(const Point<T2> &, T3);

template<typename T2, typename T3>
const Point<T2> operator/(const Point<T2> &, T3);


// stream operators
template<typename T>
std::ostream& operator<<(std::ostream& o, const Point<T>& b);

template<typename T>
std::istream& operator>>(std::istream& i, Point<T>& b);

#endif /*POINT_H_*/
