#ifndef GUARDS_H
#define GUARDS_H

#include <unistd.h>

int is_root_user () {
	if ((getuid() == 0) || (geteuid() == 0)) {
		return 1;
	}

	return 0;
}

#endif /* GUARDS_H */
