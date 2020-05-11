typedef struct command_struct {
   char command[20];
   int index;
   int PID;
   struct command_struct* nextCommandPtr;
} CommandNode;
void CreateCommandNode(CommandNode* thisNode, char *cmd, int ind, CommandNode* nextCmd);
void InsertCommandAfter(CommandNode* thisNode, CommandNode* newNode);
CommandNode* GetNextCommand(CommandNode* thisNode);
CommandNode* FindCommand(CommandNode* cmd, int pid);
