#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CommandNode.h"
/**
 *CommandNode.c
 *Author: Bill Andreopoulos
 *The function bodies for a linked list of commands
 *CS149 assignment 3 usage only
**/
/**
   Example of linked list initialization in code:
   //First command node (head of list)
   headCommand = (CommandNode*)malloc(sizeof(CommandNode));
   CreateCommandNode(headCommand, command, index, NULL);
    .....
   //Later command nodes
   nextCommand1 = (CommandNode*)malloc(sizeof(CommandNode));
   CreateCommandNode(nextCommand1, command, index, NULL);
   InsertCommandAfter(headCommand, nextCommand1);
**/
//create a new command node. usually nextCmd can be NULL and function 
//InsertCommandAfter can be called to insert after head node.

void CreateCommandNode(CommandNode* thisNode, char *cmd, int ind, CommandNode* nextCmd) {
   //this is useful if you store a string (char *): strcpy(thisNode->command, cmd);
   strcpy(thisNode->command, cmd);
   thisNode->index = ind;
   thisNode->nextCommandPtr = nextCmd;
   return;
}
//insert node newNode after thisNode 
void InsertCommandAfter(CommandNode* thisNode, CommandNode* newNode) {
   CommandNode* tmpNext = NULL;
   tmpNext = thisNode->nextCommandPtr;
   thisNode->nextCommandPtr = newNode;
   newNode->nextCommandPtr = tmpNext;
   return;
}
//get next command node in linked list
CommandNode* GetNextCommand(CommandNode* thisNode) {
   return thisNode->nextCommandPtr;
}
//find a command based on the pid
CommandNode* FindCommand(CommandNode* cmd, int pid) {
   CommandNode* tmpNext = cmd;
   while (tmpNext != NULL) {
       if (tmpNext->PID == pid) { return tmpNext; }
       tmpNext = tmpNext->nextCommandPtr;
   }
   return NULL;
}

