// ==========================================================
// Tag library
//
// Design and implementation by
// - Hervé Drolon <drolon@infonie.fr>
//
// This file is part of FreeImage 3
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// ==========================================================

// ==========================================================
// Implementation notes : 
// ----------------------
// The tag info tables declared in this file should probably 
// be loaded from an XML file. 
// This would allow internationalization features and also 
// more extensibility. 
// Maybe in a future release ? 
// ==========================================================

#pragma warning (disable : 4786) // identifier was truncated to 'number' characters

#include "FreeImage.h"
#include "Utilities.h"
#include "FreeImageTag.h"

/**
 HOW-TO : add a new TagIngo table
 --------------------------------------------------------------------------
 1) add a table identifier in the TagLib class definition (see enum MDMODEL)
 2) declare the tag table as static and use a 0/NULL value as last entry
 3) initialize the table in TagLib::TagLib
 4) provide a conversion in TagLib::getFreeImageModel
*/

// --------------------------------------------------------------------------
// EXIF standard tags definition
// --------------------------------------------------------------------------

static TagInfo
  exif_exif_tag_table[] =
  {
    {  0x0100, (char *) "ImageWidth", (char *) "Image width"},
    {  0x0101, (char *) "ImageLength", (char *) "Image height"},
    {  0x0102, (char *) "BitsPerSample", (char *) "Number of bits per component"},
    {  0x0103, (char *) "Compression", (char *) "Compression scheme"},
    {  0x0106, (char *) "PhotometricInterpretation", (char *) "Pixel composition"},
    {  0x010A, (char *) "FillOrder", (char*) NULL},
    {  0x010D, (char *) "DocumentName", (char *) NULL},
    {  0x010E, (char *) "ImageDescription", (char *) "Image title"},
    {  0x010F, (char *) "Make", (char *) "Image input equipment manufacturer"},
    {  0x0110, (char *) "Model", (char *) "Image input equipment model"},
    {  0x0111, (char *) "StripOffsets", (char *) "Image data location"},
    {  0x0112, (char *) "Orientation", (char *) "Orientation of image"},
    {  0x0115, (char *) "SamplesPerPixel", (char *) "Number of components"},
    {  0x0116, (char *) "RowsPerStrip", (char *) "Number of rows per strip"},
    {  0x0117, (char *) "StripByteCounts", (char *) "Bytes per compressed strip"},
    {  0x011A, (char *) "XResolution", (char *) "Image resolution in width direction"},
    {  0x011B, (char *) "YResolution", (char *) "Image resolution in height direction"},
    {  0x011C, (char *) "PlanarConfiguration", (char *) "Image data arrangement"},
    {  0x0128, (char *) "ResolutionUnit", (char *) "Unit of X and Y resolution"},
    {  0x012D, (char *) "TransferFunction", (char *) "Transfer function"},
    {  0x0131, (char *) "Software", (char *) "Software used"},
    {  0x0132, (char *) "DateTime", (char *) "File change date and time"},
    {  0x013B, (char *) "Artist", (char *) "Person who created the image"},
    {  0x013E, (char *) "WhitePoint", (char *) "White point chromaticity"},
    {  0x013F, (char *) "PrimaryChromaticities", (char *) "Chromaticities of primaries"},
    {  0x0156, (char *) "TransferRange", (char *) NULL},
    {  0x0200, (char *) "JPEGProc", (char *) NULL},
    {  0x0201, (char *) "JPEGInterchangeFormat", (char *) "Offset to JPEG SOI"},
    {  0x0202, (char *) "JPEGInterchangeFormatLength", (char *) "Bytes of JPEG data"},
    {  0x0211, (char *) "YCbCrCoefficients", (char *) "Color space transformation matrix coefficients"},
    {  0x0212, (char *) "YCbCrSubSampling", (char *) "Subsampling ratio of Y to C"},
    {  0x0213, (char *) "YCbCrPositioning", (char *) "Y and C positioning"},
    {  0x0214, (char *) "ReferenceBlackWhite", (char *) "Pair of black and white reference values"},
    {  0x828D, (char *) "CFARepeatPatternDim", (char *) NULL},
    {  0x828E, (char *) "CFAPattern", (char *) NULL},
    {  0x828F, (char *) "BatteryLevel", (char *) NULL},
    {  0x8298, (char *) "Copyright", (char *) "Copyright holder"},
    {  0x829A, (char *) "ExposureTime", (char *) "Exposure time"},
    {  0x829D, (char *) "FNumber", (char *) "F number"},
    {  0x83BB, (char *) "IPTC/NAA", (char *) NULL},
    {  0x8773, (char *) "InterColorProfile", (char *) NULL},
    {  0x8822, (char *) "ExposureProgram", (char *) "Exposure program"},
    {  0x8824, (char *) "SpectralSensitivity", (char *) "Spectral sensitivity"},
    {  0x8825, (char *) "GPSInfo", (char *) NULL},
    {  0x8827, (char *) "ISOSpeedRatings", (char *) "ISO speed rating"},
    {  0x8828, (char *) "OECF", (char *) "Optoelectric conversion factor"},
    {  0x9000, (char *) "ExifVersion", (char *) "Exif version"},
    {  0x9003, (char *) "DateTimeOriginal", (char *) "Date and time of original data generation"},
    {  0x9004, (char *) "DateTimeDigitized", (char *) "Date and time of digital data generation"},
    {  0x9101, (char *) "ComponentsConfiguration", (char *) "Meaning of each component"},
    {  0x9102, (char *) "CompressedBitsPerPixel", (char *) "Image compression mode"},
    {  0x9201, (char *) "ShutterSpeedValue", (char *) "Shutter speed"},
    {  0x9202, (char *) "ApertureValue", (char *) "Aperture"},
    {  0x9203, (char *) "BrightnessValue", (char *) "Brightness"},
    {  0x9204, (char *) "ExposureBiasValue", (char *) "Exposure bias"},
    {  0x9205, (char *) "MaxApertureValue", (char *) "Maximum lens aperture"},
    {  0x9206, (char *) "SubjectDistance", (char *) "Subject distance"},
    {  0x9207, (char *) "MeteringMode", (char *) "Metering mode"},
    {  0x9208, (char *) "LightSource", (char *) "Light source"},
    {  0x9209, (char *) "Flash", (char *) "Flash"},
    {  0x920A, (char *) "FocalLength", (char *) "Lens focal length"},
	{  0x9214, (char *) "SubjectArea", (char *) "Subject area"},
    {  0x927C, (char *) "MakerNote", (char *) "Manufacturer notes"},
    {  0x9286, (char *) "UserComment", (char *) "User comments"},
    {  0x9290, (char *) "SubSecTime", (char *) "DateTime subseconds"},
    {  0x9291, (char *) "SubSecTimeOriginal", (char *) "DateTimeOriginal subseconds"},
    {  0x9292, (char *) "SubSecTimeDigitized", (char *) "DateTimeDigitized subseconds"},
    {  0xA000, (char *) "FlashPixVersion", (char *) "Supported Flashpix version"},
    {  0xA001, (char *) "ColorSpace", (char *) "Color space information"},
    {  0xA002, (char *) "PixelXDimension", (char *) "Valid image width"},
    {  0xA003, (char *) "PixelYDimension", (char *) "Valid image height"},
    {  0xA004, (char *) "RelatedSoundFile", (char *) "Related audio file"},
    {  0xA005, (char *) "InteroperabilityOffset", (char *) NULL},
    {  0xA20B, (char *) "FlashEnergy", (char *) "Flash energy"},
    {  0xA20C, (char *) "SpatialFrequencyResponse", (char *) "Spatial frequency response"},
    {  0xA20E, (char *) "FocalPlaneXResolution", (char *) "Focal plane X resolution"},
    {  0xA20F, (char *) "FocalPlaneYResolution", (char *) "Focal plane Y resolution"},
    {  0xA210, (char *) "FocalPlaneResolutionUnit", (char *) "Focal plane resolution unit"},
    {  0xA214, (char *) "SubjectLocation", (char *) "Subject location"},
    {  0xA215, (char *) "ExposureIndex", (char *) "Exposure index"},
    {  0xA217, (char *) "SensingMethod", (char *) "Sensing method"},
    {  0xA300, (char *) "FileSrc", (char *) "File source"},
    {  0xA301, (char *) "SceneType", (char *) "Scene type"},
    {  0xA302, (char *) "CFAPattern", (char *) "CFA pattern"},
    {  0xA401, (char *) "CustomRendered", (char *) "Custom image processing"},
    {  0xA402, (char *) "ExposureMode", (char *) "Exposure mode"},
    {  0xA403, (char *) "WhiteBalance", (char *) "White balance"},
    {  0xA404, (char *) "DigitalZoomRatio", (char *) "Digital zoom ratio"},
    {  0xA405, (char *) "FocalLengthIn35mmFilm", (char *) "Focal length in 35 mm film"},
    {  0xA406, (char *) "SceneCaptureType", (char *) "Scene capture type"},
    {  0xA407, (char *) "GainControl", (char *) "Gain control"},
    {  0xA408, (char *) "Contrast", (char *) "Contrast"},
    {  0xA409, (char *) "Saturation", (char *) "Saturation"},
    {  0xA40A, (char *) "Sharpness", (char *) "Sharpness"},
    {  0xA40B, (char *) "DeviceSettingDescription", (char *) "Device settings description"},
    {  0xA40C, (char *) "SubjectDistanceRange", (char *) "Subject distance range"},
    {  0xA420, (char *) "ImageUniqueID", (char *) "Unique image ID"},
    {  0x0000, (char *) NULL, (char *) NULL}
  };

