1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  `fgets()` is a good choice for this application because of the following reasons:<ol>
        <li> We can specify exactly how many characters we want to read which helps us avoid buffer overflows.</li>
        <li> Preserves newline character and it lets us read user commands line by line.</li>
        <li> Null terminates the input string. </li>
        <li> Detects EOF which helps with testing. </li </ol>

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  We could've used a fixed-size array instead of using malloc() since we know the max size of the input we want to take. However, malloc lets us allocate exactly as much memory as we need, and they are easy to resize as needed without repeated copies. This can allow for dynamically allocated memories and can also let us customize our shell configurations to allow for larger buffers if needed. However, the most important reason is that fixed-size array memory is allocated on the stack which has very limited size, and if the command exceeds that, then there's a chance of stack-overflow. But malloc allocates memory on the heap which is much larger. 


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  If we didn't trim spaces, then the spaces might've been included in the executeable name or in the arguments. For example: If we wanted to run the 'dragon' command, the actual command that would've been stored could've been ' dragon' with a leading whitespace. In such a case, it wouldn't map to any executeable name and thus the command would've failed. Same goes for the arguments that're expected with commands that require them. 

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  We can implement the following redirections in our shell: <ol>
        <li> > , >> (re-direct output)</li>
        <li> < (redirect input) </li>
        <li> >& (duplicate output) </li> </ol> Some of the challenges we will face when implementing them are: file handling permissions, overwrite a file or append a file, complexity with multiple redirections and/or pipes. 
    
- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Key differences between the two are that redirection works with files or file descriptors whereas pipes work with processes, and redirected file contents persist whereas piped content disappears. 

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  It is important to keep them separate because they help us capture errors separately which helps with debugging and maintenance. They also help with testing by keeping error data separate from expected data. 

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Our custom shell should keep them separated by default, so that we can avoiding mixing errors with expected data. However, we should allow merging them together with a redirection (2>&1).