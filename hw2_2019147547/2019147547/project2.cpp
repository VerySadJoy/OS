/*
2019147547 Jin Ha Joo Project2
*/

//Included Modules
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <list>

//using namespace std
using namespace std;

//A Static String Variable that will Store the Entire Output
static string output_text = "";

//Process Struct
struct Process {
    string name; //Process Name
    int pid; //Process ID
    int ppid; //Parent Process ID
    char waiting_type; //Waiting Type(Either S or W)
    vector<string> file;
    int current_line;
};

//Status Struct
struct Status {
    int cycle; //Cycle Number
    string mode; //Mode Type(Either Kernel or User)
    string command; //Command being Prompted
    Process* process_running; //Currently Running Process
    vector<Process*> process_ready; //Ready Queue
    vector<Process*> process_waiting; //Waiting Queue
    Process* process_new; //Newly Generated Process
    Process* process_terminated; //Terminated Process
};

//Function that will Store the Output Text to Static Variable "output_text"
void store_status(Status status) {
    //0. Cycle Number
    output_text = output_text +  "[cycle #" + to_string(status.cycle) + "]\n";
    
    //1. Current Mode
    output_text = output_text +  "1. mode: " + status.mode + "\n";
    
    //2. Current Command
    output_text = output_text + "2. command: " + status.command + "\n";
    
    //3. Currently Running Process, None When There is No Running Process
    if (status.process_running == NULL) {
        output_text = output_text + "3. running: none\n";
    } 
    else {
        Process* i = status.process_running;
        output_text = output_text + "3. running: " + to_string(i->pid) + "(" + i->name + ", " + to_string(i->ppid) + ")\n";
    }

    //4. Current Ready Queue, Arranged from the Left Based on Priority
    if (status.process_ready.size() == 0) {
        output_text = output_text + "4. ready: none\n";
    } 
    else {
        output_text = output_text + "4. ready:";
        for (int i = 0; i < status.process_ready.size(); i++) { //One Blank Space to Seperate
            output_text = output_text + " " + to_string(status.process_ready[i]->pid);
        }
        output_text = output_text + "\n";
    }

    //5. Current Waiting Queue, Arranged from the Left Based on Priority
    if (status.process_waiting.size() == 0) {
        output_text = output_text + "5. waiting: none\n";
    } 
    else {
        output_text = output_text + "5. waiting:";
        for (int i = 0; i < status.process_waiting.size(); i++) { // 공백 한칸으로 구분
            output_text = output_text + " " + to_string(status.process_waiting[i]->pid) + "(" + status.process_waiting[i]->waiting_type + ")";
        }
        output_text = output_text + "\n";
    }
    

    //6. Currently New Process
    if (status.process_new == NULL) {
        output_text = output_text + "6. new: none\n";
    } 
    else {
        Process* i = status.process_new;
        output_text = output_text + "6. new: " + to_string(i->pid) + "(" + i->name + ", " + to_string(i->ppid) + ")\n";
    }

    //7. Currently Terminated Process
    if (status.process_terminated == NULL) {
        output_text = output_text + "7. terminated: none\n";
    } 
    else {
        Process* i = status.process_terminated;
        output_text = output_text + "7. terminated: " + to_string(i->pid) + "(" + i->name + ", " + to_string(i->ppid) + ")\n";
    }
        
    //Seperation between the Cycles
    output_text = output_text + "\n";
    
    
}

//Function that Returns if the Process p1 is a Child of p2
bool is_child(Process* p1, Process* p2){
    return (p1->ppid == p2->pid);
}

