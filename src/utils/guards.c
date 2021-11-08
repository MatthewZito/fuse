#include "guards.h"

int is_root_user (void) {
	if ((getuid() == 0) || (geteuid() == 0)) {
		return 1;
	}

	return 0;
}