// --------------------------------------------------------------------------
// EXIF GPS tags definition
// --------------------------------------------------------------------------

static TagInfo
  exif_gps_tag_table[] =
  {
    {  0x0000, (char *) "GPSVersionID", (char *) "GPS tag version"},
	{  0x0001, (char *) "GPSLatitudeRef", (char *) "North or South Latitude"},
    {  0x0002, (char *) "GPSLatitude", (char *) "Latitude"},
    {  0x0003, (char *) "GPSLongitudeRef", (char *) "East or West Longitude"},
    {  0x0004, (char *) "GPSLongitude", (char *) "Longitude"},
    {  0x0005, (char *) "GPSAltitudeRef", (char *) "Altitude reference"},
    {  0x0006, (char *) "GPSAltitude", (char *) "Altitude"},
    {  0x0007, (char *) "GPSTimeStamp", (char *) "GPS time (atomic clock)"},
    {  0x0008, (char *) "GPSSatellites", (char *) "GPS satellites used for measurement"},
    {  0x0009, (char *) "GPSStatus", (char *) "GPS receiver status"},
    {  0x000A, (char *) "GPSMeasureMode", (char *) "GPS measurement mode"},
    {  0x000B, (char *) "GPSDOP", (char *) "Measurement precision"},
    {  0x000C, (char *) "GPSSpeedRef", (char *) "Speed unit"},
    {  0x000D, (char *) "GPSSpeed", (char *) "Speed of GPS receiver"},
    {  0x000E, (char *) "GPSTrackRef", (char *) "Reference for direction of movement"},
    {  0x000F, (char *) "GPSTrack", (char *) "Direction of movement"},
    {  0x0010, (char *) "GPSImgDirectionRef", (char *) "Reference for direction of image"},
    {  0x0011, (char *) "GPSImgDirection", (char *) "Direction of image"},
    {  0x0012, (char *) "GPSMapDatum", (char *) "Geodetic survey data used"},
    {  0x0013, (char *) "GPSDestLatitudeRef", (char *) "Reference for latitude of destination"},
    {  0x0014, (char *) "GPSDestLatitude", (char *) "Latitude of destination"},
    {  0x0015, (char *) "GPSDestLongitudeRef", (char *) "Reference for longitude of destination"},
    {  0x0016, (char *) "GPSDestLongitude", (char *) "Longitude of destination"},
    {  0x0017, (char *) "GPSDestBearingRef", (char *) "Reference for bearing of destination"},
    {  0x0018, (char *) "GPSDestBearing", (char *) "Bearing of destination"},
    {  0x0019, (char *) "GPSDestDistanceRef", (char *) "Reference for distance to destination"},
    {  0x001A, (char *) "GPSDestDistance", (char *) "Distance to destination"},
    {  0x001B, (char *) "GPSProcessingMethod", (char *) "Name of GPS processing method"},
    {  0x001C, (char *) "GPSAreaInformation", (char *) "Name of GPS area"},
    {  0x001D, (char *) "GPSDateStamp", (char *) "GPS date"},
    {  0x001E, (char *) "GPSDifferential", (char *) "GPS differential correction"},
    {  0x0000, (char *) NULL, (char *) NULL}
  };

