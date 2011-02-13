#ifndef _FILTERCHAIN_H
#define _FILTERCHAIN_H

#include <set>
#include <vector>
#include "improclistener.h"
#include "imagesource.h"
#include "procparams.h"
#include "filter.h"
#include "rtcommon.h"
#include "finalimage16.h"

namespace rtengine {

class FilterChain {

protected:

    bool multiThread;
	ImageSource* imgSource;
	Filter* first;
	Filter* last;
	ImProcListener* listener;
	Filter* firstToUpdate;
	ProcParams* procParams;
	StringList filterOrder;
	bool invalidated;
	bool oneShot;

	void setupChain (FilterChain* previous);

public:

	FilterChain (ImProcListener* listener, ImageSource* imgSource, ProcParams* params, bool multiThread, bool oneShot=false);
	FilterChain (ImProcListener* listener, FilterChain* previous);
	~FilterChain ();
	double getScale (int skip);
    void invalidate ();
    void setNextChain (FilterChain* other);

	void setupProcessing (const std::set<ProcEvent>& events, bool useShortCut = false);
	void process (const std::set<ProcEvent>& events, Buffer<float>* buffer, MultiImage* worker);
    Dim  getReqiredBufferSize ();
    Dim  getReqiredWorkerSize ();
    Dim  getFullImageSize ();

	ImProcListener* getListener () { return listener; }
    ImageSource* getImageSource () { return imgSource; }

	ImageView getLastImageView ();
    double    getLastScale ();

    FinalImage16*  getFinalImage ();
    DisplayImage getDisplayImage ();
    
    void 	 getMetadataToSave (Exiv2::ExifData& ed, Exiv2::IptcData& id, Exiv2::XmpData& xd);
};
}
#endif
