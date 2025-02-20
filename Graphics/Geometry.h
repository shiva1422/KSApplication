
//
// Created by kalasoft on 10/8/24.
//
#ifndef KALASOFT_GEOMETRY_H
#define KALASOFT_GEOMETRY_H
#define KS_PI 3.141592653589793
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


    template<typename T> class Point3D{

    public:

        Point3D(){}

        Point3D(T x, T y, T z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        T x , y , z;
    };


    typedef Point2D<float> Point2DF;
    typedef Point3D<float> Point3DF;
    typedef Point2D<int> Size2DI;


}

#endif //KALASOFT_GEOMETRY_H

