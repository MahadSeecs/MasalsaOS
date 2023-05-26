// command_line_arguments.cpp  

#include <iostream>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <fstream>
#include <map>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>

#define TOKENSIZE 100


using namespace std;
void  StrTokenizer(char *line, char **argv);
void  myExecvp(char **argv);
int GetEnv();

void saveCommandToHistory(const char* command);
void copyFile(const string& sourceFile, const string& destinationFile);
void traverseDirectory(const char* dirPath, int depth = 0);


void DisplayCredits();
void DisplayCommands();
void AllPID();
void PID();
void KillPID(int pid);

void runTraceroute(char **argv);
void handleExport(char **argv);
void changeDirectory(char **argv);


map<string, string> colors = {
    {"red", "\033[0;31m"},
    {"green", "\033[0;32m"},
    {"blue", "\033[0;34m"},
    // Add more color mappings as needed
};


int main()
{

    char *path2;
    char *arr [250];
    char *Tokenized ;
    char input[250];
    char *argv[TOKENSIZE];

    while(true)
    {
        cout<< "masalsa-> ";
        cin.getline(input,250);
        saveCommandToHistory(input);
        StrTokenizer(input, argv);  
        if (strcmp(input, "exit") == 0)  

        {
            break;
        }
        else if (strcmp(input, "\n") == 0){
            continue;
        }
        else if (strcmp(input, "history") == 0) {
            // Print the command history
            std::ifstream file("commandHistory.txt");
            cout<<"All Previously Used Commands"<<endl;
            if (file.is_open()) {
                std::string line;
                while (std::getline(file, line)) {
                    cout << line << endl;
                }
                file.close();
            }
            else {
                std::cerr << "ERROR: Failed to open command history file." << std::endl;
            }
            continue;
        }
        else if (strcmp(argv[0], "color") == 0 && argv[1] != nullptr) {
			string colorName(argv[1]);
			auto it = colors.find(colorName);
			if (it != colors.end()) {
				cout << it->second;
				continue;
			} else {
				cout << "Invalid color name." << endl;
				continue;
			}
		}
		else if (strcmp(argv[0], "cp") == 0 && argv[1] != nullptr && argv[2] != nullptr) {
			std::string sourceFile(argv[1]);
			std::string destinationFile(argv[2]);
			copyFile(sourceFile, destinationFile);
			continue;
		}
		else if (strcmp(argv[0], "clr") == 0) {
			const char* clearScreen = "\033[2J\033[1;1H";
			cout << clearScreen;
			continue;
		}
		else if (strcmp(argv[0], "filestruct") == 0) {
			traverseDirectory(".", 0);
			continue;
		}
		else if (strcmp(input, "credits") == 0){
			DisplayCredits();
			continue;
		}
		else if (strcmp(input, "cmdlist") == 0){
			DisplayCommands();
			continue;
		}
		else if (strcmp(input, "kill") == 0){
		    if (argv[1] != NULL){
		       int pid = atoi(argv[1]);
		       KillPID(pid);
		       continue;
		    }
		    else
		    {
			cout << "Please provide a process ID to kill" << endl;
		    }
        }
        else if (strcmp(argv[0], "export")==0)
		{
			handleExport(argv);
			continue;
		}
	else if(strcmp(argv[0], "cat")==0){
		if (argv[1] != nullptr) {
			std::ifstream file(argv[1]);
			if (file.is_open()) {
				std::string line;
				while (std::getline(file, line)) {
					std::cout << line << std::endl;
				}

				file.close();
			} else {
				std::cerr << "Failed to open file: " << argv[1] << std::endl;
			}
		} else {
			std::cerr << "Error no filename" << std::endl;
		}
		continue;
	}

        else if (strcmp(input, "allpid")==0){
            AllPID();
            continue;

        }

        else if (strcmp(input, "pid")==0){
            PID();
            continue;
        }
        
		myExecvp(argv);
	}
    return 0;
}

void  myExecvp(char **argv)
{
    pid_t  pid;
    int    status;
    int childStatus;
    pid = fork();
    if(pid == 0)
	{	
		if (strcmp(*argv, "cd")==0)
			{changeDirectory(argv);}
		else
		{
			childStatus = execvp(*argv, argv);
			if (childStatus < 0){
			cout<<"ERROR:wrong input"<<endl;
			}
			exit(0);
		}
		

	}
    else if(pid < 0)
    {
        cout<< "somthing went wrong!"<<endl;
    }
    else 

    {   int status;
        waitpid(pid, &status , 0);

    }
}

void StrTokenizer(char *input, char **argv)
{
    char *stringTokenized;
    stringTokenized = strtok(input, " ");
    while(stringTokenized != NULL)
    {
        *argv++  = stringTokenized;
        stringTokenized = strtok(NULL, " ");
    }

    *argv = NULL;
}

