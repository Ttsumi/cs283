#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"
#include <errno.h>

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
        if (cmd_line[i] == '"') {
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

Built_In_Cmds match_command(const char *input){
    if(strcmp(input, "cd") == 0){
        return BI_CMD_CD;
    } else if (strcmp(input, "dragon") == 0){
        return BI_CMD_DRAGON;
    } else if (strcmp(input, "exit") == 0){
        return BI_CMD_EXIT;
    } else if (strcmp(input, "execute") == 0){
        return BI_EXECUTED;
    } else if (strcmp(input, "rc") == 0){
        return BI_RC;
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
        return BI_CMD_EXIT;
    } else if (strcmp(cmd->argv[0], "rc") == 0){
        return BI_RC;
    }
    return BI_CMD_EXIT;
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
    cmd_buff_t *cmd = (cmd_buff_t*)malloc(sizeof(cmd_buff_t));
    alloc_cmd_buff(cmd);

    // TODO IMPLEMENT MAIN LOOP
    while (1){
        
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
            printf("\n");
            break;
        }
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';

        // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff
        build_cmd_buff(cmd_buff, cmd);

        // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
        // the cd command should chdir to the provided directory; if no directory is provided, do nothing

        // TODO IMPLEMENT if not built-in command, fork/exec as an external command
        // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"
        if( match_command(cmd->argv[0]) == BI_NOT_BI){
            int f_result = fork();
            int c_result;
            if (f_result < 0)
            {
                rc = ERR_EXEC_CMD;
                perror("fork error");
                break;
            } else if (f_result == 0) {
                execvp(cmd->argv[0], cmd->argv);
                exit(errno);
            } else
            {
                wait(&c_result);
                rc = WEXITSTATUS(c_result);
            }
        } else {
            Built_In_Cmds built_in_cmd = exec_built_in_cmd(cmd);
            if (built_in_cmd == BI_CMD_EXIT){
                clear_cmd_buff(cmd);
                free_cmd_buff(cmd);
                rc = OK;
                return rc;
            } else if (built_in_cmd == BI_RC){
                printf("%d\n", rc);
                rc = OK;
            }
        }
        clear_cmd_buff(cmd);
    }

    free_cmd_buff(cmd);
    return rc;
}
