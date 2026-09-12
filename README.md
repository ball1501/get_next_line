*This project has been created as part of the 42 curriculum by wngamkri.*

# get_next_line

## Description

`get_next_line` is a project from the 42 curriculum focused on reading and processing data from file descriptors.

The goal of this project is to create a function that reads a file descriptor and returns **one line at a time**. Each call to `get_next_line()` returns the next line from the file, including the newline character (`\n`) when one is present.

The function must continue reading from where the previous call stopped, allowing a file to be processed line by line without loading the entire file into memory at once.

The project also introduces the use of **static variables**, which are used to preserve data between successive function calls.

### Function Prototype

```c
char	*get_next_line(int fd);
```

### Return Value

* Returns the next line read from the file descriptor.
* The returned line includes `\n` if the line ends with a newline character.
* Returns `NULL` when there is nothing left to read or when an error occurs.

---

## Instructions

### Compilation

Clone the repository and enter the project directory:

```bash
git clone <repository-url>
cd get_next_line
```

Compile the project together with your test program:

```bash
cc -Wall -Wextra -Werror main.c get_next_line.c get_next_line_utils.c -o gnl
```

If using the bonus version:

```bash
cc -Wall -Wextra -Werror main.c get_next_line_bonus.c get_next_line_utils_bonus.c -o gnl
```

The `BUFFER_SIZE` can be changed during compilation:

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 main.c get_next_line.c get_next_line_utils.c -o gnl
```

### Execution

The function can be tested by opening a file and repeatedly calling `get_next_line()`:

```c
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int	main(void)
{
	int		fd;
	char	*line;

	fd = open("test.txt", O_RDONLY);
	if (fd < 0)
		return (1);

	while ((line = get_next_line(fd)) != NULL)
	{
		printf("%s", line);
		free(line);
	}

	close(fd);
	return (0);
}
```

Compile and run:

```bash
./gnl
```

---

## Algorithm

The main algorithm of `get_next_line()` is based on **incremental reading with a persistent buffer**.

Instead of reading the entire file at once, the function reads the file in chunks determined by `BUFFER_SIZE`.

The general process is:

```text
File Descriptor
      |
      v
   read()
      |
      v
  Temporary Buffer
      |
      v
  Store / Append
      |
      v
 Is '\n' found?
    /       \
  No         Yes
  |           |
  v           v
read()     Extract line
more data      |
  |            v
  +-------> Save remaining data
               |
               v
          Return the line
```

### Step-by-step

1. `get_next_line()` checks the stored data from previous calls.
2. If the stored data does not contain a newline, `read()` is called to obtain another chunk of data.
3. The newly read data is appended to the stored data.
4. This process continues until:

   * A newline character is found, or
   * End-of-file is reached.
5. Once a newline is found, the function extracts everything up to and including the newline.
6. Any characters after that newline are preserved for the next call.
7. The extracted line is returned to the caller.
8. The remaining data is kept in a static variable so it survives after the function returns.

This allows repeated calls to continue from the correct position in the file.

---

## Why This Algorithm Was Chosen

The incremental reading approach is appropriate for `get_next_line()` because the function is specifically designed to return **one line per call**.

Reading the entire file into memory would be unnecessary and could consume a large amount of memory when processing large files.

By reading only `BUFFER_SIZE` bytes at a time:

* Memory usage is reduced.
* Large files can be processed without loading everything into memory.
* The function can stop reading as soon as a complete line is available.
* Data that belongs to the next line can be preserved for the next function call.

The use of a static variable is particularly important because local variables are destroyed when the function returns. A static variable allows the unread portion of the data to remain available between calls.

---

## Buffer Management

The temporary buffer is allocated according to `BUFFER_SIZE`.

For example, with:

```text
BUFFER_SIZE = 10
```

a file such as:

```text
Hello World!
This is 42.
```

may be read in several chunks:

```text
"Hello Worl"
"d!\nThis is"
" 42.\n"
```

The function combines these chunks until it finds a newline.

After returning:

```text
"Hello World!\n"
```

the remaining data:

```text
"This is"
```

is kept for the next call.

The process continues until the entire file has been consumed.

---

## Static Variable

A static variable is used to preserve the unread portion of the file between calls.

Conceptually:

```text
First call
    |
    v
read data
    |
    +---- line returned
    |
    +---- remaining data stored
                         |
                         v