// --------------------------------------------------------------------------
// EXIF interoperability tags definition
// --------------------------------------------------------------------------

static TagInfo
  exif_interop_tag_table[] =
  {
    {  0x0001, (char *) "InteroperabilityIndex", (char *) "Interoperability Identification"},
    {  0x0002, (char *) "InteroperabilityVersion", (char *) "Interoperability version"},
    {  0x1000, (char *) "RelatedImageFileFormat", (char *) "File format of image file"},
    {  0x1001, (char *) "RelatedImageWidth", (char *) "Image width"},
    {  0x1002, (char *) "RelatedImageLength", (char *) "Image height"},
    {  0x0000, (char *) NULL, (char *) NULL}
  };

// --------------------------------------------------------------------------
// EXIF maker note tags definition
// --------------------------------------------------------------------------

/**
Canon maker note
*/
static TagInfo
  exif_canon_tag_table[] =
  {
    {  0x0006, (char *) "Image Type", (char *) NULL},
    {  0x0007, (char *) "Firmware Version", (char *) NULL},
    {  0x0008, (char *) "Image Number", (char *) NULL},
    {  0x0009, (char *) "Owner Name", (char *) NULL},
    {  0x000C, (char *) "Camera Serial Number", (char *) NULL},
    {  0x000F, (char *) "Custom Functions", (char *) NULL},

	// These 'sub'-tag values have been created for consistency -- they don't exist within the exif segment

	// Fields under tag 0x0001 (we add 0xC100 to make unique tag id)
    {  0xC101, (char *) "Macro Mode", (char *) NULL},
    {  0xC102, (char *) "Self Timer Delay", (char *) NULL},
    {  0xC103, (char *) "Compression Setting", (char *) NULL},
    {  0xC104, (char *) "Flash Mode", (char *) NULL},
    {  0xC105, (char *) "Continuous Drive Mode", (char *) NULL},
    {  0xC107, (char *) "Focus Mode 1", (char *) NULL},
    {  0xC10A, (char *) "Image Size", (char *) NULL},
    {  0xC10B, (char *) "Easy Shooting Mode", (char *) NULL},
    {  0xC10C, (char *) "Digital Zoom", (char *) NULL},
    {  0xC10D, (char *) "Contrast", (char *) NULL},
    {  0xC10E, (char *) "Saturation", (char *) NULL},
    {  0xC10F, (char *) "Sharpness", (char *) NULL},
    {  0xC110, (char *) "Iso", (char *) NULL},
    {  0xC111, (char *) "Metering Mode", (char *) NULL},
    {  0xC112, (char *) "Focus Type", (char *) NULL},
    {  0xC113, (char *) "AF Point Selected", (char *) NULL},
    {  0xC114, (char *) "Exposure Mode", (char *) NULL},
    {  0xC117, (char *) "Long Focal Length", (char *) NULL},
    {  0xC118, (char *) "Short Focal Length", (char *) NULL},
    {  0xC119, (char *) "Focal Units per mm", (char *) NULL},
    {  0xC11D, (char *) "Flash Details", (char *) NULL},
    {  0xC120, (char *) "Focus Mode 2", (char *) NULL},
    {  0xC124, (char *) "Zoomed Resolution", (char *) NULL},
    {  0xC125, (char *) "Base Zoom Resolution", (char *) NULL},
	
	// Fields under tag 0x0004 (we add 0xC400 to make unique tag id)
    {  0xC407, (char *) "White Balance", (char *) NULL},
    {  0xC409, (char *) "Sequence Number", (char *) NULL},
    {  0xC40E, (char *) "AF Point Used", (char *) NULL},
    {  0xC40F, (char *) "Flash Bias", (char *) NULL},
    {  0xC413, (char *) "Subject Distance", (char *) NULL},
    {  0x0000, (char *) NULL, (char *) NULL}
  };

