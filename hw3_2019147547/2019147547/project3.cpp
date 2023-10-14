/*
2019147547 Jin Ha Joo Project3
*/

//Included Modules
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <list>
#include <algorithm>

//using namespace std
using namespace std;

//A Static String Variable that will Store the Entire Output
static string output_text = "";

//Static Variables Used for Page Replacement
static int fifo_counter = 0;
static int ult_counter = 0;

//Page Struct
struct Page {
    int page_id;
    int* process_id;
    string authority;
    int allocation_id;
    int index = 0;
};

//Memory Struct
struct Memory{
    vector<Page*> page_vector;
    vector<int> order;
};

//Process Struct
struct Process {
    string name; //Process Name
    int pid; //Process ID
    int ppid; //Parent Process ID
    char waiting_type; //Waiting Type(Either S or W)
    vector<string> file;
    int current_line;
    int allocation_id;
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
    Memory physical_memory; // Physical Memory
};

//Function that will Store the Output Text to Static Variable "output_text"
void store_status(Status status, vector<Page*> vlist, vector<Page*> tlist) {
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
    
    //8. Physical Memory
    output_text = output_text + "4. physical memory:\n|";
    for (int i = 0; i < 16; i++){
        if(status.physical_memory.page_vector[i] == NULL){
            output_text = output_text + "-";
        }
        else{

            output_text = output_text + to_string(*(status.physical_memory.page_vector[i]->process_id)) + "(" + to_string(status.physical_memory.page_vector[i]->page_id) + ")";
        }
        if (i % 4 == 3){
            output_text = output_text + "|";
        }
        else{
            output_text = output_text + " ";
        }
    }
    output_text = output_text + "\n";

    //Virtual Memory
    if (status.process_running != NULL){
        output_text = output_text + "5. virtual memory:\n|";
        for (int i = 0; i < 32; i++){
            if(i >= vlist.size()){
                output_text = output_text + "-";
            }
            else if(vlist[i] == NULL){
                output_text = output_text + "-";
            }
            else{
                output_text = output_text + to_string(vlist[i]->page_id);
            }
            if (i % 4 == 3){
                output_text = output_text + "|";
            }
            else{
                output_text = output_text + " ";
            }
        }
        output_text = output_text + "\n";
    }
   
    
    //Page Table
    if (status.process_running != NULL){
        output_text = output_text + "6. page table:\n|";
        for (int i = 0; i < 32; i++){
            if(tlist[i] == NULL){
                output_text = output_text + "-";
            }
            else{
                if(tlist[i]->index != -1){
                    output_text = output_text + to_string(tlist[i]->index);
                }
                else{
                    output_text = output_text + "-";
                }
            }
            if (i % 4 == 3){
                output_text = output_text + "|";
            }
            else{
                output_text = output_text + " ";
            }
        }
        output_text = output_text + "\n|";
        for (int i = 0; i < 32; i++){
            if(tlist[i] == NULL){
                output_text = output_text + "-";
            }
            else{
                output_text = output_text + tlist[i]->authority;
            }
            if (i % 4 == 3){
                output_text = output_text + "|";
            }
            else{
                output_text = output_text + " ";
            }
        }
        output_text = output_text + "\n";
    }
    

    //Seperation between the Cycles
    output_text = output_text + "\n";
    
    
}

//Function that Returns if the Process p1 is a Child of p2
bool is_child(Process* p1, Process* p2){
    return (p1->ppid == p2->pid);
}

//Function that Returns the Mimimum Value's Index in the Vector
int min_index(vector<int> vect){
    auto it = std::minmax_element(vect.begin(), vect.end());
    return std::distance(vect.begin(), it.first);
}

//Function that Returns the Maximum Value's Index in the Vector
int max_index(vector<int> vect){
    auto it = std::minmax_element(vect.begin(), vect.end());
    return std::distance(vect.begin(), it.second);
}

//Function that Checks if Vector is Full(No Null Values)
bool full_check(vector<Page*> vect){
    for (int i = 0; i < vect.size(); i++){
        if (vect[i] == NULL){
            return false;
        }
    }
    return true;
}

//Function that Returns the First Index of a Null place
int find_null(vector<Page*> vect){
    for (int i = 0; i < vect.size(); i++){
        if (vect[i] == NULL){
            return i;
        }
    }
    return -1;
}

