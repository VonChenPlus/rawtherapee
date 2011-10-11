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
// generated 2004/6/3 19:15:32 CEST by gabor@darkstar.(none)
// using glademm V2.5.0
//
// newer (non customized) versions of this file go to raw.cc_new

// This file is for your program, I won't touch it again!

#include <config.h>
#include <gtkmm.h>
#include <giomm.h>
#include <iostream>
#include <rtwindow.h>
#include <string.h>
#include <stdlib.h>
#include <options.h>
#include <soundman.h>
#include <rtimage.h>
#include "version.h"

#ifndef WIN32
#include <glibmm/fileutils.h>
#include <glib.h>
#include <glib/gstdio.h>
#endif

#include <safegtk.h>

extern Options options;

// stores path to data files
Glib::ustring argv0;
Glib::ustring creditsPath;
Glib::ustring licensePath;
Glib::ustring argv1;
bool simpleEditor;

/* Process line command options
 * Returns
 *  0 if process in batch has executed
 *  1 to start GUI (with a dir or file option)
 *  2 to start GUI because no files found
 *  -1 if there is an error in parameters
 *  -2 if an error occurred during processing */
int processLineParams( int argc, char **argv );

int main(int argc, char **argv)
{
#ifdef BUILD_BUNDLE
    char exname[512] = {0};
    Glib::ustring exePath;
    // get the path where the rawtherapee executable is stored
    #ifdef WIN32
        WCHAR exnameU[512] = {0};
        GetModuleFileNameW (NULL, exnameU, 512);
        WideCharToMultiByte(CP_UTF8,0,exnameU,-1,exname,512,0,0 );
    #else
        if (readlink("/proc/self/exe", exname, 512) < 0) {
            strncpy(exname, argv[0], 512);
        }
    #endif
    exePath = Glib::path_get_dirname(exname);

    // set paths
    if (Glib::path_is_absolute(DATA_SEARCH_PATH)) {
        argv0 = DATA_SEARCH_PATH;
    } else {
        argv0 = Glib::build_filename(exePath, DATA_SEARCH_PATH);
    }
    if (Glib::path_is_absolute(CREDITS_SEARCH_PATH)) {
        creditsPath = CREDITS_SEARCH_PATH;
    } else {
        creditsPath = Glib::build_filename(exePath, CREDITS_SEARCH_PATH);
    }
    if (Glib::path_is_absolute(LICENCE_SEARCH_PATH)) {
        licensePath = LICENCE_SEARCH_PATH;
    } else {
        licensePath = Glib::build_filename(exePath, LICENCE_SEARCH_PATH);
    }
#else
    argv0 = DATA_SEARCH_PATH;
    creditsPath = CREDITS_SEARCH_PATH;
    licensePath = LICENCE_SEARCH_PATH;
#endif
  
   Glib::thread_init();
   gdk_threads_init();
   Gio::init ();

   Options::load ();

   SoundManager::init();

   if (argc>1){
	   int ret = processLineParams( argc, argv);
	   if( ret <= 0 )
		   return ret;
   }

#ifndef WIN32
   // Move the old path to the new one if the new does not exist
   if (safe_file_test(Glib::build_filename(options.rtdir,"cache"), Glib::FILE_TEST_IS_DIR) && !safe_file_test(options.cacheBaseDir, Glib::FILE_TEST_IS_DIR))
       safe_g_rename(Glib::build_filename(options.rtdir,"cache"), options.cacheBaseDir);
#endif

   simpleEditor=false;
   if( !argv1.empty() )
      if( safe_file_test(argv1, Glib::FILE_TEST_EXISTS) && !safe_file_test(argv1, Glib::FILE_TEST_IS_DIR))
         simpleEditor = true;

   if (!options.useSystemTheme)
   {
       std::vector<Glib::ustring> rcfiles;
       rcfiles.push_back (argv0+"/themes/"+options.theme+".gtkrc");
   	   if (options.slimUI)
           rcfiles.push_back (argv0+"/themes/slim");
       // Set the font face and size
       Gtk::RC::parse_string (Glib::ustring::compose(
          "style \"clearlooks-default\" { font_name = \"%1\" }", options.font));
       Gtk::RC::set_default_files (rcfiles);
   }
   Gtk::Main m(&argc, &argv);

   RTImage::setPaths(options);

#ifndef WIN32
   // For an unknown reason, gtkmm 2.22 don't know the gtk-button-images property, while it exists in the documentation...
   // Anyway, the problem was Linux only
   static Glib::RefPtr<Gtk::Settings> settings = Gtk::Settings::get_default();
   if (settings)
      settings->property_gtk_button_images().set_value(true);
   else
      printf("Error: no default settings to update!\n");
#endif

   RTWindow *rtWindow = new class RTWindow();
   gdk_threads_enter ();
   m.run(*rtWindow);
   gdk_threads_leave ();
   delete rtWindow;
   rtengine::cleanup();
   return 0;
}

