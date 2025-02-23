1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

   > **Answer**: If we call execvp directly, then our main program will shape-shift into the program/command that we try to execute through execvp. Our program and all the necessities that were loaded into virtual memory for our program will get overwritten because the Linux will load the necessities to run that program/command into the virtual memory. However, by using fork, we make an exact copy of our current process and give the task of running our desired program or command to that process.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

   > **Answer**: If the fork() system call fails, my program uses perror() to print out a helpful message and does not execute the external command or program. It sets the return code accordingly as well and breaks the loop before freeing up memory and exiting the program. I have considered continuing the loop and keep the program running, but I don't know under what scenario fork would fail and ended up deciding that it'd be safer to just exit the program.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

   > **Answer**: execvp searches for the command in the path of the user. The $PATH environment varable plays the main role in this process.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

   > **Answer**: The purpose of calling wait() in the parent process is to give the child process time to finish executing. If we didn't call it, then the parent process could finish executing faster than the child process, and end before the child process. In that case, the child process would become orphaned and be adopted by the init process.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

   > **Answer**: WEXITSTATUS() contains the exit status of the child process. This information is very important because it can help us determine whether the child process successfully executed the command or program it was given or in case of a failure, it can also get the errorno which also helps us debug.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

   > **Answer**: For quoted arguments, my implementation of build_cmd_buff() tracks whether we are in quotes. If we are, we preserve whatever character was in the input buffer, mainly whitespaces. This is necessary because we want to treat quoted arguments as a string, and whitespaces are part of the whole string.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

   > **Answer**: For my parsing logic, I did not have to make many changes. One of the two main changes had to do with storing the input into a command struct, instead of storing all commands. I also did not account for pipes in this assignment since we are not implementing the pipe handling logic.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

  > **Answer**: Signals are a form of asynchronus communication between processes. They can be used to interrupt, pause or kill processes. They differ from other forms of IPCs like pipes in the sense that Signals are asynchronus whereas a pipe in synchronus.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

  > **Answer**: SIGKILL is used to kill the process immediately. It is used to terminate unresponsive processes.
  > SIGTERM is used to request a process to terminate. It is used to catch and handle a process.
  > SIGINT is used to interrupt a process. It is used when we precc "CTRL + C".

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

  > **Answer**: When a process receives SIGSTOP, it pauses execution. It cannot be caught or ignored like SIGINT. Because that would invalidate the purpose of the signal.
