#include "test02root.h"

extern "C"
{
__declspec(dllexport) DrawSpace::Interface::Module::Root* PIFactory( void )
{
    return new Test02Root;
}
}