/**
Casio maker note
*/
static TagInfo
  exif_casio_tag_table[] =
  {
    {  0x0001, (char *) "Recording Mode", (char *) NULL},
    {  0x0002, (char *) "Quality", (char *) NULL},
    {  0x0003, (char *) "Focusing Mode", (char *) NULL},
    {  0x0004, (char *) "Flash Mode", (char *) NULL},
    {  0x0005, (char *) "Flash Intensity", (char *) NULL},
    {  0x0006, (char *) "Object Distance", (char *) NULL},
    {  0x0007, (char *) "White Balance", (char *) NULL},
    {  0x000A, (char *) "Digital Zoom", (char *) NULL},
    {  0x000B, (char *) "Sharpness", (char *) NULL},
    {  0x000C, (char *) "Contrast", (char *) NULL},
    {  0x000D, (char *) "Saturation", (char *) NULL},
    {  0x0014, (char *) "CCD Sensitivity", (char *) NULL},
    {  0x0000, (char *) NULL, (char *) NULL}
  };

/**
FujiFilm maker note
*/
static TagInfo
  exif_fujifilm_tag_table[] =
  {
    {  0x0000, (char *) "Makernote Version", (char *) NULL},
    {  0x1000, (char *) "Quality", (char *) NULL},
    {  0x1001, (char *) "Sharpness", (char *) NULL},
    {  0x1002, (char *) "White Balance", (char *) NULL},
    {  0x1003, (char *) "Color", (char *) NULL},
    {  0x1004, (char *) "Tone", (char *) NULL},
    {  0x1010, (char *) "Flash Mode", (char *) NULL},
    {  0x1011, (char *) "Flash Strength", (char *) NULL},
    {  0x1020, (char *) "Macro", (char *) NULL},
    {  0x1021, (char *) "Focus Mode", (char *) NULL},
    {  0x1030, (char *) "Slow Synchro", (char *) NULL},
    {  0x1031, (char *) "Picture Mode", (char *) NULL},
    {  0x1100, (char *) "Continuous Taking Or Auto Bracketting", (char *) NULL},
    {  0x1300, (char *) "Blur Warning", (char *) NULL},
    {  0x1301, (char *) "Focus Warning", (char *) NULL},
    {  0x1302, (char *) "AE Warning", (char *) NULL},
    {  0x0000, (char *) NULL, (char *) NULL}
  };

