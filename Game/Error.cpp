#include "Error.h"
#include "RessourceFile.h"

namespace UL {

	ThrowableEvent &ThrowableEvent::operator<<(const RessourceFile & f) {
		*this << f.getFilename();
		return *this;
	}
	
}