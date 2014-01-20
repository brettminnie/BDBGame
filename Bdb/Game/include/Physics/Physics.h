// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the BDBPHYSICS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// BDBPHYSICS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef BDBPHYSICS_EXPORTS
#define BDBPHYSICS_API __declspec(dllexport)
#else
#define BDBPHYSICS_API __declspec(dllimport)
#endif

// This class is exported from the bdb.Physics.dll
class BDBPHYSICS_API CbdbPhysics {
public:
	CbdbPhysics(void);
	// TODO: add your methods here.
};

extern BDBPHYSICS_API int nbdbPhysics;

BDBPHYSICS_API int fnbdbPhysics(void);
