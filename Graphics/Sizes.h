//
// Created by kalasoft on 10/6/21.
//

#ifndef KALASOFT_SIZES_H
#define KALASOFT_SIZES_H

class ImageSize{
    int width = 0 ,height = 0;
public:
    int height() const
    {
        return height;
    }
    int width() const
    {
        return width;
    }
};
#endif //KALASOFT_SIZES_H