/**
Kyocera maker note
*/
static TagInfo
  exif_kyocera_tag_table[] =
  {
    {  0x0001, (char *) "ThumbnailImage", (char *) NULL},
    {  0x0E00, (char *) "PrintIM", (char *) "Print Image Matching Info"},
    {  0x0000, (char *) NULL, (char *) NULL}
  };

/**
Olympus maker note
*/
static TagInfo
  exif_olympus_tag_table[] =
  {
    {  0x0200, (char *) "Special Mode", (char *) NULL},
    {  0x0201, (char *) "Jpeg Quality", (char *) NULL},
    {  0x0202, (char *) "Macro", (char *) NULL},
    {  0x0204, (char *) "Digital Zoom", (char *) NULL},
    {  0x0207, (char *) "Firmware Version", (char *) NULL},
    {  0x0208, (char *) "Picture Info", (char *) NULL},
    {  0x0209, (char *) "Camera ID", (char *) NULL},
    {  0x0F00, (char *) "Data Dump", (char *) NULL},
    {  0x0000, (char *) NULL, (char *) NULL}
  };

/**
Minolta maker note
*/
static TagInfo
  exif_minolta_tag_table[] =
  {
    {  0x0000, (char *) "Version", (char *) NULL},
	{  0x0040, (char *) "CompressedImageSize", (char *) NULL},
    {  0x0081, (char *) "JPEGThumbnailImage", (char *) NULL},
    {  0x0088, (char *) "JPEGThumbnailOffset", (char *) NULL},
    {  0x0089, (char *) "JPEGThumbnailLength", (char *) NULL},
    {  0x0101, (char *) "ColorMode", (char *) NULL},
    {  0x0102, (char *) "ImageQuality", (char *) NULL},
    {  0x0103, (char *) "ImageQuality 2", (char *) NULL},
    {  0x0000, (char *) NULL, (char *) NULL}
  };

/**
There are 3 formats of Nikon's MakerNote. MakerNote of E700/E800/E900/E900S/E910/E950
starts from ASCII string "Nikon". Data format is the same as IFD, but it starts from
offset 0x08. This is the same as Olympus except start string. 
*/

/**
TYPE 1 is for E-Series cameras prior to (not including) E990
*/
static TagInfo
  exif_nikon_type1_tag_table[] =
  {
    {  0x0003, (char *) "Quality", (char *) NULL},
    {  0x0004, (char *) "Color Mode", (char *) NULL},
    {  0x0005, (char *) "Image Adjustment", (char *) NULL},
    {  0x0006, (char *) "CCD Sensitivity", (char *) NULL},
    {  0x0007, (char *) "White Balance", (char *) NULL},
    {  0x0008, (char *) "Focus", (char *) NULL},
    {  0x000A, (char *) "Digital Zoom", (char *) NULL},
    {  0x000B, (char *) "Fisheye Converter", (char *) NULL},
    {  0x0000, (char *) NULL, (char *) NULL}
  };

