/*
 *  This file is part of RawTherapee.
 *
 *  Copyright (c) 2004-2010 Gabor Horvath <hgabor@rawtherapee.com>
 *
 *  RawTherapee is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  RawTherapee is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RawTherapee.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ffmanager.h"
#include "../rtgui/options.h"
#include <giomm.h>
#include "safegtk.h"
#include "rawimage.h"
#include <sstream>
#include <cstdio>
#include "imagedata.h"

namespace rtengine{

extern const Settings* settings;

// *********************** class ffInfo **************************************

inline ffInfo& ffInfo::operator =(const ffInfo &o){
	pathname = o.pathname;
	maker = o.maker;
	model = o.model;
	lens = o.lens;
	shutter = o.shutter;
	focallength = o.focallength;
	timestamp = o.timestamp;
	if( ri ){
		delete ri;
		ri = NULL;
	}
	return *this;
}

bool ffInfo::operator <(const ffInfo &e2) const
{
	if( this->maker.compare( e2.maker) >=0 )
		return false;
	if( this->model.compare( e2.model) >=0 )
		return false;
	if( this->lens.compare( e2.lens) >=0 )
		return false;
	if( this->focallength >= e2.focallength )
		return false;
	if( this->timestamp >= e2.timestamp )
		return false;
	return true;
}

std::string ffInfo::key(const std::string &mak, const std::string &mod, const std::string &len, double focal, double apert )
{
	std::ostringstream s;
	s << mak << " " << mod << " ";
	s.width(5);
	s << len << " ";
	s.precision( 2 );
	s.width(4);
	s << focal << "mm F" << apert;
	return s.str();
}

double ffInfo::distance(const std::string &mak, const std::string &mod, const std::string &len, double focallength, double aperture) const
{
	if( this->maker.compare( mak) != 0 )
		return INFINITY;
	if( this->model.compare( mod) != 0 )
		return INFINITY;
	if( this->lens.compare( len) != 0 )
			return INFINITY;
	double dAperture = 2*(log(this->aperture) - log(aperture))/log(2);//more important for vignette
    double dfocallength = (log(this->focallength/100.) - log(focallength/100.))/log(2);//more important for PRNU

    return sqrt( dfocallength*dfocallength + dAperture*dAperture);
}

RawImage* ffInfo::getRawImage()
{
	if(ri)
		return ri;
	updateRawImage();

	return ri;
}

/* updateRawImage() load into ri the actual pixel data from pathname if there is a single shot
 * otherwise load each file from the pathNames list and extract a template from the media;
 * the first file is used also for reading all information other than pixels
 */
void ffInfo::updateRawImage()
{
	typedef unsigned int acc_t;
	// averaging of flatfields if more than one is found matching the same key.
	// this may not be necessary, as flatfield is further blurred before being applied to the processed image.
	if( !pathNames.empty() ){
		std::list<Glib::ustring>::iterator iName = pathNames.begin();
		ri = new RawImage(*iName); // First file used also for extra pixels informations (width,height, shutter, filters etc.. )
		if( ri->loadRaw(true)){
			delete ri;
			ri=NULL;
		}else{
			int H = ri->get_height();
			int W = ri->get_width();
			ri->compress_image();
            int rSize = W*((ri->getSensorType()!=ST_NONE)?1:3);
			acc_t **acc = new acc_t*[H];
			for( int row=0; row<H;row++)
				acc[row] = new acc_t[rSize ];

			// copy first image into accumulators
			for (int row = 0; row < H; row++)
				for (int col = 0; col < rSize; col++)
					acc[row][col] = ri->data[row][col];
			int nFiles = 1; // First file data already loaded

			for( iName++; iName != pathNames.end(); iName++){
				RawImage* temp = new RawImage(*iName);
				if( !temp->loadRaw(true)){
					temp->compress_image();		//\ TODO would be better working on original, because is temporary
					nFiles++;
					if( ri->getSensorType()!=ST_NONE ){
						for( int row=0; row<H;row++){
							for( int col=0; col < W;col++)
								acc[row][col] += temp->data[row][col];
						}
					}else{
						for( int row=0; row<H;row++){
							for( int col=0; col < W;col++){
								acc[row][3*col+0] += temp->data[row][3*col+0];
								acc[row][3*col+1] += temp->data[row][3*col+1];
								acc[row][3*col+2] += temp->data[row][3*col+2];
							}
						}
					}
				}
				delete temp;
			}
			for (int row = 0; row < H; row++){
				for (int col = 0; col < rSize; col++)
					ri->data[row][col] = acc[row][col] / nFiles;
				delete [] acc[row];
			}
			delete [] acc;
		}
	}else{
		ri = new RawImage(pathname);
		if( ri->loadRaw(true)){
			delete ri;
			ri=NULL;
		}else {
			ri->compress_image();
		}
	}
}


// ************************* class FFManager *********************************

void FFManager::init( Glib::ustring pathname )
{
    std::vector<Glib::ustring> names;
    Glib::RefPtr<Gio::File> dir = Gio::File::create_for_path (pathname);
	if( dir && !dir->query_exists())
    	return;
	safe_build_file_list (dir, names, pathname);
	
	ffList.clear();
    for (size_t i=0; i<names.size(); i++) {
        try{
            addFileInfo(names[i]);
        }catch( std::exception& e ){}
    }
    // Where multiple shots exist for same group, move filename to list
    for( ffList_t::iterator iter = ffList.begin(); iter != ffList.end();iter++ ){
    	ffInfo &i = iter->second;
    	if( !i.pathNames.empty() && !i.pathname.empty() ){
    		i.pathNames.push_back( i.pathname );
    		i.pathname.clear();
    	}
    	if( settings->verbose ){
			if( !i.pathname.empty() )
			   printf( "%s:  %s\n",i.key().c_str(),i.pathname.c_str());
			else{
				printf( "%s: MEAN of \n    ",i.key().c_str());
				for( std::list<Glib::ustring>::iterator iter = i.pathNames.begin(); iter != i.pathNames.end();iter++  )
					printf( "%s, ", iter->c_str() );
				printf("\n");
			}
    	}
    }
    currentPath = pathname;
    return;
}

