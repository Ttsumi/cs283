1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

After we fork, we store the PID's of all the child processes. This allows us to call waitpid() on each child process. It is a system call that is used to wait for state changes in a child process. This ensures that we wait for a child process to terminate before moving on. If we forgot to call waitpid() on all child processes, then that child process becomes a "zombie". As long as it is not terminated, it will consume a slot in the kernels process table, and if this table fills out, then it will not be able to create more processes. If the program/parent process terminates, then the "zombie" children will be adopted by the "init" process.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

It is necessary to close unused pipe ends after calling dup2() otherwise it could lead to memory leaks. There's a limited number of slots on the file descriptor table, and in a large scale program, if we are constantly opening but not closing these, it could take up all the space and no further ones could be opened.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

cd is implemented as a built-in command rather than an external command because, this command is targeted for the current process. If we were to make it an external command, then we'd only change directory on the child process, but not the parent process. Therefore, in the shell, it would seem like the command did not work.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To allow an arbitrary number of piped commands while still handling memory allocation efficiently, we have to dynamically allocate and reallocate memory for the command_list and command_buff everytime. This would increase the complexity of memory management and can potentially slow down the program if we have to resize frequently.
