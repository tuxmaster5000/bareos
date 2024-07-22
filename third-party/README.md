# Bareos third party libraries

This is the "home" of external libraries that are used in Bareos

### Adding third party libraries

Each Library will be put into its own subdirectory with `git subtree` and will have its own CMake include-file (if it is not already made available by the provider of the library), that is then loaded in CMakeLists.txt

A typical git subtree command would look like this : `git subtree add --prefix destination/path https://www.yourgithost.com/your/repo yourbranch (usually master) --squash`

Make sure the library license is covered by `LICENSE.txt`. Preferably the library is also licensed as AGPL-3. If it is not already covered, a section needs to be added for it in `devtools/template/LICENSE.txt`.
Executing `pr-tool update-license` will renew the `LICENSE.txt` file.
This will also happen automatically when merging the PR.

### What to add?

External things that we rely on (but don't want to change) should live here. Things like `FMT` and `GSL` could be added here. 
`lmdb` (and maybe the `md5` code) should be migrated here.

Things like `droplet`, `fastlz` and `ndmp` will stay as is, because there is no upstream and we made significant changes (i.e. we imported that code into our code-base instead of just using it).
