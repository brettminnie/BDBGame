// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GRAPHICS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GRAPHICS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GRAPHICS_EXPORTS
#define BDBGRAPHICS __declspec(dllexport)
#else
#define BDBGRAPHICS __declspec(dllimport)
#endif

/*// This class is exported from the Graphics.dll
class BDBGRAPHICS CGraphics {
public:
	CGraphics(void);
	// TODO: add your methods here.
};

extern BDBGRAPHICS int nGraphics;

BDBGRAPHICS int fnGraphics(void);*/
