//---------------------------------------------------------------------------
#ifndef StringUtilitiesH
#define StringUtilitiesH

#include <string>

std::string UnderscoresToBlanks( std::string Source );
// Ritorna la stringa che si ottiene da Source sostituendo tutti
// gli underscore presenti con spazi bianchi.
// N underscore consecutivi verranno sostituiti con N spazi bianchi
// consecutivi.

std::string BlanksToUnderscores( std::string Source );
// Ritorna la stringa che si ottiene da Source sostituendo tutti
// gli spazi bianchi presenti con underscore.
// N spazi bianchi consecutivi verranno sostituiti con N underscore
// consecutivi.

//---------------------------------------------------------------------------
#endif
