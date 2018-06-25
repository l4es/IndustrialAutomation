3D SCANNING SOFTWARE

This software implements the calibration method described in "Simple, Accurate, and Robust 
Projector-Camera Calibration" by Daniel Moreno and Gabriel Taubin in 3DimPVT 2012(*). 

It extends the original software by including projection and capture of gray code patterns, 
and a simple triangulation feauture to generate oriented color pointclouds.

(*) http://dx.doi.org/10.1109/3DIMPVT.2012.77


--- LICENSE ---

If you download the software we will assume that you have read and agreed to the terms of
the Licence Agreement


--- COMPILATION ---

See INSTALL.txt


--- SAMPLE DATA SET ---

For testing purposes download the sample data from the project website


--- DISABLE CAMERA AUTO-PARAMETERS

It is required that the camera in use has fixed parameters. All the auto-settings (auto-focus,
auto-exposure, auto-gain, auto-white-balance) must be disabled. How to adjust these settings
depends on the camera currently in use and it has to be done using either by using 
camera manufacturer software or other external tools.

In Microsoft Windows consider installing camera manufacturer specific drivers instead 
of Windows generic ones.

In Mac OS X consider using 'UVC Camera Control':
 http://phoboslab.org/log/2009/07/uvc-camera-control-for-mac-os-x

