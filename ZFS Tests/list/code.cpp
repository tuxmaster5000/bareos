#include <libzfs.h>
#include <errno.h>
#include <cstdio>
#include <cstdlib>

using namespace std;

typedef struct {
	uint* pool_counter;
	libzfs_handle_t* libzfs;
} data_struct;
void cleanup(data_struct* was) {
	libzfs_fini(was->libzfs);
	printf("Aufgeräumt\n");
}
int pool_info(zpool_handle_t* pool, void* data) {
	const char *p_name;
	data_struct* my_data= static_cast<data_struct*>(data); 
	p_name = zpool_get_name(pool);
	zfs_handle_t* my_pool=zfs_open(my_data->libzfs, p_name,ZFS_TYPE_FILESYSTEM | ZFS_TYPE_VOLUME);
	if(pool) {
		printf("%s:\n", p_name);
		++*my_data->pool_counter;
		zfs_close(my_pool);
	} else {
		perror(p_name);
		return 1;
	}
	return 0;
}
bool pool_list(data_struct* data) {
	printf("Suche alle Pools\n");
	uint* pool_count = new uint(0);
	data->pool_counter = pool_count;
	if(zpool_iter(data->libzfs, pool_info, data) != 0)
	{
		printf("Konnte die Pools nicht auswerten.\n%s\n", libzfs_error_description(data->libzfs));
		return false;
	}
	printf("%i Pool(s) gefunden.\n", *pool_count);
	return true;
}
int main() {
	libzfs_handle_t* libzfs_hadle = libzfs_init();
	if(libzfs_hadle)
		printf("ZFS geladen\n");
	else {
		printf("ZFS konnte nich geladen werden.\n%s\n", libzfs_error_init(errno));
		exit(1);
	}
	data_struct data;
	data.libzfs = libzfs_hadle;
	pool_list(&data);
	cleanup(&data);
}
