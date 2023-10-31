# filewriter
Stdin is being appended to a file. When the output file disappears (or changes inode), the file descriptor will be closed and reopened (possibly created) and appending will happen to the new file.

Filewriter works with any kind of data (treats input and output as binary octets) and does no buffering, because it uses read(2) and write(2).

This functionality is a bit like a logging functionality, which keeps writing to log files. When you want to work on the accumulated data, you can move e.g. `output.log` to `output.old`, parse/rsync/whatever the `.old` file before removing it.

This was written for being used roughly like this: `tcpdump -nnp | filewriter dump.out`