int processLineParams( int argc, char **argv )
{
	std::vector<Glib::ustring> inputFiles;
	Glib::ustring outputPath = "";
	Glib::ustring processingParams = "";
	bool isDirectory=false;
	bool outputDirectory=false;
	bool overwriteFiles=false;
	bool sideProcParams=false;
	bool copyParamsFile=false;
	bool useDefaultIfAbsent=true;
	int compression=100;
	int bits=-1;
	std::string outputType = "";
	unsigned errors=0;
	for( int iArg=1; iArg<argc; iArg++){
		if( argv[iArg][0]=='-' ){
			switch( argv[iArg][1]){
			case 'O':
				copyParamsFile = true;
			case 'o': // outputfile or dir
				if( iArg+1 <argc ){
					iArg++;
					outputPath = safe_filename_to_utf8 (argv[iArg]);
					if( safe_file_test (outputPath, Glib::FILE_TEST_IS_DIR))
						outputDirectory=true;
				}
				break;
			case 'p': // processing parameters for all inputs
				if( iArg+1 <argc ){
					iArg++;
					processingParams = safe_filename_to_utf8 ( argv[iArg] );
				}
				break;
			case 'S':
				useDefaultIfAbsent=false;
			case 's': // Processing params next to file (.pp3 appended)
				sideProcParams = true;
				break;
			case 'Y':
				overwriteFiles =true;
				break;
			case 'j':
				outputType = "jpg";
				sscanf(&argv[iArg][2],"%d",&compression);
				break;
			case 't':
				outputType = "tif";
				compression = ((argv[iArg][2]!='1')?0:1);
				break;
			case 'n':
				outputType = "png";
				compression = -1;
				break;
			case 'c': // MUST be last option
				while( iArg+1 <argc ){
					iArg++;
					if( !safe_file_test( safe_filename_to_utf8(argv[iArg]), Glib::FILE_TEST_EXISTS )){
						std::cerr << argv[iArg] << " doesn't exist."<< std::endl;
						continue;
					}
					if( safe_file_test( safe_filename_to_utf8(argv[iArg]), Glib::FILE_TEST_IS_DIR )){
						isDirectory = true;
						std::vector<Glib::ustring> names;
						Glib::RefPtr<Gio::File> dir = Gio::File::create_for_path ( argv[iArg] );
						safe_build_file_list (dir, names, argv[iArg] );
						for(size_t iFile=0; iFile< names.size(); iFile++ ){
							if( !safe_file_test( names[iFile] , Glib::FILE_TEST_IS_DIR)){
								// skip files without extension and pp3 files
								Glib::ustring s(names[iFile]);
								Glib::ustring::size_type ext= s.find_last_of('.');
								if( Glib::ustring::npos == ext )
									continue;
								if( ! s.substr(ext).compare( paramFileExtension ))
									continue;
								inputFiles.push_back( names[iFile] );
							}
						}
					}else{
						inputFiles.push_back( safe_filename_to_utf8 (argv[iArg]) );
					}
				}
				break;
			case 'h':
			case '?':
			default:
				std::cerr << "RawTherapee, " << VERSION << std::endl;
				std::cerr << "Copyright (c)2004-2011 Gabor Horvath <hgabor@rawtherapee.com>"<< std::endl << std::endl;
				std::cerr << "Usage:"<< std::endl;
				std::cerr << Glib::path_get_basename(argv[0]) << " [<selected dir>] : start RT GUI browser inside dir."<< std::endl;
				std::cerr << Glib::path_get_basename(argv[0]) << " <file> : start GUI editor with file."<< std::endl;
				std::cerr << Glib::path_get_basename(argv[0]) << " -c <inputDir>|<file list> : convert files in batch with default parameters."<< std::endl<< std::endl;
				std::cerr << "Other options used with -c (that must be last option) "<< std::endl;
				std::cerr << Glib::path_get_basename(argv[0]) <<" [-o <output> | -O <output>] [-s | -S | -p <file>] [-j[1-100]|-t|-n] -Y -c <input>"<< std::endl;
				std::cerr << " -o <outputFile>|<outputDir> : select output directory."<< std::endl;
				std::cerr << " -O <outputFile>|<outputDir> : select output dir and copy pp3 file into it"<< std::endl;
				std::cerr << " -s : select parameters to be pp3 file next to input file (with same name)"<< std::endl;
				std::cerr << "      ex: for IMG001.NEF there should be IMG001.NEF.pp3 in the same dir" << std::endl;
				std::cerr << "      if absent use default" << std::endl;
				std::cerr << " -S : like -s but skip if pp3 file not found." << std::endl;
				std::cerr << " -p <file.pp3> : specify pp3 file to be used for all conversions."<< std::endl;
				std::cerr << " -j[compression] : specify output to be jpeg.(default)"<< std::endl;
				std::cerr << " -t : specify output to be tif."<< std::endl;
				std::cerr << " -n : specify output to be png."<< std::endl;
				std::cerr << " -Y : overwrite output if present."<< std::endl;
				return -1;
			}
		}else{
			argv1 = safe_filename_to_utf8 ( argv[iArg] );
			if( outputDirectory ){
				options.savePathFolder = outputPath;
				options.saveUsePathTemplate = false;
			}
			else {
				options.saveUsePathTemplate = true;
				if (!options.savePathTemplate.length())
					// If the save path template is empty, we use its default value
					options.savePathTemplate = "%p1/converted/%f";
			}
			if (outputType == "jpg") {
				options.saveFormat.format = outputType;
				options.saveFormat.jpegQuality = compression;
			} else if (outputType == "tif") {
				options.saveFormat.format = outputType;
			} else if (outputType == "png") {
				options.saveFormat.format = outputType;
			}
			break;
		}
	}
	if( !argv1.empty() )
		return 1;
	if( !inputFiles.size() )
		return 2;

	rtengine::procparams::ProcParams params,paramsRaw,paramsImg, *currentParams;
	if( !sideProcParams ){
		if( processingParams.length()>0 )
			params.load ( processingParams );
		else{
			paramsRaw.load(options.profilePath+"/"+options.defProfRaw+paramFileExtension);
			paramsImg.load(options.profilePath+"/"+options.defProfImg+paramFileExtension);
		}
	}

	for( size_t iFile=0; iFile< inputFiles.size(); iFile++){
		Glib::ustring inputFile = inputFiles[iFile];
		std::cout << "Processing: " << inputFile << std::endl;

		rtengine::InitialImage* ii=NULL;
		rtengine::ProcessingJob* job =NULL;
		int errorCode;
		bool isRaw=false;

		Glib::ustring outputFile;
		if( outputType.empty() )
			outputType = "jpg";
		if( outputPath.empty() ){
			Glib::ustring s = inputFile;
			Glib::ustring::size_type ext= s.find_last_of('.');
			outputFile = s.substr(0,ext)+ "." + outputType;
		}else if( outputDirectory ){
			Glib::ustring s = Glib::path_get_basename( inputFile );
			Glib::ustring::size_type ext= s.find_last_of('.');
			outputFile = outputPath + "/" + s.substr(0,ext) + "." + outputType;
		}else{
			Glib::ustring s = outputPath;
			Glib::ustring::size_type ext= s.find_last_of('.');
			outputFile =  s.substr(0,ext) + "." + outputType;
		}
		if( inputFile == outputFile){
			std::cerr << "Cannot overwrite: " << inputFile << std::endl;
			continue;
		}
		if( !overwriteFiles && safe_file_test( outputFile , Glib::FILE_TEST_EXISTS ) ){
			std::cerr << outputFile  <<" already exists: use -Y option to overwrite. This image has been skipped." << std::endl;
			continue;
		}

		// Load the image
		ii = rtengine::InitialImage::load ( inputFile, true, &errorCode, NULL );
		if (ii)
			isRaw=true;
		else
			ii = rtengine::InitialImage::load ( inputFile , false, &errorCode, NULL );
		if (!ii) {
			errors++;
			std::cerr << "Error loading file: "<< inputFile << std::endl;
			continue;
		}
		if( sideProcParams ){
			Glib::ustring sideProcessingParams = inputFile + paramFileExtension;
			if( !safe_file_test( sideProcessingParams, Glib::FILE_TEST_EXISTS ) || params.load ( sideProcessingParams )){
				if( useDefaultIfAbsent ){
					currentParams = isRaw? &paramsRaw: &paramsImg;
				}else{
					delete ii;
					errors++;
					std::cerr << "Error loading processing params: "<< sideProcessingParams << std::endl;
					continue;
				}
			}else
				currentParams = &params;
		}else if( processingParams.length()>0 ){
			currentParams = &params;
		}else if(isRaw ){
			currentParams = &paramsRaw;
		}else{
			currentParams = &paramsImg;
		}
		job = rtengine::ProcessingJob::create (ii, *currentParams);
		if( !job ){
			errors++;
			std::cerr << "Error creating processing for: "<< inputFile << std::endl;
			ii->decreaseRef();
			continue;
		}

		// Process image
		rtengine::IImage16* resultImage = rtengine::processImage (job, errorCode, NULL, options.tunnelMetaData);
        if( !resultImage ){
        	errors++;
        	std::cerr << "Error processing: "<< inputFile << std::endl;
        	rtengine::ProcessingJob::destroy( job );
    		continue;
        }
		// save image to disk
		if( outputType=="jpg" )
			errorCode = resultImage->saveAsJPEG( outputFile, compression );
		else if( outputType=="tif" )
			errorCode = resultImage->saveAsTIFF( outputFile, bits, compression==0  );
		else if( outputType=="png" )
			errorCode = resultImage->saveAsPNG( outputFile,compression, bits );
		else
			errorCode = resultImage->saveToFile (outputFile);

		if(errorCode){
			errors++;
			std::cerr << "Error saving to: "<< outputFile << std::endl;
		}else{
			if( copyParamsFile ){
			   Glib::ustring outputProcessingParams = outputFile + paramFileExtension;
			   currentParams->save( outputProcessingParams );
			}
		}

		ii->decreaseRef();
		resultImage->free();
	}
	return errors>0?-2:0;
}
