=== SETUP ===

1. Copy dirent.h to kernel/inc/stacsos/kernel/fs/dirent.h.
2. Copy dirent.h to user/ulib/inc/stacsos/dirent.h.
3. Replace the fat.h file in stacsos (at kernel/inc/stacsos/kernel/fs/fat.h).
4. Replace the fat.cpp file in stacsos (at kernel/src/fs/fat.cpp).
5. Copy the src/ directory to user/ls in stacsos (create a new 'ls' directory if it doesn't exist yet).
6. Add 'ls' to 'apps' in user/Makefile:
	apps := init shell sched-test mandelbrot cat poweroff sched-test2 cls ls

Then run 'make run' in stacsos.


=== USAGE ===

Possible ways of running my 'ls' implementation:
1. /usr/ls /path/to/directory
2. /usr/ls -l /path/to/directory
3. /usr/ls -lh /path/to/directory

Running "/usr/ls -h /path/to/directory" will give the same output as running 'ls' with no flags.

Note that we can also swap '-h' and '-l' to run:
	/usr/ls -hl /path/to/directory
It should give the same output as using '-lh'.
