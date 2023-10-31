# filewriter
Stdin is being appended to a file. When the output file disappears (or changes inode), the file descriptor will be closed and reopened (possibly created) and appending will happen to the new file.
