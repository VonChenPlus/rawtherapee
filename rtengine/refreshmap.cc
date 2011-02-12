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
#include <refreshmap.h>
#include <procevents.h>

int refreshmap[rtengine::NUMOFEVENTS] = {
ALL,              // EvPhotoLoaded,
ALL,              // EvProfileLoaded,
ALL,              // EvProfileChanged,
ALL,              // EvHistoryBrowsed,
RGBCURVE,         // EvBrightness,
RGBCURVE,         // EvContrast,
RGBCURVE,         // EvBlack,
RGBCURVE,         // EvExpComp,
RGBCURVE,         // EvHLCompr,
RGBCURVE,         // EvSHCompr,
RGBCURVE,         // EvToneCurve,
AUTOEXP,          // EvAutoExp,
AUTOEXP,          // EvClip,
LUMINANCECURVE,   // EvLBrightness,
LUMINANCECURVE,   // EvLContrast,
LUMINANCECURVE,   // EvLBlack,
LUMINANCECURVE,   // EvLHLCompr,
LUMINANCECURVE,   // EvLSHCompr,
LUMINANCECURVE,   // EvLLCurve,
SHARPENING,       // EvShrEnabled,
SHARPENING,       // EvShrRadius,
SHARPENING,       // EvShrAmount,
SHARPENING,       // EvShrThresh,
SHARPENING,       // EvShrEdgeOnly,
SHARPENING,       // EvShrEdgeRadius,
SHARPENING,       // EvShrEdgeTolerance,
SHARPENING,       // EvShrHaloControl,
SHARPENING,       // EvShrHaloAmount,
SHARPENING,       // EvShrMethod,
SHARPENING,       // EvShrDRadius,
SHARPENING,       // EvShrDAmount,
SHARPENING,       // EvShrDDamping,
SHARPENING,       // EvShrDIterations,
COLORBOOST,       // EvCBAvoidClip,
COLORBOOST,       // EvCBSatLimiter,
COLORBOOST,       // EvCBSatLimit,
COLORBOOST,       // EvCBBoost,
WHITEBALANCE,     // EvWBMethod,
WHITEBALANCE,     // EvWBTemp,
WHITEBALANCE,     // EvWBGreen,
COLORBOOST,       // EvCShiftA,
COLORBOOST,       // EvCShiftB,
LUMADENOISE,      // EvLDNEnabled,
LUMADENOISE,      // EvLDNRadius,
LUMADENOISE,      // EvLDNEdgeTolerance,
COLORDENOISE,     // EvCDNEnabled,
COLORDENOISE,     // EvCDNRadius,
COLORDENOISE,     // EvCDNEdgeTolerance,
COLORDENOISE,     // EvCDNEdgeSensitive,
RETINEX,          // EvSHEnabled,
RGBCURVE,         // EvSHHighlights,
RGBCURVE,         // EvSHShadows,
RGBCURVE,         // EvSHHLTonalW,
RGBCURVE,         // EvSHSHTonalW,
RGBCURVE,         // EvSHLContrast,
RETINEX,          // EvSHRadius,
ALL,              // EvCTRotate,
ALL,              // EvCTHFlip,
ALL,              // EvCTVFlip,
TRANSFORM,        // EvROTDegree,
TRANSFORM,        // EvTransAutoFill,
TRANSFORM,        // EvDISTAmount,
ALL,              // EvBookmarkSelected,
CROP,             // EvCrop,
TRANSFORM,        // EvCACorr,
ALL,              // EvHREnabled,
ALL,              // EvHRAmount,
ALL,              // EvHRMethod,
ALL,              // EvWProfile,
ALL,              // EvOProfile,
ALL,              // EvIProfile,
TRANSFORM,        // EvVignetting,
RGBCURVE,         // EvChMixer,
RESIZE,           // EvResizeScale,
RESIZE,           // EvResizeMethod,
EXIF,             // EvExif,
IPTC,             // EvIPTC
RESIZE,           // EvResizeSpec,
RESIZE,           // EvResizeWidth
RESIZE,           // EvResizeHeight
RESIZE,           // EvResizeEnabled
ALL,              // EvProfileChangeNotification
RETINEX,          // EvShrHighQuality
TRANSFORM,        // EvPerspCorr
EQUALIZER,        // EvEqualizer
EQUALIZER,        // EvEqlEnabled
IMPULSEDENOISE,   // EvIDNEnabled,
IMPULSEDENOISE,   // EvIDNThresh,
DIRPYRDENOISE,    // EvDPDNEnabled,
DIRPYRDENOISE,    // EvDPDNLuma,
DIRPYRDENOISE,    // EvDPDNChroma,
DIRPYRDENOISE,    // EvDPDNGamma,
DIRPYREQUALIZER,  // EvDirPyrEqualizer,
DIRPYREQUALIZER,  // EvDirPyrEqlEnabled,
LUMINANCECURVE,   // EvLSaturation,
LUMINANCECURVE,   // EvLaCurve,
LUMINANCECURVE,   // EvLbCurve,
DEMOSAIC,         // EvDemosaic
DARKFRAME,        // EvPreProcess
RGBCURVE,         // EvSaturation,
RGBCURVE,         // EvHSVEqualizerH,
RGBCURVE,         // EvHSVEqualizerS,
RGBCURVE,         // EvHSVEqualizerV,
RGBCURVE,         // EvHSVEqEnabled,
DEFRINGE,         // EvDefringeEnabled,
DEFRINGE,         // EvDefringeRadius,
DEFRINGE,         // EvDefringeThreshold,
RGBCURVE,         // EvHLComprThreshold,
RESIZE,           // EvResizeBoundingBox
RESIZE,           // EvResizeAppliesTo
LUMINANCECURVE,   // EvCBAvoidClip,
LUMINANCECURVE,   // EvCBSatLimiter,
LUMINANCECURVE,   // EvCBSatLimit,
FLATFIELD,        // EvFlatFieldFile,
FLATFIELD,        // EvFlatFieldAutoSelect,
FLATFIELD,        // EvFlatFieldBlurRadius,
FLATFIELD         // EvFlatFieldBlurType,

};

