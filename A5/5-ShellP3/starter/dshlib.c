#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include "dshlib.h"

extern void print_dragon();

int alloc_cmd_buff(cmd_buff_t *cmd_buff){
    cmd_buff->_cmd_buffer = (char *)malloc(SH_CMD_MAX);
    if (!cmd_buff->_cmd_buffer) {
        return ERR_MEMORY;
    }
    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);
    cmd_buff->argc = 0;
    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv));
    return 0;
}

int free_cmd_buff(cmd_buff_t *cmd_buff){
    for (int i = 0; i < cmd_buff->argc; i++) {
        free(cmd_buff->argv[i]);
    }

    free(cmd_buff->_cmd_buffer);
    cmd_buff->_cmd_buffer = NULL;
    cmd_buff->argc = 0;
    return 0;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {

    for (int i = 0; i < cmd_buff->argc; i++) {
        free(cmd_buff->argv[i]);
        cmd_buff->argv[i] = NULL;
    }

    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);
    cmd_buff->argc = 0;

    return 0;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cbuff)
{
    alloc_cmd_buff(cbuff);
    int input_len = strlen(cmd_line);
    int argc = 0;
    bool inWord = false;
    bool inQuotes = false;
    char *track_word = malloc(input_len + 1);
    int inWordLen = 0;

    int j = 0;

    while (cmd_line[j] && isspace(cmd_line[j])){
        j++;
    } 

    for (int i = j; i < input_len; i++) {
        if (cmd_line[i] == '"' || cmd_line[i] == '\'') {
            inQuotes = !inQuotes;
            continue; 
        }
        if (isspace(cmd_line[i]) && !inQuotes) {
            if (inWord) {  
                track_word[inWordLen] = '\0';  
                cbuff->argv[argc] = strdup(track_word);
                argc++;
                inWordLen = 0;
                inWord = false;
                if (argc >= CMD_ARGV_MAX) {
                    break;
                }
            }
        } else {
            track_word[inWordLen++] = cmd_line[i];
            inWord = true;
        }
    }

    if (inWordLen > 0) {
        track_word[inWordLen] = '\0';
        cbuff->argv[argc] = strdup(track_word);
        argc++;
    }

    cbuff->argc = argc;
    free(track_word);

    return 0;
}

int pipe_count(char *cmd_line){
    int i=0;
    int count=0;
    while(cmd_line[i] != '\0'){
        if (cmd_line[i] == PIPE_CHAR){
            count++;
        }
        i++;
    }
    return count;
}

int build_cmd_list(char *cmd_line, command_list_t *clist){
    memset(clist, 0, sizeof(command_list_t));

    if (pipe_count(cmd_line) >= CMD_MAX){
        return ERR_TOO_MANY_COMMANDS;
    }

    const char outer_delimiters[] = PIPE_STRING;
    char* token;
    char* outer_saveptr = NULL;
    int cmd_count = 0;

    token = strtok_r(cmd_line, outer_delimiters, &outer_saveptr);
    while(token != NULL){
        if (cmd_count >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }           
        if (build_cmd_buff(token, &clist->commands[cmd_count]) != 0) {
            return ERR_CMD_ARGS_BAD;
        }

        cmd_count++;
        token = strtok_r(NULL, outer_delimiters, &outer_saveptr);
    }
    clist->num = cmd_count;
    return OK;
}

int free_cmd_list(command_list_t *cmd_lst){
    for (int i = 0; i < cmd_lst->num; i++) {
        free_cmd_buff(&cmd_lst->commands[i]);
    }
    cmd_lst->num = 0;
    return 0;
}

Built_In_Cmds match_command(const char *input){
    if(strcmp(input, "cd") == 0){
        return BI_CMD_CD;
    } else if (strcmp(input, "dragon") == 0){
        return BI_CMD_DRAGON;
    } else if (strcmp(input, "exit") == 0){
        return BI_CMD_EXIT;
    } else if (strcmp(input, "execute") == 0){
        return BI_EXECUTED;
    }
    return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd){
    if (strcmp(cmd->argv[0], "cd") == 0 ){
        if (cmd->argc > 1){
            int cd_success = chdir(cmd->argv[1]);
            if (cd_success != 0){
                perror("cd");
            }
        }
        return BI_CMD_CD;
    } else if (strcmp(cmd->argv[0], "dragon") == 0){
        print_dragon();
        return BI_CMD_DRAGON;
    } else if (strcmp(cmd->argv[0], EXIT_CMD) == 0){
        printf("exiting...\n");
        return BI_CMD_EXIT;
    }
    return BI_CMD_EXIT;
}

int execute_pipeline(command_list_t *commands) {
    int num_commands = commands->num;
    int pipes[num_commands - 1][2];  // Array of pipes
    pid_t pids[num_commands];        // Array to store process IDs

    int rc = 0;

    // Create all necessary pipes
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Create processes for each command
    for (int i = 0; i < num_commands; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) {  // Child process
            // Set up input pipe for all except first process
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }

            // Set up output pipe for all except last process
            if (i < num_commands - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Close all pipe ends in child
            for (int j = 0; j < num_commands - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(commands->commands[i].argv[0], commands->commands[i].argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    // Parent process: close all pipe ends
    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all children
    for (int i = 0; i < num_commands; i++) {
        waitpid(pids[i], NULL, 0);
    }
    return rc;
}


/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{

    char *cmd_buff = malloc(SH_CMD_MAX);
    int rc = 0;
    command_list_t *clist = (command_list_t *)malloc(sizeof(command_list_t));

    while(1){
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
            printf("\n");
            break;
            }
        //remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';

        //IMPLEMENT THE REST OF THE REQUIREMENTS
        rc = build_cmd_list(cmd_buff, clist);

        if (rc != OK){
            free_cmd_list(clist);
            break;
        }

        if (match_command(clist->commands[0].argv[0]) == BI_NOT_BI){
            rc = execute_pipeline(clist);
        } else {
            rc = exec_built_in_cmd(&clist->commands[0]);
            if (rc == BI_CMD_EXIT){
                free_cmd_list(clist);
                rc = OK;
                return rc;
            }
        }
    }
    free_cmd_list(clist);
    free(clist);
    return rc;
}