static TagInfo
  exif_nikon_type2_tag_table[] =
  {
    {  0x0002, (char *) "ISO Setting", (char *) NULL},
    {  0x0003, (char *) "Color Mode", (char *) NULL},
    {  0x0004, (char *) "Quality", (char *) NULL},
    {  0x0005, (char *) "White Balance", (char *) NULL},
    {  0x0006, (char *) "Image Sharpening", (char *) NULL},
    {  0x0007, (char *) "Focus Mode", (char *) NULL},
    {  0x0008, (char *) "Flash Setting", (char *) NULL},
    {  0x000F, (char *) "ISO Selection", (char *) NULL},
    {  0x0080, (char *) "Image Adjustment", (char *) NULL},
    {  0x0082, (char *) "Adapter", (char *) NULL},
    {  0x0085, (char *) "Focus Distance", (char *) NULL},
    {  0x0086, (char *) "Digital Zoom", (char *) NULL},
    {  0x0088, (char *) "AF Focus Position", (char *) NULL},
    {  0x0010, (char *) "Data Dump", (char *) NULL},
    {  0x0000, (char *) NULL, (char *) NULL}
  };

/**
The type-3 directory is for D-Series cameras such as the D1 and D100.
see http://www.timelesswanderings.net/equipment/D100/NEF.html
*/
static TagInfo
  exif_nikon_type3_tag_table[] =
  {
    {  0x0001, (char *) "Firmware Version", (char *) NULL},
    {  0x0002, (char *) "ISO", (char *) NULL},
    {  0x0004, (char *) "File Format", (char *) NULL},
    {  0x0005, (char *) "White Balance", (char *) NULL},
    {  0x0006, (char *) "Sharpening", (char *) NULL},
    {  0x0007, (char *) "AF Type", (char *) NULL},
    {  0x000B, (char *) "White Balance Fine", (char *) NULL},
    {  0x000C, (char *) "White Balance RB Coefficients", (char *) NULL},
    {  0x0013, (char *) "ISO", (char *) NULL},
    {  0x0081, (char *) "Tone Compensation", (char *) NULL},
    {  0x0084, (char *) "Lens", (char *) NULL},
    {  0x008D, (char *) "Color Mode", (char *) NULL},
    {  0x0092, (char *) "Camera Hue Adjustment", (char *) NULL},
    {  0x0095, (char *) "Noise Reduction", (char *) NULL},
    {  0x0E01, (char *) "Capture Editor Data", (char *) NULL},
    {  0x0000, (char *) NULL, (char *) NULL}
  };

/**
Panasonic maker note
*/
static TagInfo
  exif_panasonic_tag_table[] =
  {
    {  0x0001, (char *) "Image Quality", (char *) NULL},
    {  0x0002, (char *) "Version", (char *) NULL},
    {  0x0003, (char *) "White Balance", (char *) NULL},
    {  0x000F, (char *) "Spot Mode", (char *) NULL},
    {  0x001A, (char *) "Image Stabilizer", (char *) NULL},
    {  0x001C, (char *) "Macro Mode", (char *) NULL},
    {  0x001F, (char *) "Shooting Mode", (char *) NULL},
    {  0x0020, (char *) "Audio", (char *) NULL},
    {  0x0023, (char *) "White Balance Adjust", (char *) NULL},
    {  0x0024, (char *) "Flash Bias", (char *) NULL},
    {  0x0028, (char *) "Color Effect", (char *) NULL},
    {  0x0000, (char *) NULL, (char *) NULL}
  };

/**
Pentax (Asahi) maker note
*/
static TagInfo
  exif_pentax_tag_table[] =
  {
    {  0x0001, (char *) "Capture Mode", (char *) NULL},
    {  0x0002, (char *) "Quality Level", (char *) NULL},
    {  0x0003, (char *) "Focus Mode", (char *) NULL},
    {  0x0004, (char *) "Flash Mode", (char *) NULL},
    {  0x0007, (char *) "White Balance", (char *) NULL},
    {  0x000A, (char *) "Digital Zoom", (char *) NULL},
    {  0x000B, (char *) "Sharpness", (char *) NULL},
    {  0x000C, (char *) "Contrast", (char *) NULL},
    {  0x000D, (char *) "Saturation", (char *) NULL},
    {  0x0014, (char *) "ISO Speed", (char *) NULL},
    {  0x0017, (char *) "Color", (char *) NULL},
    {  0x0E00, (char *) "PrintIM", (char *) NULL},
    {  0x1000, (char *) "Time Zone", (char *) NULL},
    {  0x1001, (char *) "Daylight Savings", (char *) NULL},
    {  0x0000, (char *) NULL, (char *) NULL}
  };

