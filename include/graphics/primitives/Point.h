#ifndef POINT_H
#define POINT_H

class Point {
   public:
    int x;
    int y;

    inline Point()
        : x(0), y(0) {
    }
    inline Point(int aX, int aY)
        : x(aX), y(aY) {
    }
    inline Point(const Point &aPoint)
        : x(aPoint.x), y(aPoint.y) {
    }
};

#endif  // POINT_H