ffInfo *FFManager::addFileInfo(const Glib::ustring &filename, bool pool )
{
	Glib::RefPtr<Gio::File> file = Gio::File::create_for_path(filename);
    if (!file )
        return 0;
    if( !file->query_exists())
    	return 0;
    Glib::RefPtr<Gio::FileInfo> info = safe_query_file_info(file);
    if (info && info->get_file_type() != Gio::FILE_TYPE_DIRECTORY && (!info->is_hidden() || !options.fbShowHidden)) {
	size_t lastdot = info->get_name().find_last_of ('.');
        if (options.is_extention_enabled(lastdot!=Glib::ustring::npos ? info->get_name().substr (lastdot+1) : "")){
        	RawImage ri(filename);
        	int res = ri.loadRaw(false); // Read informations about shot
        	if( !res ){
        	   ffList_t::iterator iter;
        	   if(!pool){
        		   ffInfo n(filename,"","","",0,0,0);
        		   iter = ffList.insert(std::pair< std::string,ffInfo>( "", n ) );
        		   return &(iter->second);
        	   }
         	   RawMetaDataLocation rml;
         	   rml.exifBase = ri.get_exifBase();
         	   rml.ciffBase = ri.get_ciffBase();
         	   rml.ciffLength = ri.get_ciffLen();
         	   ImageData idata(filename, &rml);
         	   /* Files are added in the map, divided by same maker/model,lens and aperture*/
        	   std::string key( ffInfo::key(idata.getMake(),idata.getModel(),idata.getLens(),idata.getFocalLen(),idata.getFNumber()) );
        	   iter = ffList.find( key );
        	   if( iter == ffList.end() ){
				   ffInfo n(filename,idata.getMake(),idata.getModel(),idata.getLens(),idata.getFocalLen(),idata.getFNumber(),idata.getDateTimeAsTS());
				   iter = ffList.insert(std::pair< std::string,ffInfo>( key,n ) );
        	   }else{
        		   while( iter != ffList.end() && iter->second.key() == key && ABS(iter->second.timestamp - ri.get_timestamp()) >60*60*6 ) // 6 hour difference
        			   iter++;

        		   if( iter != ffList.end() )
        		      iter->second.pathNames.push_back( filename );
        		   else{
    				   ffInfo n(filename,idata.getMake(),idata.getModel(),idata.getLens(),idata.getFocalLen(),idata.getFNumber(),idata.getDateTimeAsTS());
    				   iter = ffList.insert(std::pair< std::string,ffInfo>( key,n ) );
        		   }
        	   }
               return &(iter->second);
        	}
		}
    }
    return 0;
}

void FFManager::getStat( int &totFiles, int &totTemplates)
{
	totFiles=0;
	totTemplates=0;
    for( ffList_t::iterator iter = ffList.begin(); iter != ffList.end();iter++ ){
    	ffInfo &i = iter->second;
    	if( i.pathname.empty() ){
    		totTemplates++;
    		totFiles += i.pathNames.size();
    	}else
    		totFiles++;
    }
}

/*  The search for the best match is twofold:
 *  if perfect matches for make and model are found, then the list is scanned for lesser distance in time
 *  otherwise if no match is found, the whole list is searched for lesser distance in lens and aperture
 */
ffInfo* FFManager::find( const std::string &mak, const std::string &mod, const std::string &len, double focal, double apert, time_t t )
{
	if( ffList.empty() )
		return 0;
	std::string key( ffInfo::key(mak,mod,len,focal,apert) );
	ffList_t::iterator iter = ffList.find( key );

	if(  iter != ffList.end() ){
		ffList_t::iterator bestMatch = iter;
		time_t bestDeltaTime = ABS(iter->second.timestamp - t);
		for(iter++; iter != ffList.end() && !key.compare( iter->second.key() ); iter++ ){
			time_t d = ABS(iter->second.timestamp - t );
			if( d< bestDeltaTime ){
				bestMatch = iter;
				bestDeltaTime = d;
			}
		}
        return &(bestMatch->second);
	}else{
		iter = ffList.begin();
		ffList_t::iterator bestMatch = iter;
		double bestD = iter->second.distance(  mak, mod, len, focal, apert );
		for( iter++; iter != ffList.end();iter++ ){
           double d = iter->second.distance(  mak, mod, len, focal, apert );
           if( d < bestD ){
        	   bestD = d;
        	   bestMatch = iter;
           }
		}
		return bestD != INFINITY ? &(bestMatch->second) : 0 ;
	}
}

RawImage* FFManager::searchFlatField( const std::string &mak, const std::string &mod, const std::string &len, double focal, double apert, time_t t )
{
   ffInfo *ff = find( mak, mod, len, focal, apert, t );
   if( ff )
      return ff->getRawImage();
   else
	  return 0;
}

RawImage* FFManager::searchFlatField( const Glib::ustring filename )
{
	for ( ffList_t::iterator iter = ffList.begin(); iter != ffList.end();iter++ ){
		if( iter->second.pathname.compare( filename )==0  )
			return iter->second.getRawImage();
	}
	ffInfo *ff = addFileInfo( filename , false);
	if(ff)
		return ff->getRawImage();
	return 0;
}


// Global variable
FFManager ffm;


}

