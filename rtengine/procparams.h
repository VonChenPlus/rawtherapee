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
#ifndef _PROCPARAMS_H_
#define _PROCPARAMS_H_

#include <glibmm.h>
#include <vector>
#include "rtcommon.h"
#include <map>
#include <vector>

namespace rtengine {


/**
  * Parameters of the cropping
  */
class CropParams {

    public:
        bool    enabled;
        int     x;
        int     y;
        int     w;
        int     h;
        bool    fixratio;
        Glib::ustring   ratio;
        Glib::ustring   orientation;
        Glib::ustring   guide;
};

/**
  * Common transformation parameters
  */

/**
  * Parameters of the color spaces used during the processing
  */
class ColorManagementParams {

    public:
		Glib::ustring input;
        bool        gammaOnInput;
        Glib::ustring working;
        Glib::ustring output;
};

/**
  * A class representing a key/value for the exif metadata information
  */
class ExifPair {

    public:
		String field;
		String value;
};

/**
  * The IPTC key/value pairs
  */
class IPTCPair {

    public:
		String field;
        StringList values;
};

/**
  * This class holds all the processing parameters applied on the images
  */

class ProcParams {

		std::map<String,double> floatParams;
		std::map<String,int> 	 intParams;
		std::map<String,bool> 	 boolParams;
		std::map<String,String> stringParams;
		std::map<String,FloatList> floatListParams;
		std::map<String,IntList> intListParams;
		std::map<String,StringList> stringListParams;

    public:

		void   setFloat   (const String& key, float value);
		float  getFloat   (const String& key);

		void   setInteger (const String& key, int value);
		int    getInteger (const String& key);

		void   setBoolean (const String& key, bool value);
		bool   getBoolean (const String& key);

		void   setString  (const String& key, const String& value);
		String getString  (const String& key);

		void   		setFloatList (const String& key, const FloatList& value);
		FloatList&  getFloatList (const String& key);

		void   		setIntegerList (const String& key, const IntList& value);
		IntList& 	getIntegerList (const String& key);

		void   		setStringList (const String& key, const StringList& value);
		StringList& getStringList (const String& key);

		// --------------8<------------------ to be removed when all filters are rewritten ------

        CropParams              crop;           ///< Crop parameters

        ColorManagementParams   icm;            ///< profiles/color spaces used during the image processing
        std::vector<ExifPair> exif;             ///< List of modifications appplied on the exif tags of the input image
        std::vector<IPTCPair> iptc;             ///< The IPTC tags and values to be saved to the output image
        int version;                            ///< Version of the file from which the parameters have been read
        
      /**
        * The constructor only sets the hand-wired defaults.
        */
        ProcParams          ();
      /**
        * Sets the hand-wired defaults parameters.
        */
        void    setDefaults ();
      /**
        * Saves the parameters to a file.
        * @param fname the name of the file
        * @return Error code (=0 if no error)
        */
        int     save        (Glib::ustring fname) const;
      /**
        * Loads the parameters from a file.
        * @param fname the name of the file
        * @return Error code (=0 if no error)
        */
        int     load        (Glib::ustring fname);

      /** Creates a new instance of ProcParams.
        * @return a pointer to the new ProcParams instance. */
        static ProcParams* create  ();

      /** Destroys an instance of ProcParams.
        * @param pp a pointer to the ProcParams instance to destroy. */
        static void        destroy (ProcParams* pp);

        bool operator== (const ProcParams& other);
        bool operator!= (const ProcParams& other);
};

extern ProcParams defaultProcParams;
}
#endif
