#ifndef BOX_H_
#define BOX_H_

#include <iostream>
#include "Point.h"
#include "Size.h"


template <typename T = double>
class Box
{
public:
	typedef T ValueType;
	
	Box()
	: xp(T(0)), yp(T(0)), w(T(0)), h(T(0)) { }
	
	Box(const Point<T> &topLeft, const Size<T> &size)
	: xp(topLeft.getX()), yp(topLeft.getY()), w(size.getWidth()), h(size.getHeight()) { }
	
	Box(T left, T top, T width, T height)
	: xp(left), yp(top), w(width), h(height) { }

	bool isEmpty() const;
	bool isValid() const;
	Box normalized() const;

	T getLeft() const { return xp; }
	T getTop() const { return yp; }
	T getRight() const { return xp + w; }
	T getBottom() const { return yp + h; }
	Size<T> getSize() const;

	T getX() const;
	T getY() const;
	void setLeft(T pos);
	void setTop(T pos);
	void setRight(T pos);
	void setBottom(T pos);
	void setX(T pos) { setLeft(pos); }
	void setY(T pos) { setTop(pos); }

	Point<T> getTopLeft() const { return Point<T>(xp, yp); }
	Point<T> getBottomRight() const { return Point<T>(xp+w, yp+h); }
	Point<T> getTopRight() const { return Point<T>(xp+w, yp); }
	Point<T> getBottomLeft() const { return Point<T>(xp, yp+h); }
	Point<T> getCenter() const;

	void setTopLeft(const Point<T> &p);
	void setBottomRight(const Point<T> &p);
	void setTopRight(const Point<T> &p);
	void setBottomLeft(const Point<T> &p);

	void moveLeft(T pos);
	void moveTop(T pos);
	void moveRight(T pos);
	void moveBottom(T pos);
	void moveTopLeft(const Point<T> &p);
	void moveBottomRight(const Point<T> &p);
	void moveTopRight(const Point<T> &p);
	void moveBottomLeft(const Point<T> &p);
	void moveCenter(const Point<T> &p);

	void translate(T dx, T dy);
	void translate(const Point<T> &p);

	Box translated(T dx, T dy) const;
	Box translated(const Point<T> &p) const;

	void moveTo(T x, T t);
	void moveTo(const Point<T> &p);

	void setBox(T x, T y, T w, T h);
	void getBox(T *x = 0, T *y = 0, T *w = 0, T *h = 0) const;

	void setCoords(T x1, T y1, T x2, T y2);
	void getCoords(T *x1 = 0, T *y1 = 0, T *x2 = 0, T *y2 = 0) const;

	template<typename T2>
    void scale(T2 xScale, T2 yScale);

	T getWidth() const;
	T getHeight() const;
	void setWidth(T w);
	void setHeight(T h);
	void setSize(const Size<T> &s);
	T getArea() const;

	Box<T> operator|(const Box &r) const;
	Box<T> operator&(const Box &r) const;
	Box<T>& operator|=(const Box &r);
	Box<T>& operator&=(const Box &r);
	template<typename T2>
	Box<T>& operator*=(T2 c);
	template<typename T2>
	Box<T>& operator/=(T2 c);

	template<typename T2>
	bool contains(const Point<T2> &p) const;
	template<typename T2>
	bool contains(T2 x, T2 y) const;
	bool contains(const Box<T> &r) const;
	Box<T> unite(const Box<T> &r) const;
	Box<T> intersect(const Box<T> &r) const;
	bool intersects(const Box<T> &r) const;

	template<typename T2>
	friend bool operator==(const Box<T2> &, const Box<T2> &);
	template<typename T2>
	friend bool operator!=(const Box<T2> &, const Box<T2> &);
	template<typename T2, typename T3>
	friend Box<T2> operator*(const Box<T2> &, T3 c);
	template<typename T2, typename T3>
	friend Box<T2> operator*(T3 c, const Box<T2> &);
	template<typename T2, typename T3>
	friend Box<T2> operator/(const Box<T2> &, T3 c);

	template<typename T2>
	operator Box<T2>() const;

private:
	T xp;
	T yp;
	T w;
	T h;
};

// stream operators
template<typename T>
std::ostream& operator<<(std::ostream& o, const Box<T>& b);

template<typename T>
std::istream& operator>>(std::istream& i, Box<T>& b);


//bool operator==(const Box &, const Box &);
//bool operator!=(const Box &, const Box &);

#endif /*BOX_H_*/
