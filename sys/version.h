#ifndef VERSION_H
#define VERSION_H

// Little Dawn Version Information
#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 2

// Version string
#define VERSION_STRING "1.0.2"

// Full version string with name
#define VERSION_FULL "Little Dawn v" VERSION_STRING

// Build date (will be updated by build system if needed)
#ifndef BUILD_DATE
#define BUILD_DATE __DATE__ " " __TIME__
#endif

#endif // VERSION_H