#include "cdlodplanetroot.h"

extern "C"
{
__declspec(dllexport) DrawSpace::Interface::Module::Root* PIFactory( void )
{
    return new CDLODPlanetRoot;
}
}