Second call <-------------+
    |
    v
use stored data
    |
    v
read more if necessary
```

This is the key mechanism that allows `get_next_line()` to remember its position without requiring the caller to manually manage the reading state.

---

## Algorithm Complexity

The exact complexity depends on the implementation and how strings are concatenated.

In the typical implementation, data may need to be copied when buffers are joined or when a line is extracted.

For a line of length `n`, the total processing can therefore reach **O(n)** for the line itself, while repeated string concatenation can introduce additional copying overhead depending on the implementation.

The additional memory required is proportional to the amount of unread data that must be stored.

The algorithm does not require the entire file to be loaded into memory at once.

---

## Data Structures

The project does not require complex data structures.

The main structures used are:

### Static Character Buffer

A static `char *` is used to store data that has been read but not yet returned.

It allows data to persist between calls to `get_next_line()`.

### Temporary Buffer

A dynamically allocated character array of size `BUFFER_SIZE + 1` is used to store data returned by `read()`.

The additional byte is used for the null terminator.

### Returned String

A dynamically allocated string contains the line returned to the caller.

The caller is responsible for freeing this memory.

### Bonus: Multiple File Descriptors

In the bonus implementation, the function must be able to handle multiple file descriptors simultaneously.

Instead of maintaining only one static buffer, the implementation maintains separate persistent data for each file descriptor.

Conceptually:

```text
fd 3  ---> stored data for fd 3
fd 4  ---> stored data for fd 4
fd 5  ---> stored data for fd 5
```

This prevents data from one file descriptor from interfering with another.

---

## Technical Choices

### `read()`

The project uses the Unix `read()` system call to retrieve data from the file descriptor.

The amount of data read in each operation is controlled by `BUFFER_SIZE`.

### Dynamic Memory Allocation

Dynamic memory allocation is necessary because the length of a line is not known in advance.

The implementation allocates enough memory for the returned line and releases temporary memory when it is no longer needed.

### Static Storage

Static storage is used to preserve unread data between function calls.

This provides the state required for a line-by-line reader without requiring additional state to be passed through the function's parameters.

---

## Edge Cases

The implementation should correctly handle:

* Empty files
* Files containing only one line
* Files ending with `\n`
* Files without a final `\n`
* Multiple consecutive newlines
* Empty lines
* Very long lines
* `BUFFER_SIZE = 1`
* Large `BUFFER_SIZE` values
* Invalid file descriptors
* Reading errors
* Repeated calls after reaching EOF

For the bonus version, multiple file descriptors should also be handled independently.

---

## Resources

### Documentation

* `read(2)` — documentation for the Unix `read()` system call.
* `open(2)` — documentation for opening files and obtaining file descriptors.
* `close(2)` — documentation for closing file descriptors.
* `malloc(3)` — documentation for dynamic memory allocation.
* `free(3)` — documentation for releasing allocated memory.

Useful manual pages:

```bash
man 2 read
man 2 open
man 2 close
man 3 malloc
man 3 free
```

### Learning Resources

The following resources were used to understand the concepts required for this project:

* The 42 `get_next_line` subject
* Unix/Linux system call documentation
* C documentation for dynamic memory allocation
* C documentation for static variables
* Tutorials and references about file descriptors
* Tutorials about string manipulation and memory management in C

### AI Usage

AI tools were used as a learning and debugging aid during the development of this project.

They were used for tasks such as:

* Explaining C concepts that were unclear during implementation.
* Helping understand file descriptors and the `read()` system call.
* Explaining how static variables can preserve data between function calls.
* Analyzing compiler errors and tester failures.
* Reviewing implementation logic and identifying potential edge cases.
* Explaining memory management and possible memory leaks.
* Suggesting ways to simplify or improve code while keeping the implementation understandable.

The project code was implemented and reviewed by the student. AI was used as a supplementary learning and debugging tool rather than as a replacement for understanding or implementing the project.

---

## Project Structure

A typical project structure is:

```text
get_next_line/
├── Makefile
├── get_next_line.h
├── get_next_line.c
├── get_next_line_utils.c
├── get_next_line_bonus.h
├── get_next_line_bonus.c
├── get_next_line_utils_bonus.c
└── README.md
```

The exact file structure may vary depending on the implementation.

---

## Author

**wngamkri**

42 Student