// --------------------------------------------------------------------------
// IPTC tags definition
// --------------------------------------------------------------------------

static TagInfo
  iptc_tag_table[] =
  {
    {  0x0200, (char *) "DirectoryVersion", (char *) "Directory Version"},
    {  0x0278, (char *) "Caption/Abstract", (char *) "Caption"},
    {  0x027A, (char *) "Writer/Editor", (char *) "Caption Writer"},
    {  0x0269, (char *) "Headline", (char *) "Headline"},
    {  0x0228, (char *) "SpecialInstructions", (char *) "Instructions"},
    {  0x0250, (char *) "ByLine", (char *) "Author"},
    {  0x0255, (char *) "BylineTitle", (char *) "Author's Position"},
    {  0x026E, (char *) "Credit", (char *) "Credit"},
    {  0x0273, (char *) "Source", (char *) "Source"},
    {  0x0205, (char *) "ObjectName", (char *) "Title"},
    {  0x0237, (char *) "DateCreated", (char *) "Date Created"},
    {  0x025A, (char *) "City", (char *) "City"},
    {  0x025F, (char *) "Province/State", (char *) "State/Province"},
    {  0x0265, (char *) "Country/PrimaryLocation", (char *) "Country"},
    {  0x0267, (char *) "OriginalTransmissionReference", (char *) "Transmission Reference"},
    {  0x020F, (char *) "Category", (char *) "Category"},
    {  0x0214, (char *) "SupplementalCategory", (char *) "Supplemental Categories"},
    {  0x020A, (char *) "Urgency", (char *) "Urgency"},
    {  0x0219, (char *) "Keywords", (char *) "Keywords"},
    {  0x0274, (char *) "CopyrightNotice", (char *) "Copyright Notice"},
    {  0x021E, (char *) "ReleaseDate", (char *) "Release Date"},
    {  0x0223, (char *) "ReleaseTime", (char *) "Release Time"},
    {  0x023C, (char *) "TimeCreated", (char *) "Time Created"},
    {  0x0241, (char *) "OriginatingProgram", (char *) "Originating Program"},
    {  0x0000, (char *) NULL, (char *) NULL}
  };

// --------------------------------------------------------------------------
// GeoTIFF tags definition
// --------------------------------------------------------------------------

static TagInfo
  geotiff_tag_table[] =
  {
    {  0x830E, (char *) "GeoPixelScale", (char *) NULL},
    {  0x8480, (char *) "Intergraph TransformationMatrix", (char *) NULL},
    {  0x8482, (char *) "GeoTiePoints", (char *) NULL},
    {  0x85D7, (char *) "JPL Carto IFD offset", (char *) NULL},
    {  0x85D8, (char *) "GeoTransformationMatrix", (char *) NULL},
    {  0x87AF, (char *) "GeoKeyDirectory", (char *) NULL},
    {  0x87B0, (char *) "GeoDoubleParams", (char *) NULL},
    {  0x87B1, (char *) "GeoASCIIParams", (char *) NULL},
    {  0x0000, (char *) NULL, (char *) NULL}
  };

// --------------------------------------------------------------------------
// TagLib class definition
// --------------------------------------------------------------------------


