
//
// Created by kalasoft on 10/8/24.
//
#ifndef KALASOFT_GEOMETRY_H
#define KALASOFT_GEOMETRY_H
#define KS_PI 3.141592653589793
namespace ks
{

    enum EOrientation{

        POTRAIT_0 = 0,
        LANDSCAPE_90 = 90,
        POTRAIT_180 = 180,
        LANDSCAPE_270 = 270,
        UNKNOWN_ORIENTATION = -1
    };


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


    template<typename T> class Size{

    public:

        Size(){}

        Size(T pWidth, T pHeight)
        {
            this->width = pWidth;
            this->height = pHeight;
        }

        T width , height;
    };




    typedef Point2D<float> Point2DF;
    typedef Point3D<float> Point3DF;



    template<typename T> struct Bounds{

    public:

        T startX = 0, startY = 0,width = 0,height = 0;

        Bounds(){}

        Bounds(T pStartX,T pStartY,T pWidth,T pHeight)
        {
            startX = pStartX;
            startY = pStartY;
            width  = pWidth;
            height = pHeight;
        }

        Point2D<T> getCentre()
        {
            return Point2D<T>(startX + width/2,startY + height/2);
        }

    };

    typedef Bounds<float> BoundsF;

}

#endif //KALASOFT_GEOMETRY_H