//Main Function
int main(int arc, char* argv[]) {//int arc, char* argv[]
    
    //Open the init from the Given Directory
    string pathname = argv[1]; //pathname Stores the Input Path from the Terminal
    pathname = pathname + "/";
    ifstream init_ifstream; //init_ifstream is the Initial File
    init_ifstream.open(pathname + "init"); //Open the Given Path
    
    //Variables Used in Main Function
    ifstream temp_ifstream; //Temporary ifstream Object
    int process_counter = 1; //Loop Control Variable
    string line; //string that will Read the Lines of the "reading_file"
    Process initial_process = {"init", 1, 0, '0', {}, 0}; //Process Object that Stores the Initial Process
    Status status = {0, "kernel", "boot", NULL, {}, {}, &initial_process, NULL}; //Status Object that will Keep Track of the Status of the Kernel
    string temp_string = ""; //Temporary string that will Store the Commands such as run, sleep, wait and etc
    string temp_process_name = ""; //Temporary string that will Store the New Process Name
    int temp_int = 0; //Temporary int that will Store the Number of the Command Needed to be Executed
    vector<int> waiting_cycle; //vector of int that will Keep Track of Waiting Cycle of Processes in Waiting Queue
    vector<int> temp_vector; //Temporary int vector Created to Erase Processes in Waiting Queue
    int run_counter = 0; //int to Count the Number of Runs Required to be Made
    vector<Process*> total_process; //Vector that Keeps the all Processes that were Created During the Cycle
    list<Process> temp_process {}; //Temporary Process Variable Used for New Proccess being Created
    bool child_is_terminated = true; //bool Value Used to Store Whether the Child Process is Terminated or Not
    Process ttemp_process; //Temporary Process
    string file_reader = ""; //Temporary Filing Reading String

    //Initialization
    while(getline(init_ifstream, file_reader)){
        if(file_reader.size() > 0){
            initial_process.file.push_back(file_reader);
        }
    }
    file_reader = "";
    total_process.push_back(&initial_process);
    
    //While Loop until all Processes are Terminated
    while(process_counter > 0){
        //Store Status to "output_text" Every time the Loop is Iterated
        store_status(status);

        //Processes Controls
        //When There is no Running Process and Ready Queue is Empty, then New Process will be Running Process
        if (status.process_running == NULL and status.process_ready.empty()){
            status.process_running = status.process_new;
            status.process_new = NULL;
        }
        //When Running Process is Empty, then the First Process in the Ready Queue will be Running Process
        else if (status.process_running == NULL){
            status.process_running = status.process_ready[0];
            status.process_ready.erase(status.process_ready.begin());
        }
        //When Running Process Exists, then the New Process Goes into the End of the Ready Queue
        if (status.process_running != NULL and status.process_new != NULL){
            status.process_ready.push_back(status.process_new);
            status.process_new = NULL;
        }
        //Remove the Process in Terminated State
        if (status.process_terminated != NULL){
            status.process_terminated = NULL;
        }
        
        //When Current Command is not Run
        if(run_counter == 0){
            if (status.command == "boot"){ //Changing Status to Schedule when Booting
                status.command = "schedule";
            }

            else if (status.command == "schedule"){ //When the Current Command is Scheduling, then Proceed to next Appropriate Command by Reading the Next Line of "reading_file"
                if (status.process_running != NULL){
                    status.command = status.process_running->file[status.process_running->current_line];
                    status.process_running->current_line++;
                    status.mode = "user";
                }
                else {
                    status.command = "schedule";
                    status.mode = "kernel";
                }
            }

            else if (status.command == "system call"){ //System Call: Mode -> Kernel Mode, Next Command -> Schedule
                status.command = "schedule";
                status.mode = "kernel";
            }

            else if (status.command.find(" ") != std::string::npos){ //When the Command is fork_and_exec, run, or sleep
                temp_string = status.command.substr(0, status.command.find(" ")); //Store First Part to temp_string
                temp_process_name = status.command.substr(status.command.find(" ") + 1, status.command.length()); //Store the Last Part to temp_process_name
                if (temp_string == "run"){ //When Command is Run
                    temp_int = std::stoi(temp_process_name);
                    run_counter = temp_int; //run_counter is the int Value after the Run Command
                }

                else if (temp_string == "sleep"){ //When Command is Sleep
                    status.mode = "kernel";
                    status.command = "system call";
                    temp_int = std::stoi(temp_process_name);
                    status.process_running->waiting_type = 'S'; //Change Waiting Type to Sleep
                    status.process_waiting.push_back(status.process_running);
                    status.process_running = NULL;
                    waiting_cycle.push_back(temp_int - 1); //waiting_cycle will Contain How Long the Process will Sleep
                }

                else if (temp_string == "fork_and_exec"){ //When Command is fork_and_exec
                    status.command = "system call";
                    status.mode = "kernel";
                    ttemp_process = {temp_process_name, total_process.back()->pid + 1, status.process_running->pid, '0', {}, 0};
                    temp_process.push_back(ttemp_process);
                    status.process_new = &(temp_process.back());
                    status.process_ready.push_back(status.process_running);
                    status.process_running = NULL;
                    total_process.push_back(status.process_new);
                    process_counter++;                
                    temp_ifstream.open(pathname + temp_process_name);
                    while(getline(temp_ifstream, file_reader)){
                        if(file_reader.size() > 0){
                            status.process_new->file.push_back(file_reader);
                        }
                    }
                    file_reader = "";
                    temp_ifstream.close();
                    temp_ifstream.clear();
                }
            }

            else if (status.command == "exit"){ //When Command is exit
                status.command = "system call";
                status.mode = "kernel";
                status.process_terminated = status.process_running;
                status.process_running = NULL;
                process_counter--;
                for (int i = 0; i < status.process_waiting.size(); i++){ //Check if Parent is Sleeping and Wake it up
                    if (is_child(status.process_terminated, status.process_waiting[i])){
                        status.process_waiting[i]->waiting_type = '0';
                        status.process_ready.push_back(status.process_waiting[i]);
                        status.process_waiting.erase(status.process_waiting.begin() + i);
                    }
                }
            }

            else if(status.command == "wait"){ //When Command is wait
                status.command = "system call";
                status.mode = "kernel";
                child_is_terminated = true;
                for (int i = 0; i < status.process_ready.size(); i++){ //Check if Child is in the Ready Queue
                    if(is_child(status.process_ready[i], status.process_running)){
                        child_is_terminated = false;
                    }
                }
                for (int i = 0; i < status.process_waiting.size(); i++){ //Check if Child is in the Waiting Queue
                    if(is_child(status.process_waiting[i], status.process_running)){
                        child_is_terminated = false;
                    }
                }
                if (child_is_terminated){ //When the Child Process is Terminated, Go to Ready Queue
                    status.process_ready.push_back(status.process_running);
                    status.process_running = NULL;
                }
                else { //When the Child Process is not Terminated, Go to Waiting Queue
                    status.process_running->waiting_type = 'W';
                    status.process_waiting.push_back(status.process_running);
                    waiting_cycle.push_back(-1);
                    status.process_running = NULL;
                }
            }
            
            
        }

        //When the Current Command is Run
        if(run_counter != 0){
            run_counter--;
            if (run_counter == 0){
                if (status.process_running != NULL){
                    status.command = status.process_running->file[status.process_running->current_line];
                    status.process_running->current_line++;
                }
            }
        }

        //When Waiting is Over, Go to Ready Queue
        for (int i = 0; i < waiting_cycle.size(); i++){
            if(waiting_cycle[i] == 0){
                status.process_ready.push_back(status.process_waiting[i]);
                temp_vector.push_back(i);
            }
        }
        for (int i = 0; i < temp_vector.size(); i++){
            status.process_waiting.erase(status.process_waiting.begin() + temp_vector[i] - i);
            waiting_cycle.erase(waiting_cycle.begin() + temp_vector[i] - i);
        }
        temp_vector.clear();

        //Reduce One Cycle of Waiting Processes
        for (int i = 0; i < waiting_cycle.size(); i++){
            waiting_cycle[i]--;
        }

        //Increase status.cycle by One
        status.cycle++;
    }

    //Store the Last Status
    store_status(status);

    //Output
    ofstream write_file;
    write_file.open("result");
    write_file << output_text << "";
    write_file.close();

    //Return
    return 0;
}