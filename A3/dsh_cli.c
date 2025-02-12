#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "dshlib.h"

#define DRAGON_FILE "dragon.txt"

void print_commandList(command_list_t *clist){
    printf(CMD_OK_HEADER, clist->num);
    for(int i=0; i < clist->num; i++){
        printf("<%d> %s ", i+1, clist->commands[i].exe);
        if(clist->commands[i].args[0] != '\0'){
            printf("[%s]", clist->commands[i].args);
        }
        printf("\n");
    }
}

size_t get_file_size(char *filename){
    FILE *file = fopen(filename, "r");
    if (!file){
        return (size_t) -1;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fclose(file);
    return size;
}

void compress_ascii_art(char *buff){
    printf("%s\n", buff);
}

/*
 * Implement your main function by building a loop that prompts the
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.  Since we want fgets to also handle
 * end of file so we can run this headless for testing we need to check
 * the return code of fgets.  I have provided an example below of how
 * to do this assuming you are storing user input inside of the cmd_buff
 * variable.
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
 *
 *   Expected output:
 *
 *      CMD_OK_HEADER      if the command parses properly. You will
 *                         follow this by the command details
 *
 *      CMD_WARN_NO_CMD    if the user entered a blank command
 *      CMD_ERR_PIPE_LIMIT if the user entered too many commands using
 *                         the pipe feature, e.g., cmd1 | cmd2 | ... |
 *
 *  See the provided test cases for output expectations.
 */
int main()
{
    char *cmd_buff = malloc(SH_CMD_MAX);
    int rc = 0;
    char *dragon_buff;
    bool dragon_initialized = false;

    command_list_t *clist = (command_list_t *)malloc(sizeof(command_list_t));

    while(1){
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL){
            break;
        } else if (cmd_buff[0] == '\n'){
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
        int success = build_cmd_list(cmd_buff, clist);

        if(success == OK){
            if (clist->num >0){
                if (strcmp(clist->commands[clist->num - 1].exe, EXIT_CMD) == 0){
                    rc = OK;
                    break;
                } else if (strcmp(clist->commands[clist->num - 1].exe, DRAGON_CMD) == 0){
                    if (!dragon_initialized){
                        size_t dragon_size = get_file_size(DRAGON_FILE);
                        if (dragon_size == (size_t) -1){
                            printf("Error getting size of file: %s\n", DRAGON_FILE);
                            break;
                        }
                        dragon_buff = malloc(dragon_size);
                        FILE *file = fopen(DRAGON_FILE, "r");
                        size_t len = fread(dragon_buff, 1, dragon_size - 1, file);
                        fclose(file);
                        if (len == 0){
                            printf("Error reading file: %s\n", DRAGON_FILE);
                            break;
                        }
                        dragon_initialized = true;
                    }
                    printf("%s\n", dragon_buff);
                    //eval_dragon_cmd(dragon_buff);
                }
            }
            print_commandList(clist);
        } else if (success == ERR_TOO_MANY_COMMANDS){
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
        }
    }

    free(dragon_buff);
    free(cmd_buff);
    free(clist);
    exit(rc);
}