int GetEnv()
{
    char *path2;
    char *arr2[250];
    char *Tokenized ;
    path2 = getenv("PATH");
    Tokenized = strtok(path2, ":");
    int k = 0;
    while(Tokenized != NULL)
    {
        arr2[k] = Tokenized;
        Tokenized = strtok(NULL, ":");
        k++;
    }

    arr2[k] = NULL;
}

void saveCommandToHistory(const char* command) {
    ofstream file("commandHistory.txt", std::ios_base::app);
    if (file.is_open()) {
        file << command << std::endl;
        file.close();
    } else {
        std::cerr << "ERROR: Failed to open command history file." << std::endl;
    }
}

void copyFile(const std::string& sourceFile, const std::string& destinationFile) {
    ifstream source(sourceFile, std::ios::binary);
    ofstream destination(destinationFile, std::ios::binary);

    if (source.is_open() && destination.is_open()) {
        destination << source.rdbuf();
        cout << "File copied successfully." << endl;
    } else {
        cout << "Failed to copy file." << endl;
    }

    source.close();
    destination.close();
}

void traverseDirectory(const char* dirPath, int depth) {
    DIR* dir = opendir(dirPath);
    if (dir == nullptr) {
        std::cerr << "Failed to open directory: " << dirPath << std::endl;
        return;
    }

    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // Skip "." and ".." directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        std::cout << std::string(depth, ' ') << "+ " << entry->d_name << std::endl;

        // Construct the full path of the entry
        std::string fullPath = std::string(dirPath) + "/" + entry->d_name;

        struct stat fileStat;
        if (stat(fullPath.c_str(), &fileStat) == 0 && S_ISDIR(fileStat.st_mode)) {
            traverseDirectory(fullPath.c_str(), depth + 2);
        }
    }

    closedir(dir);
}

void DisplayCredits(){
 	const char *longString = R""""(
=====================================================================================================================================================================
          _____                    _____                    _____                    _____                    _____            _____                    _____          
         /\    \                  /\    \                  /\    \                  /\    \                  /\    \          /\    \                  /\    \         
        /::\____\                /::\    \                /::\    \                /::\    \                /::\____\        /::\    \                /::\    \        
       /::::|   |               /::::\    \              /::::\    \              /::::\    \              /:::/    /       /::::\    \              /::::\    \       
      /:::::|   |              /::::::\    \            /::::::\    \            /::::::\    \            /:::/    /       /::::::\    \            /::::::\    \      
     /::::::|   |             /:::/\:::\    \          /:::/\:::\    \          /:::/\:::\    \          /:::/    /       /:::/\:::\    \          /:::/\:::\    \     
    /:::/|::|   |            /:::/__\:::\    \        /:::/__\:::\    \        /:::/__\:::\    \        /:::/    /       /:::/__\:::\    \        /:::/__\:::\    \    
   /:::/ |::|   |           /::::\   \:::\    \       \:::\   \:::\    \      /::::\   \:::\    \      /:::/    /        \:::\   \:::\    \      /::::\   \:::\    \   
  /:::/  |::|___|______    /::::::\   \:::\    \    ___\:::\   \:::\    \    /::::::\   \:::\    \    /:::/    /       ___\:::\   \:::\    \    /::::::\   \:::\    \  
 /:::/   |::::::::\    \  /:::/\:::\   \:::\    \  /\   \:::\   \:::\    \  /:::/\:::\   \:::\    \  /:::/    /       /\   \:::\   \:::\    \  /:::/\:::\   \:::\    \ 
/:::/    |:::::::::\____\/:::/  \:::\   \:::\____\/::\   \:::\   \:::\____\/:::/  \:::\   \:::\____\/:::/____/       /::\   \:::\   \:::\____\/:::/  \:::\   \:::\____\
\::/    / ~~~~~/:::/    /\::/    \:::\  /:::/    /\:::\   \:::\   \::/    /\::/    \:::\  /:::/    /\:::\    \       \:::\   \:::\   \::/    /\::/    \:::\  /:::/    /
 \/____/      /:::/    /  \/____/ \:::\/:::/    /  \:::\   \:::\   \/____/  \/____/ \:::\/:::/    /  \:::\    \       \:::\   \:::\   \/____/  \/____/ \:::\/:::/    / 
             /:::/    /            \::::::/    /    \:::\   \:::\    \               \::::::/    /    \:::\    \       \:::\   \:::\    \               \::::::/    /  
            /:::/    /              \::::/    /      \:::\   \:::\____\               \::::/    /      \:::\    \       \:::\   \:::\____\               \::::/    /   
           /:::/    /               /:::/    /        \:::\  /:::/    /               /:::/    /        \:::\    \       \:::\  /:::/    /               /:::/    /    
          /:::/    /               /:::/    /          \:::\/:::/    /               /:::/    /          \:::\    \       \:::\/:::/    /               /:::/    /     
         /:::/    /               /:::/    /            \::::::/    /               /:::/    /            \:::\    \       \::::::/    /               /:::/    /      
        /:::/    /               /:::/    /              \::::/    /               /:::/    /              \:::\____\       \::::/    /               /:::/    /       
        \::/    /                \::/    /                \::/    /                \::/    /                \::/    /        \::/    /                \::/    /        
         \/____/                  \/____/                  \/____/                  \/____/                  \/____/          \/____/                  \/____/         
====================================================================================================================================================================
                                                                                                                                                                       
)"""";
	cout << "Creator: Some dude on git"<<endl;
	cout <<"Team members:"<<endl;
	cout <<"Arsal Rahman"<<endl;
	cout <<"Musa Ali Kazmi"<<endl;
	cout <<"Mahad Mohtashim"<<endl<<endl;
	cout << longString;

	
}

