//---------------------------------------------------------------------------
#include "StringUtilities.h"

std::string UnderscoresToBlanks( std::string Source )
{
 unsigned int i;
 std::string temp = Source;

 for(i = 0; i < Source.size(); i++)
    temp[i] = (Source[i] == '_') ? ' ' : Source[i];

 return temp;
}

std::string BlanksToUnderscores( std::string Source )
{
 unsigned int i;
 std::string temp = Source;

 for(i = 0; i <= Source.size(); i++)
    temp[i] = (Source[i] == ' ') ? '_' : Source[i];

 return temp;
}
//---------------------------------------------------------------------------
