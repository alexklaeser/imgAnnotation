#ifndef SIZE_H_
#define SIZE_H_

#include <iostream>


template<typename T = double>
class Size
{
public:
	typedef T ValueType;
	
    Size()
    : wd(T(-1)), ht(T(-1)) { }
    
    Size(T w, T h)
    : wd(w), ht(h) { }

    bool isEmpty() const;
    bool isValid() const;

    T getWidth() const;
    T getHeight() const;
    void setWidth(T w);
    void setHeight(T h);
    void transpose();
	T getArea() const;

//    void scale(T w, T h);
//    void scale(const Size &s);

//    Size expandedTo(const Size &) const;
//    Size boundedTo(const Size &) const;

//    T &rwidth();
//    T &rheight();

    Size<T> &operator+=(const Size<T> &);
    Size<T> &operator-=(const Size<T> &);
    template<typename T2>
    Size<T> &operator*=(T2 c);
    template<typename T2>
    Size<T> &operator/=(T2 c);

	template<typename T2>
	operator Size<T2>() const;

private:
    T wd;
    T ht;
};


// some useful operators
template<typename T2>
bool operator==(const Size<T2> &, const Size<T2> &);

template<typename T2>
bool operator!=(const Size<T2> &, const Size<T2> &);

template<typename T2>
const Size<T2> operator+(const Size<T2> &, const Size<T2> &);

template<typename T2>
const Size<T2> operator-(const Size<T2> &, const Size<T2> &);

template<typename T2, typename T3>
const Size<T2> operator*(const Size<T2> &, T3);

template<typename T2, typename T3>
const Size<T2> operator*(T3, const Size<T2> &);

template<typename T2, typename T3>
const Size<T2> operator/(const Size<T2> &, T3);


// stream operators
template<typename T>
std::ostream& operator<<(std::ostream& o, const Size<T>& b);

template<typename T>
std::istream& operator>>(std::istream& i, Size<T>& b);


#endif /*SIZE_H_*/