void DisplayCommands(){
cout << "Command list:"<<endl;
cout << "1) cmdlist: display the list of commands"<<endl;
cout << "2) cd <dir>: change directory" <<endl;
cout << "3) pid: display pid of current process" <<endl;
cout << "4) allpid: display pid of all running processes" <<endl;
cout << "5) kill <pid>: kill the process with the given pid."<<endl;
cout << "6) export <var type> = <value>: create environment variable of that format"<<endl;
cout << "7) history: display a list of all previously used commands"<<endl;
cout << "8) color <color>: change color of the terminal"<<endl;
cout << "9) cp <f1> <f2>: copy contents of f1 to f2"<<endl;
cout << "10) clr: clear terminal"<<endl;
cout << "11) filestruct: print file structure and sub-directories"<<endl;
cout << "12) traceroute: display IP of incoming packets"<<endl;
cout << "13) credits: display credits"<<endl;
cout << "14) ls: list all files in directory" <<endl;
cout << "15) cmp: compare two values" <<endl;
cout << "16) man <cmd>: manual for command " <<endl;
cout << "17) echo <string>: print string"<<endl;
cout << "18) mkdir <dir>: make directory"<<endl;
cout << "19) cat <file>: read file to command line"<<endl;
cout << "20) exit: exit masalsa" <<endl;
}

void PID(){
    ifstream selfStat("/proc/self/stat");
    if (selfStat)
    {
        string pid;
        selfStat >> pid; // Read the first value (process ID) from /proc/self/stat
        cout << "Process ID: " << pid << endl;
    }

}



void AllPID(){
    cout << "Process IDs of all processes:" << endl;
    pid_t pid = fork();
    if (pid == 0)
    {
        execlp("ps", "ps", "-e", "-o", "pid,comm", NULL);
        exit(0);
    }
    else if (pid < 0)
    {
        cout << "Failed to retrieve process IDs" << endl;
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
}

void KillPID(int pid){
    int result = kill(pid, SIGTERM);
    if (result == 0)
    {
        cout << "Process with ID " << pid << " terminated successfully" << endl;
    }
    else
    {
        cout << "Failed to terminate process with ID " << pid << endl;
    }

}

void changeDirectory(char **argv)
{	
	// Check if the command is traceroute
    if (strcmp(*argv, "traceroute") == 0) {
        runTraceroute(argv);
        return;
    }
    if (argv[1] == nullptr)
    {
        // No directory provided, change to the home directory
        const char *homeDir = getenv("HOME");
        chdir(homeDir);
    }
    else
    {
        // Change to the specified directory
        int result = chdir(argv[1]);
        if (result != 0)
        {
            std::cout << "Error: Failed to change directory" << std::endl;
        }
    }
}

void handleExport(char **argv)
{
    if (argv[1] != NULL)
    {
        char *name = strtok(argv[1], "=");
        char *value = strtok(NULL, "=");

        if (name != NULL && value != NULL)
        {
            int result = setenv(name, value, 1);
            if (result == 0)
            {
                cout << "Environment variable exported successfully" << endl;
            }
            else
            {
                cout << "Failed to export environment variable" << endl;
            }
        }
        else
        {
            cout << "ERROR: Invalid syntax for export command" << endl;
        }
    }
    else
    {
        cout << "ERROR: Invalid syntax for export command" << endl;
    }
}

void runTraceroute(char **argv) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        execvp(*argv, argv);
        // If execvp fails, print an error message
        cerr << "ERROR: Failed to execute traceroute" << endl;
        exit(1);
    } else if (pid < 0) {
        // Fork failed
        cerr << "ERROR: Fork failed" << endl;
        exit(1);
    } else {
        // Parent process
        waitpid(pid, &status, 0);
    }
}

