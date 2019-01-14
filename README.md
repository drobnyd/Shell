# A Simple Shell Implementation for Linux/Unix programming in C course at Charles University in Prague

Phase 1 requirements (due to January 13, 2019)
-------
Note - the "mysh$" prompt in examples below represents the behavior of
the shell you implement.  The "Bash$" prompt represents the system shell
you start your shell from.

- Honor and use the existing PATH you get from your parent shell, i.e.
  so that one can use "ls" and the command will execute.

- ^C (= SIGINT) in the prompt kills the current unfinished line and
  offers a new prompt (same way as bash does it).  E.g:

	mysh$ ls xxx^C
	mysh$

- An empty command just prints a new prompt (as bash does it).  E.g.

	mysh$ <SPACE><SPACE><SPACE><ENTER>
	mysh$

- Implement simple foreground command execution.  Commands may be
  separated with a semicolon.  Any number of commands separated by a
  semicolon must be supported (i.e. until you reach some other system
  limits):

	- i.e. "cmd [; cmd2 [; cmd3 ...]]]
	- for example:

	mysh$ date
	Mon Oct  8 04:51:27 PDT 2018
	mysh$

	mysh$ /usr/bin/echo X
	X
	mysh$

	mysh$ echo X; date; echo Z
	X
	Monday, October  8, 2018 at 11:36:04 AM CEST
	Z
	mysh$

  Whitespace does not matter with the semicolon, i.e.:

	mysh$ echo X;date;echo Z
	X
	Monday, October  8, 2018 at 11:36:04 AM CEST
	Z
	mysh$

- A semicolon after the last command is legal (as is in bash).  E.g:

	mysh$ echo x;
	x

  However, the following is not legal:

	mysh$ date;;
	error:1: syntax error near unexpected token ';'
	mysh$

- Implement an internal "exit" to exit the shell.  E.g.:

	Bash$ ./mysh
	mysh$ exit
	Bash$ echo $?
	0
	Bash$

  The internal exit command does not change the shell's return value set
  from a previous command execution.  See below for more information.

- ^D (i.e. Ctrl-D) exits the interactive shell as well (same as the
  "exit" internal command).  Again, see bash for an example.

- Implement an internal "cd" command.  It works in three modes:

	- "cd" goes to $HOME
	- "cd -" goes to previous directory
	- "cd <dir>" goes to "<dir>"

  The internal cd must set the PWD and OLDPWD environment variables
  (again see bash).

  E.g.:

	mysh$ pwd
	/data/maish
	mysh$ cd
	mysh$ pwd
	/home/jpechane
	mysh$ cd -
	mysh$ pwd
	/data/maish
	mysh$ cd /tmp
	mysh$ pwd
	/tmp
	mysh$ cd -
	mysh$ pwd
	/data/maish

- The "-c" option is supported as in any other usual shell.

  E.g.:

	Bash$ mysh -c "echo XXX; date"
	XXX
	Monday, October  8, 2018 at  1:36:56 PM CEST
	Bash$

- The '#" comment is supported. E.g.:

	mysh$ echo X # comment
	X
	mysh$     # another comment
	mysh$

	Or:

	./mysh -c 'date; echo X # comment'
	Wednesday, October 10, 2018 at 10:10:09 AM CEST
	X

	Comments must also work in the non-interactive mode, see below.

- The non-interactive mode is supported.

	- i.e. 'mysh test.sh' must work and the commands from the file
	  are executed sequentially.

	- the shell errors out on first syntax error as a usual shell
	  does.  E.g:

	Bash$ cat test.sh 
	echo X
	date
	;;
	echo Y

	Bash$ ./mysh test.sh
	X
	Monday, October  8, 2018 at 11:38:16 AM CEST
	error:3: syntax error near unexpected token ';'
	Bash$

	- shell must treat syntax errors gracefully.  Print the line
	  number where the problem occured.  See the error message
	  above.

- In all modes, the return value of your shell is the return value of
  the last command (or pipeline) executed or some specific error on a
  syntax error (e.g. bash returns 2, ksh returns 3).  E.g. if we use
  return value of 254 for a syntax error, it will look like this:

	Bash$ ./mysh -c "grep XXX123 /etc/passwd"
	Bash$ echo $?
	1

	Bash$ ./mysh -c ';;'
	error:1: syntax error near unexpected token ';'
	Bash$ echo $?
	254

	mysh$ grep XXX123 /etc/passwd
	mysh$ exit
	Bash$ echo $?
	1

	mysh$ grep XXX123 /etc/passwd
	mysh$ ^D
	Bash$ echo $?
	1

	mysh$ ls; ;
	error:1: syntax error near unexpected token ';'
	mysh$ exit
	Bash$ echo $?
	254

- The return value of an unknown command is 127 (as in bash).  E.g:

	Bash$ ./mysh -c 'xxx123'
	mysh: xxx123: No such file or directory
	Bash$ echo $?
	127

- Error and warning messages from the shell itself go to standard error
  output.  E.g. the message "mysh: xxx123: No such file or directory"
  from above goes to stderr.

- The interactive shell must handle SIGINT well (= killing the
  foreground process via ^C).  I.e.:

	mysh$ sleep 100
	^CKilled by signal 2.
	mysh$ 

  If a command exits on receiving a signal and it is the last command
  run by the shell, the shell return value is 128 plus the signal number
  (same as in bash).  E.g.:
  
	Bash$ ./mysh
	mysh$ sleep 10
	^CKilled by signal 2.
	mysh$ exit
	Bash$ echo $?
	130

  However, see also the paragraph about killing the unfinished command
  line with ^C.  Both must work as explained.

  The "Killed by..." message goes to stderr.  Also see above on error
  and warning messages.

- It would be nice if you put the current working directory in the
  prompt, like this:

	mysh:/data/mysh$ cd /
	mysh:/$ cd /tmp
	mysh:/tmp$
