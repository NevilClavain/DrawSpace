
#ifndef _DXCONFAPP_H_
#define _DXCONFAPP_H_

#include <wx/wx.h>

class ConfApp : public wxApp
{
public:
	ConfApp( void );
	virtual ~ConfApp( void );
	virtual bool OnInit( void );
};

DECLARE_APP( ConfApp )




#endif