/**
This is where the tag info tables are initialized
*/
TagLib::TagLib() {
	// initialize all known metadata models
	// ====================================

	// Exif
	addMetadataModel(TagLib::EXIF_MAIN, exif_exif_tag_table);
	addMetadataModel(TagLib::EXIF_EXIF, exif_exif_tag_table);
	addMetadataModel(TagLib::EXIF_GPS, exif_gps_tag_table);
	addMetadataModel(TagLib::EXIF_INTEROP, exif_interop_tag_table);

	// Exif maker note
	addMetadataModel(TagLib::EXIF_MAKERNOTE_CANON, exif_canon_tag_table);
	addMetadataModel(TagLib::EXIF_MAKERNOTE_CASIO, exif_casio_tag_table);
	addMetadataModel(TagLib::EXIF_MAKERNOTE_FUJIFILM, exif_fujifilm_tag_table);
	addMetadataModel(TagLib::EXIF_MAKERNOTE_KYOCERA, exif_kyocera_tag_table);
	addMetadataModel(TagLib::EXIF_MAKERNOTE_MINOLTA, exif_minolta_tag_table);
	addMetadataModel(TagLib::EXIF_MAKERNOTE_NIKONTYPE1, exif_nikon_type1_tag_table);
	addMetadataModel(TagLib::EXIF_MAKERNOTE_NIKONTYPE2, exif_nikon_type2_tag_table);
	addMetadataModel(TagLib::EXIF_MAKERNOTE_NIKONTYPE3, exif_nikon_type3_tag_table);
	addMetadataModel(TagLib::EXIF_MAKERNOTE_OLYMPUS, exif_olympus_tag_table);
	addMetadataModel(TagLib::EXIF_MAKERNOTE_PANASONIC, exif_panasonic_tag_table);
	addMetadataModel(TagLib::EXIF_MAKERNOTE_PENTAX, exif_pentax_tag_table);

	// IPTC/NAA
	addMetadataModel(TagLib::IPTC, iptc_tag_table);

	// GeoTIFF
	addMetadataModel(TagLib::GEOTIFF, geotiff_tag_table);
}

BOOL TagLib::addMetadataModel(MDMODEL md_model, TagInfo *tag_table) {
	// check that the model doesn't already exist
	TAGINFO *info_map = (TAGINFO*)_table_map[md_model];

	if((info_map == NULL) && (tag_table != NULL)) {
		// add the tag description table
		TAGINFO *info_map = new TAGINFO();
		for(int i = 0; ; i++) {
			if((tag_table[i].tag == 0) && (tag_table[i].fieldname == NULL))
				break;
			(*info_map)[tag_table[i].tag] = &tag_table[i];
		}

		// add the metadata model
		_table_map[md_model] = info_map;

		return TRUE;
	}

	return FALSE;
}

TagLib::~TagLib() {
	// delete metadata models
	for(TABLEMAP::iterator i = _table_map.begin(); i != _table_map.end(); i++) {
		TAGINFO *info_map = (*i).second;
		delete info_map;
	}
}


TagLib& 
TagLib::instance() {
	static TagLib s;
	return s;
}

const TagInfo* 
TagLib::getTagInfo(MDMODEL md_model, WORD tagID) {
	TAGINFO *info_map = (TAGINFO*)_table_map[md_model];

	if(info_map != NULL) {
		return (*info_map)[tagID];
	}

	return NULL;
}


const char* 
TagLib::getTagFieldName(MDMODEL md_model, WORD tagID) {
	static char unknown_tag[11];

	const TagInfo *info = getTagInfo(md_model, tagID);
	if(NULL == info) {
		sprintf(unknown_tag, "Tag 0x%04X", tagID);
		return &unknown_tag[0];
	}

	return info->fieldname;
}

const char* 
TagLib::getTagDescription(MDMODEL md_model, WORD tagID) {

	const TagInfo *info = getTagInfo(md_model, tagID);
	if(info) {
		return info->description;
	}

	return NULL;
}

FREE_IMAGE_MDMODEL 
TagLib::getFreeImageModel(MDMODEL model) {
	switch(model) {
		case EXIF_MAIN:
			return FIMD_EXIF_MAIN;

		case EXIF_EXIF:
			return FIMD_EXIF_EXIF;

		case EXIF_GPS: 
			return FIMD_EXIF_GPS;

		case EXIF_INTEROP:
			return FIMD_EXIF_INTEROP;

		case EXIF_MAKERNOTE_CANON:
		case EXIF_MAKERNOTE_CASIO:
		case EXIF_MAKERNOTE_FUJIFILM:
		case EXIF_MAKERNOTE_KYOCERA:
		case EXIF_MAKERNOTE_MINOLTA:
		case EXIF_MAKERNOTE_NIKONTYPE1:
		case EXIF_MAKERNOTE_NIKONTYPE2:
		case EXIF_MAKERNOTE_NIKONTYPE3:
		case EXIF_MAKERNOTE_OLYMPUS:
		case EXIF_MAKERNOTE_PANASONIC:
		case EXIF_MAKERNOTE_PENTAX:
			return FIMD_EXIF_MAKERNOTE;

		case IPTC:
			return FIMD_IPTC;

		case GEOTIFF:
			return FIMD_GEOTIFF;
	}

	return FIMD_NODATA;
}

