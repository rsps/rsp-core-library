#ifndef POINT_H
#define POINT_H

class Point
{
  public:
    int mX;
    int mY;

    inline Point()
        : mX(0), mY(0)
    {
    }
    inline Point(int aX, int aY)
        : mX(aX), mY(aY)
    {
    }
    inline Point(const Point &aPoint)
        : mX(aPoint.mX), mY(aPoint.mY)
    {
    }
};

#endif // POINT_H
