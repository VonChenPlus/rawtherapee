#ifndef _FILTERCHAIN_H
#define _FILTERCHAIN_H

#include <set>
#include <vector>
#include "improclistener.h"
#include "imagesource.h"
#include "procparams.h"
#include "filter.h"

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
	std::vector<Glib::ustring> filterOrder;
	bool invalidated;

	void setupChain (FilterChain* previous);

public:

	FilterChain (ImProcListener* listener, ImageSource* imgSource, ProcParams* params, bool multiThread);
	FilterChain (ImProcListener* listener, FilterChain* previous);
	~FilterChain ();
	void getReqiredBufferSize (int& w, int& h);
	void getFullImageSize (int& w, int& h);

	void setupProcessing (const std::set<ProcEvent>& events, int fullW, int fullH, int& maxWorkerWidth, int& maxWorkerHeight, bool useShortCut = false);
	void process (const std::set<ProcEvent>& events, Buffer<int>* buffer, MultiImage* worker);

	ImProcListener* getListener () { return listener; }
	void invalidate ();

	ImageSource* getImageSource () { return imgSource; }

	void setNextChain (FilterChain* other);
};
}
#endif