//FIFO Page Replacement Policy
Status fifo(Status status, vector<Page*> input_vect){
    int temp_num = -1;
    for (int i = 0; i < input_vect.size(); i++){
        if(!full_check(status.physical_memory.page_vector)){
            if(status.physical_memory.page_vector[i] == NULL){
                status.physical_memory.page_vector[i] = input_vect[i];
                fifo_counter++;
                status.physical_memory.order[i] = fifo_counter;
            }
            else if(status.physical_memory.page_vector[i] != NULL){
                temp_num = find_null(status.physical_memory.page_vector);
                status.physical_memory.page_vector[temp_num] = input_vect[i];
                fifo_counter++;
                status.physical_memory.order[temp_num] = fifo_counter;
            }
        }
        else{
            temp_num = min_index(status.physical_memory.order);
            status.physical_memory.page_vector[temp_num] = input_vect[i];
            fifo_counter++;
            status.physical_memory.order[temp_num] = fifo_counter;
        }
    }
    return status;

}

//LRU Page Replacement Policy
Status lru(Status status, vector<Page*> input_vect){
    int temp_num = -1;
    for (int i = 0; i < input_vect.size(); i++){
        if(!full_check(status.physical_memory.page_vector)){
            if(status.physical_memory.page_vector[i] == NULL){
                status.physical_memory.page_vector[i] = input_vect[i];
                status.physical_memory.order[i] = status.cycle;
            }
            else if(status.physical_memory.page_vector[i] != NULL){
                temp_num = find_null(status.physical_memory.page_vector);
                status.physical_memory.page_vector[temp_num] = input_vect[i];
                status.physical_memory.order[temp_num] = status.cycle;
            }
        }
        else{
            temp_num = min_index(status.physical_memory.order);
            status.physical_memory.page_vector[temp_num] = input_vect[i];
            status.physical_memory.order[temp_num] = status.cycle;
        }
    }
    return status;
}

//LFU Page Replacement Policy
Status lfu(Status status, vector<Page*> input_vect){
    int temp_num = -1;
    for (int i = 0; i < input_vect.size(); i++){
        if(!full_check(status.physical_memory.page_vector)){
            if(status.physical_memory.page_vector[i] == NULL){
                status.physical_memory.page_vector[i] = input_vect[i];
                status.physical_memory.order[i]++;
            }
            else if(status.physical_memory.page_vector[i] != NULL){
                temp_num = find_null(status.physical_memory.page_vector);
                status.physical_memory.page_vector[temp_num] = input_vect[i];
                status.physical_memory.order[temp_num]++;
            }
        }
        else{
            temp_num = min_index(status.physical_memory.order);
            status.physical_memory.page_vector[temp_num] = input_vect[i];
            status.physical_memory.order[temp_num]++;
        }
    }
    return status;
}

//MFU Page Replacement Policy
Status mfu(Status status, vector<Page*> input_vect){
    int temp_num = -1;
    for (int i = 0; i < input_vect.size(); i++){
        if(!full_check(status.physical_memory.page_vector)){
            if(status.physical_memory.page_vector[i] == NULL){
                status.physical_memory.page_vector[i] = input_vect[i];
                status.physical_memory.order[i]++;
            }
            else if(status.physical_memory.page_vector[i] != NULL){
                temp_num = find_null(status.physical_memory.page_vector);
                status.physical_memory.page_vector[temp_num] = input_vect[i];
                status.physical_memory.order[temp_num]++;
            }
        }
        else{
            temp_num = max_index(status.physical_memory.order);
            status.physical_memory.page_vector[temp_num] = input_vect[i];
            status.physical_memory.order[temp_num]++;
        }
    }
    return status;
}

//Function that Returns the First Index of Page in Page Vector
int get_index(vector<Page*> vect, Page* p){
    int index = std::find(vect.begin(), vect.end(), p) - vect.begin();
    if (index != vect.size()){
        return index;
    }
    else{
        return -1;
    }
}

