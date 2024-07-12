.. _BackupZFS:

Backup Of ZFS
-------------------------------

All list options are aomma seperated.
To see the  debug messages, set the debug level to an minimum of 150.
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
