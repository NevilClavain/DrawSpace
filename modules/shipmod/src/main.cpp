#include "shiproot.h"

extern "C"
{
__declspec(dllexport) DrawSpace::Interface::Module::Root* ModFactory( void )
{
    return new ShipRoot;
}
}