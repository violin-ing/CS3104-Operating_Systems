// THIS FILE BELONGS TO: user/ls/src/main.cpp

#include <stacsos/console.h>
#include <stacsos/memops.h>
#include <stacsos/objects.h>
#include <stacsos/dirent.h>


using namespace stacsos;

int main(const char *cmdline) {
	bool long_listing = false;
	bool human_readable = false;
	const char *path = nullptr;

	// Parse arguments
	if (!cmdline || memops::strlen(cmdline) == 0) {
		console::get().write("error: usage: /usr/ls [-lh] <directory>\n");
		return 1;
	}

	while (*cmdline) {
		if (*cmdline == ' ') {
			cmdline++;
		
		} else if (*cmdline == '-') { // Deal with potential flag
			cmdline++;
			
			if (*cmdline == 'l') {
				long_listing = true;
			} else if (*cmdline == 'h') {
				human_readable = true;
			} else {
				console::get().write("error: usage: /usr/ls [-lh] <directory>\n");
				return 1;
			}
			cmdline++;
		
		} else {
			path = cmdline;
			break;
		}
	}

	if (path == nullptr || memops::strlen(path) == 0) {
		console::get().write("error: usage: /usr/ls [-l] <directory>\n");
		return 1;
	}

	object *dir = object::open(path);
	if (!dir) {
		console::get().writef("error: unable to open directory '%s' for reading\n", path);
		return 1;
	}

	stacsos::dirent entry;
	size_t bytes_read;
	size_t offset = 0;

    // Main loop to read directory listing
    while ((bytes_read = dir->pread(&entry, sizeof(entry), offset)) > 0) {

        if (bytes_read < sizeof(entry)) {
            break;
        }
		
		if (long_listing) {
			if (entry.kind == 1) { // 1 = Directory
				console::get().writef("[D] %s\n", entry.name);
			} else { // 0 = File
				if (human_readable) {
					console::get().writef("[F] %s   ", entry.name);

					// Convert bytes to KB, MB, GB (anything more is highly unlikely)
					// If single digit, include 1 d.p. (else round to nearest whole number)
					u64 file_size = entry.size;
					
					if (file_size >= 1073741824) { // Larger than 1 GB
						float file_size_exact = (float) file_size / 1073741824.0f; {
							if (file_size_exact < 10.0) {
								console::get().writef("%0.1fG\n", file_size_exact);
							} else {
								console::get().writef("%0.0fG\n", file_size_exact);
							}
						}
					} else if (file_size >= 1048576) { // Larger than 1 MB
						float file_size_exact = (float) file_size / 1048576.0f; {
							if (file_size_exact < 10.0) {
								console::get().writef("%0.1fM\n", file_size_exact);
							} else {
								console::get().writef("%0.0fM\n", file_size_exact);
							}
						}
					} else if (file_size >= 1024) { // Larger than 1 KB
						float file_size_exact = (float) file_size / 1024.0f; {
							if (file_size_exact < 10.0) {
								console::get().writef("%0.1fK\n", file_size_exact);
							} else {
								console::get().writef("%0.0fK\n", file_size_exact);
							}
						}
					} else {
						console::get().writef("%luB\n", file_size);
					}
				} else {
					console::get().writef("[F] %s   ", entry.name);
					console::get().writef("%lu\n", entry.size);
				}
			}
		} else {
			console::get().writef("%s\n", entry.name);
		}

		// Advance the offset to ask for the next entry
		offset += sizeof(entry);
	}

	// Cleanup
    delete dir;
    return 0;
}
