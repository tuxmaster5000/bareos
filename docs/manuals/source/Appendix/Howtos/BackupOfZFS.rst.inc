.. _BackupZFS:

Backup of ZFS
-------------------------------
By default all datasets and volumes of all tanks are seleted for backup.
By nature incremetal backups can only done for datasets, becauce ZFS don't know anything about the content of the volumes.
So the volumes are allways full backups. This means, every incremental contains an full backup of the ZFS volumes.
Backing up the data sets are an three step process. First take an snapshot, then backup it and remove it(When it will not be needes anymore.).
This will garant an stable backup for the data sets.

All list options are comma seperated. To see the debug messages, set the debug level to an minimum of 150.

Used options in the plugin string are:

snapshot-prefix
   The prefix for the dataset snapshots. (If empty, default bareos.).

tanks
   List of ZFS tanks to backup or set it to none, set the volumes/datasets seperate. (If empty, default all.).

exclude-datasets
   Datasets to exclude from the backup(If default empty, none will be excluded.).

exclude-volumes
   Volumes to exclude from the backup(If default empty, none will be excluded.).

datasets
   List of datasets to backup, only valid if the tank list are set to none.(Default empty).

volumes
   List of volums to backup, only valid if the tank list are set to none.(Default empty).
