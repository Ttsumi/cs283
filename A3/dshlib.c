#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

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

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    memset(clist, 0, sizeof(command_list_t));

    if (pipe_count(cmd_line) >= CMD_MAX){
        return ERR_TOO_MANY_COMMANDS;
    }

    const char outer_delimiters[] = PIPE_STRING;
    const char inner_delimiters[] = " \t";
    char* token;
    char* outer_saveptr = NULL;
    char* inner_saveptr = NULL;
    int cmd_count = 0;

    token = strtok_r(cmd_line, outer_delimiters, &outer_saveptr);
    while(token != NULL){           
        char* inner_token = strtok_r(token, inner_delimiters, &inner_saveptr);

        if (strlen(inner_token) > EXE_MAX){
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        strcpy(clist->commands[cmd_count].exe, inner_token);
        inner_token = strtok_r(NULL, "", &inner_saveptr);

        if(inner_token){
            if (strlen(inner_token) > ARG_MAX){
                return ERR_CMD_OR_ARGS_TOO_BIG;
            } else {
                strcpy(clist->commands[cmd_count].args, inner_token);
            }
        }
        clist->num = ++cmd_count;
        token = strtok_r(NULL, outer_delimiters, &outer_saveptr);
    }
    return OK;
}