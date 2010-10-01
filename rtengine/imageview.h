#ifndef _IMAGEVIEW_H_
#define _IMAGEVIEW_H_

#include "dim.h"

namespace rtengine {

class ImageView {

public:
	int x, y, w, h, skip;

    ImageView ();
    ImageView (int x, int y, int w, int h, int skip=1);

    bool operator== (const ImageView& other) const;
    bool operator!= (const ImageView& other) const;
	bool isPartOf (const ImageView& other) const;
	ImageView getScaled (double scale) const;
	Dim getSize () const;
};

}

#endif
