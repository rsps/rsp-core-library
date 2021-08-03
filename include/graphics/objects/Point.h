#ifndef POINT_H
#define POINT_H

class Point {
   public:
    int x;
    int y;

    Point();
    Point(int aX, int aY);
    Point(const Point &aPoint);
};

#endif  // POINT_H
