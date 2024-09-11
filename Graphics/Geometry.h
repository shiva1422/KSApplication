
//
// Created by kalasoft on 10/8/24.
//
#ifndef KALASOFT_GEOMETRY_H
#define KALASOFT_GEOMETRY_H

namespace ks
{
    template<typename T> class Point2D{

    public:

        Point2D(){}

        Point2D(T x, T y)
        {
            this->x = x;
            this->y = y;
        }

        T x , y;
    };

    typedef Point2D<float> Point2DF;


}

#endif //KALASOFT_GEOMETRY_H

