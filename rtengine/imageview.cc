/*
 * imageview.cc
 *
 *  Created on: Sep 22, 2010
 *      Author: gabor
 */

#include "imageview.h"

namespace rtengine {

ImageView::ImageView () : x(0), y(0), w(0), h(0), skip(1) {}
ImageView::ImageView (int x, int y, int w, int h, int skip) : x(x), y(y), w(w), h(h), skip(skip) {}

bool ImageView::operator== (const ImageView& other) const {

    return x==other.x && y==other.y && w==other.w && h==other.h && skip==other.skip;
}

bool ImageView::operator!= (const ImageView& other) const {

    return !(*this == other);
}

int ImageView::getPixelWidth () const {

    return w >> skip;
}

int ImageView::getPixelHeight () const {

    return h >> skip;
}

bool ImageView::isPartOf (const ImageView& other) const {

    return x >= other.x && x+w-1 <= other.x+other.w-1
        && y >= other.y && y+h-1 <= other.y+other.h-1
        && skip >= other.skip && skip%other.skip == 0;
}

}
