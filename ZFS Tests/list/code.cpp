#include <libzfs.h>
#include <errno.h>
#include <cstdio>
#include <cstdlib>

using namespace std;
void cleanup(libzfs_handle_t* was) {
	libzfs_fini(was);
	printf("Aufgeräumt\n");
}

int main() {
	libzfs_handle_t* libzfs_hadle = libzfs_init();
	if(libzfs_hadle)
		printf("ZFS geladen\n");
	else {
		printf("ZFS konnte nich geladen werden.\n%s\n", libzfs_error_init(errno));
		exit(1);
	}
	cleanup(libzfs_hadle);
}
