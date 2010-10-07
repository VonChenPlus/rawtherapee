/*
 * rtcmd.cc
 *
 *  Created on: Oct 7, 2010
 *      Author: gabor
 */

#include <glibmm.h>
#include <iostream>
#include <string.h>
#include "rtengine.h"

using namespace rtengine;

void printUsage () {

    std::cout << "usage: rtcmd -i <infilename> -o <outfilename> -p <ppfilename> -f <outformat>" << std::endl;
    std::cout << "Available formats: \"jpg\", \"png\", \"png16\", \"tif\", \"tif16\"" << std::endl;
    exit (1);
}

int main (int argc, char* argv[]) {

    Glib::ustring inFName = "", outFName = "", ppName = "", format = "";

    for (int i=1; i<argc; i++)
        if (!strcmp (argv[i], "-i") && i<argc-1)
            inFName = argv[++i];
        else if (!strcmp (argv[i], "-o") && i<argc-1)
            outFName = argv[++i];
        else if (!strcmp (argv[i], "-p") && i<argc-1)
            ppName = argv[++i];
        else if (!strcmp (argv[i], "-f") && i<argc-1)
            format = argv[++i];
        else {
            std::cerr << "Unknown parameter: " << argv[i] << std::endl;
            printUsage ();
        }

    if (inFName == "") {
        std::cerr << "Input file name missing." << std::endl;
        printUsage ();
    }
    if (outFName == "") {
        std::cerr << "Output file name missing." << std::endl;
        printUsage ();
    }
    if (ppName == "") {
        std::cerr << ".pp file name missing." << std::endl;
        printUsage ();
    }
    if (format == "")
        format = "jpg";

    Glib::thread_init();
    new Settings;

    int errorCode = 0;
    InitialImage* iImg = InitialImage::load (inFName, true, errorCode);

    if (errorCode) {
        std::cerr << "Could not load image (error code = " << errorCode << ")\n";
        exit(2);
    }

    ProcParams params;
    if (errorCode = params.load (ppName)) {
        std::cerr << "Could not load pp file (error code = " << errorCode << ")\n";
        exit(2);
    }

    IImage16* img = SingleImageProcessor::process (ProcessingJob::create(iImg, params), NULL, NULL, errorCode);

    img->saveToFile (outFName);

    if (errorCode) {
        std::cerr << "An error occurred during processing (error code = " << errorCode << ")\n";
        exit(2);
    }


}