//Main Function
int main(int arc, char* argv[]) {//int arc, char* argv[]
    
    //Open the init from the Given Directory
    string pathname = argv[1]; //pathname Stores the Input Path from the Terminal
    pathname = pathname + "/";
    ifstream init_ifstream(pathname + "init"); //init_ifstream is the Initial File
    string policy = argv[2];//"fifo";//argv[2];

    //Variables Used in Main Function
    ifstream temp_ifstream; //Temporary ifstream Object
    int process_counter = 1; //Loop Control Variable
    string line; //string that will Read the Lines of the "reading_file"
    Process initial_process;  //Process Object that Stores the Initial Process
    Status status; //Status Object that will Keep Track of the Status of the Kernel
    string temp_string = ""; //Temporary string that will Store the Commands such as run, sleep, wait and etc
    string temp_process_name = ""; //Temporary string that will Store the New Process Name
    vector<Process*> total_process; //Vector that Keeps the all Processes that were Created During the Cycle
    list<Process> temp_process {}; //Temporary Process Variable Used for New Proccess being Created
    Process ttemp_process; //Temporary Process
    string file_reader = ""; //Temporary Filing Reading String
    int temp_int = 0; // Temporary Integer
    vector<vector<Page*>> rvirtual = {}; //2D Vector of Pages that Contains the Virtual Memory
    vector<Page*> pvect (32, nullptr); //Temporary Page Vector
    vector<vector<Page*>> rpagetable = {}; //2d Vector that has Page Table Information
    vector<Page*> ptvect (32, nullptr); //Empty Page Vector
    bool fault = true; //Boolean Used to Check Fault
    vector<Page*> physic (16, nullptr); //Initial Physical Memory
    vector<int> order; //Vector that Contains Order of Pages in Physical Memory
    vector<Page*> temp_vector; //Temporary Vector
    int temp_index = 0; //Temporary Index
    Page* temp_page = NULL; //Temporary Page Pointer
    int fault_index = -1; //Fault Index



    //Initialization
    for (int i = 0; i < 16; i++){
        order.push_back(0);
    }
    initial_process = {"init", 1, 0, '0', {}, 0, -1};
    status = {0, "kernel", "boot", NULL, {}, {}, &initial_process, NULL, {physic, order}};

    //Read File
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
        if(status.process_running != NULL){
            if(rvirtual.size() > 0 and rpagetable.size() > 0){
                store_status(status, rvirtual[status.process_running->pid - 1], rpagetable[status.process_running->pid - 1]);
            }
            else{
                store_status(status, pvect, ptvect);
            }
        }
        else{
            store_status(status, {}, {});
        }
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

        else if (status.command == "fault"){
            status.command = "schedule";
            status.mode = "kernel";
        }

        else if (status.command.find(" ") != std::string::npos){ //When the Command is fork_and_exec, run, or sleep
            temp_string = status.command.substr(0, status.command.find(" ")); //Store First Part to temp_string
            temp_process_name = status.command.substr(status.command.find(" ") + 1, status.command.length()); //Store the Last Part to temp_process_name
            
            if (temp_string == "fork_and_exec"){ //When Command is fork_and_exec
                status.command = "system call";
                status.mode = "kernel";
                ttemp_process = {temp_process_name, total_process.back()->pid + 1, status.process_running->pid, '0', {}, 0, status.process_running->allocation_id};
                for (int i = 0; i < rvirtual[status.process_running->pid - 1].size(); i++){
                    if(rvirtual[status.process_running->pid - 1][i] != NULL){
                        rvirtual[status.process_running->pid - 1][i]->authority = "R";
                    }
                    
                }

                //Setting Up the Virtual Memory of Child Process
                rvirtual.push_back(rvirtual[status.process_running->pid - 1]);
                rpagetable.push_back(rpagetable[status.process_running->pid - 1]);
                temp_process.push_back(ttemp_process);
                status.process_new = &(temp_process.back());
                status.process_ready.push_back(status.process_running);
                status.process_running = NULL;
                total_process.push_back(status.process_new);
                process_counter++;     

                //Reading New File   
                ifstream temp_ifstream(pathname + temp_process_name);
                while(getline(temp_ifstream, file_reader)){
                    if(file_reader.size() > 0){
                        status.process_new->file.push_back(file_reader);
                    }
                }
                file_reader = "";
                temp_ifstream.close();
                temp_ifstream.clear();
            }

            else if(temp_string == "memory_allocate"){ //When Command is Allocate
                status.command = "system call";
                status.mode = "kernel";
                temp_int = std::stoi(temp_process_name);
                status.process_running->allocation_id++;
                for(int i = 0; i < pvect.size(); i++){
                    pvect[i] = NULL;
                }
                for (int i = 0; i < temp_int; i++){
                    pvect[i] = new Page({ult_counter + i, &status.process_running->pid, "W", status.process_running->allocation_id});    
                }
                ult_counter = ult_counter + temp_int;
                for (int i = 0; i < pvect.size(); i++){
                    if(pvect[i] != NULL){
                        temp_vector.push_back(pvect[i]);
                    }
                }

                if(policy == "fifo"){
                    status = fifo(status, temp_vector);
                }
                else if(policy == "lru"){
                    status = lru(status, temp_vector);
                }
                else if(policy == "lfu"){
                    status = lfu(status, temp_vector);
                }
                else if(policy == "mfu"){
                    status = mfu(status, temp_vector);
                }

                temp_vector.clear();
                if(status.process_running->pid > rvirtual.size()){
                    rvirtual.push_back(pvect);
                }
                else{
                    for (int i = 0; i < pvect.size(); i++){
                        for (int j = 0; j < rvirtual[status.process_running->pid - 1].size(); j++){
                            if(rvirtual[status.process_running->pid - 1][j] == NULL){
                                rvirtual[status.process_running->pid - 1][j] = pvect[i];
                                break;
                            }
                        } 
                    }
                }
                status.process_ready.push_back(status.process_running);
                status.process_running = NULL;
            }

            else if(temp_string == "memory_release"){ //When Command is Release
                status.command = "system call";
                status.mode = "kernel";
                temp_int = std::stoi(temp_process_name);

                //Release Virtual Memory
                for (int i = 0; i < 32; i++){
                    if(rvirtual[status.process_running->pid - 1][i] != NULL){
                        if(rvirtual[status.process_running->pid - 1][i]->allocation_id == temp_int){
                            rvirtual[status.process_running->pid - 1][i]->authority = "W";
                            rvirtual[status.process_running->pid - 1][i] = NULL;
                            rpagetable[status.process_running->pid - 1][i] = NULL;
                        }
                    }
                }

                //Release Physical Memory
                for (int i = 0; i < status.physical_memory.page_vector.size(); i++){
                    if(status.physical_memory.page_vector[i] != NULL){
                        if(status.physical_memory.page_vector[i]->allocation_id == temp_int and status.physical_memory.page_vector[i]->process_id == &status.process_running->pid){
                            status.physical_memory.page_vector[i] = NULL;
                        }
                    }
                }
                status.process_ready.push_back(status.process_running);
                status.process_running = NULL;
            }

            else if(temp_string == "memory_read"){ //When Command is Read
                fault = true;
                temp_int = std::stoi(temp_process_name);
                for (int i = 0; i < status.physical_memory.page_vector.size(); i++){
                    if(status.physical_memory.page_vector[i] != NULL){
                        if(status.physical_memory.page_vector[i]->page_id == temp_int){
                            if(policy == "lru"){
                                status.physical_memory.order[i] = status.cycle;
                            }
                            else if(policy == "lfu" or policy == "mfu"){
                                status.physical_memory.order[i]++;
                            }
                            fault = false;
                            break;
                        }
                    }
                }
                if(fault){ //Fault Case
                    status.command = "fault";
                    status.mode = "kernel";
                    for (int i = 0; i < rvirtual[status.process_running->pid - 1].size(); i++){
                        if (rvirtual[status.process_running->pid - 1][i] != NULL){
                            if(rvirtual[status.process_running->pid - 1][i]->page_id == temp_int){
                                temp_page = rvirtual[status.process_running->pid - 1][i];
                            }
                        }
                    }

                    if(policy == "fifo"){
                        status = fifo(status, {temp_page});
                    }
                    else if(policy == "lru"){
                        status = lru(status, {temp_page});
                    }
                    else if(policy == "lfu"){
                        status = lfu(status, {temp_page});
                    }
                    else if(policy == "mfu"){
                        status = mfu(status, {temp_page});
                    }

                    temp_page = NULL;
                    status.process_ready.push_back(status.process_running);
                    status.process_running = NULL;
                }
                else{ //Not Fault Case
                    status.command = status.process_running->file[status.process_running->current_line];
                    status.process_running->current_line++;
                    status.mode = "user";
                }
            }

            else if(temp_string == "memory_write"){ //When Command is Write
                fault = false;
                temp_int = std::stoi(temp_process_name);
                for (int i = 0; i < status.physical_memory.page_vector.size(); i++){
                    if(status.physical_memory.page_vector[i] != NULL){
                        if(status.physical_memory.page_vector[i]->page_id == temp_int){
                            fault = true;
                            break;
                        }
                    }
                }
                if(fault){ //Fault Case
                    for (int i = 0; i < rvirtual[status.process_running->pid - 1].size(); i++){
                        if (rvirtual[status.process_running->pid - 1][i] != NULL){
                            if(rvirtual[status.process_running->pid - 1][i]->page_id == temp_int){
                                fault_index = i;
                                temp_page = rvirtual[status.process_running->pid - 1][i];
                            }
                        }
                    }
                    if(temp_page->authority == "W"){
                        status.command = status.process_running->file[status.process_running->current_line];
                        status.process_running->current_line++;
                        status.mode = "user";
                    }
                    else if (temp_page->authority == "R"){
                        status.command = "fault";
                        status.mode = "kernel";
                        temp_page = new Page({temp_int, &status.process_running->pid, "W", status.process_running->allocation_id});
                        rvirtual[status.process_running->pid - 1][fault_index] = temp_page;
                        
                        if(policy == "fifo"){
                            status = fifo(status, {temp_page});
                        }
                        else if(policy == "lru"){
                            status = lru(status, {temp_page});
                        }
                        else if(policy == "lfu"){
                            status = lfu(status, {temp_page});
                        }
                        else if(policy == "mfu"){
                            status = mfu(status, {temp_page});
                        }

                        for(int i = 0; i < rvirtual[status.process_running->ppid - 1].size(); i++){
                            if (rvirtual[status.process_running->ppid - 1][i] != NULL){
                                if(rvirtual[status.process_running->ppid - 1][i]->page_id == temp_int){
                                    rvirtual[status.process_running->ppid - 1][i]->authority = "W";
                                }
                            }
                        }

                        status.process_ready.push_back(status.process_running);
                        status.process_running = NULL;
                    }
                }
                else{ //Not Fault Case
                    for (int i = 0; i < rvirtual[status.process_running->pid - 1].size(); i++){
                        if (rvirtual[status.process_running->pid - 1][i] != NULL){
                            if(rvirtual[status.process_running->pid - 1][i]->page_id == temp_int){
                                temp_page = rvirtual[status.process_running->pid - 1][i];
                            }
                        }
                    }

                    if(policy == "fifo"){
                        status = fifo(status, {temp_page});
                    }
                    else if(policy == "lru"){
                        status = lru(status, {temp_page});
                    }
                    else if(policy == "lfu"){
                        status = lfu(status, {temp_page});
                    }
                    else if(policy == "mfu"){
                        status = mfu(status, {temp_page});
                    }

                    status.process_ready.push_back(status.process_running);
                    status.process_running = NULL;
                }
                fault = true;
                temp_page = NULL;
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
            for (int i = 0; i < rvirtual[status.process_terminated->pid - 1].size(); i++){
                if (rvirtual[status.process_terminated->pid - 1][i] != NULL){
                    rvirtual[status.process_terminated->pid - 1][i]->authority = "W";
                }
            }
            for (int i = 0; i < status.physical_memory.page_vector.size(); i++){
                if (status.physical_memory.page_vector[i] != NULL){
                    if(status.physical_memory.page_vector[i]->process_id == &status.process_terminated->pid){
                        status.physical_memory.page_vector[i] = NULL;
                    }
                }
            }
        }

        else if(status.command == "wait"){ //When Command is wait
            status.command = "system call";
            status.mode = "kernel";
            status.process_running->waiting_type = 'W';
            status.process_waiting.push_back(status.process_running);
            status.process_running = NULL;
        }

        //Page Table
        if (rpagetable.size() < rvirtual.size()){
            rpagetable.push_back(ptvect);
        }

        //Check Page Table by Comparing Physical Memory and Virtual Memory
        if (status.process_running != NULL and rvirtual.size() > 0){
            for (int i = 0; i < rvirtual[status.process_running->pid - 1].size(); i++){
                if(rvirtual[status.process_running->pid - 1][i] != NULL){
                    temp_index = get_index(status.physical_memory.page_vector, rvirtual[status.process_running->pid - 1][i]);
                    rpagetable[status.process_running->pid - 1][i] = rvirtual[status.process_running->pid - 1][i];
                    rpagetable[status.process_running->pid - 1][i]->index = temp_index;
                }
            }
        }
        
        //Increase status.cycle by One
        status.cycle++;
    }

    //Store the Last Status
    store_status(status, {}, {});

    //Output
    
    ofstream write_file;
    write_file.open("result");
    write_file << output_text << "";
    write_file.close();
    

    //Return
    return 0;
}