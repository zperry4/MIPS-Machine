/*Zachary Perry
 *3/26/21
 *COSC 130: MIPS Machine Lab
 *
 * Purpose: Purpose of this assignment is to simulate a MIPS processor. 
 * The code will load a file with MIPS instructions inside, decode the instructions, execute them, and then store the output into the created "registers".
*/

#include <cstdio> 
#include <iostream> 
#include <iomanip>
#include <fstream> 
#include <sstream> 
#include <string>

using namespace std; 

const int  NUM_REGS = 32;
const string reg_names[] = 
{	
	"$zero",
	"$at",
	"$v0",
	"$v1",
	"$a0",
	"$a1",
	"$a2",
	"$a3",
	"$t0",
	"$t1",
	"$t2",
	"$t3",
	"$t4",
	"$t5",
	"$t6",
	"$t7",
	"$s0",
	"$s1",
	"$s2",
	"$s3",
	"$s4",
	"$s5",
	"$s6",
	"$s7",
	"$t8",
	"$t9",
	"$k0",
	"$k1",
	"$gp",
	"$sp",
	"$fp",
	"$ra"
};


//Global constants. They represent the different supported instructions.
const unsigned int ADDI_OP = 8; 
const unsigned int BEQ_OP  = 4; 
const unsigned int BNE_OP  = 5; 
const unsigned int ADD_OP  = 32;
const unsigned int AND_OP  = 36; 
const unsigned int OR_OP   = 37; 
const unsigned int SLL_OP  = 0; 
const unsigned int SLT_OP  = 42; 
const unsigned int SRA_OP  = 3; 
const unsigned int SRL_OP  = 2;

struct Machine { 
	uint32_t regs[NUM_REGS] = {0};  
	uint32_t pc = 0; 
	uint32_t num_instructions;	
	uint32_t *instructions;
	
	//Function used for reg command. 
	bool reg(const string &name ,unsigned int &val);

	//Function to run the instructions. 
	bool run_instructions();

	//Function to print out the registers;
	void Print_Registers();	
	
};



void Machine::Print_Registers(){
	 
	//For loop to iterate through reg_names / regs.
	//If statement will print a new line after every 4 registers. 
	for (int i = 0; i < 32; i++)
	{
		if (i % 4 == 0 && i > 0){
			cout << '\n';
		}
		
		//Outputs the register names, register values in hex, and the register values. 
		//Cast regs[i] as a signed int so it could properly print negative numbers. 
		//Formatting is all done using setw and left/right.
		cout <<	 reg_names[i] << ":" << setw(1) << left; 
		printf(" 0x%08x", regs[i]);
		cout << ' ' <<  "(" << setw(5) << right  << (int32_t)regs[i] << ")" << setw(3) << left; 
	}

	//Outputs the PC. 
	cout << '\n' << "PC : " << pc << endl; 
}



bool Machine::reg(const string &name , uint32_t &val) { 
	
	//For loop to iterate through the reg_names array in order to match the input name with a reg_name. 
	//Will also look for the correct values stored within the register and match them.
	for (int i = 0; i < NUM_REGS; i++){
			
		if (name == reg_names[i]){
				val = regs[i];
				return true; 
			}
				
		}
	return false; 
}



bool Machine::run_instructions(){
	

	//Used to make sure the PC is in range and valid. 
	//Will continue to run instructions as long as the pc >= the total number of instructions. 
	if ((pc / 4) >= num_instructions){
			cout << "No more instructions to run." << endl; 
			return false; 
		}


	//Indexes into the instructions array and grabs the instruction we are on. Stores it into variable. 
	unsigned int instr = instructions[pc / 4]; 


	//Shifts the instruction right by 26 and masks it in order to get the opcode (6bits); 
	//Initializes the sub opcode to 0. 
	unsigned int op_code = (instr >> 26) & 0x3f; 
	unsigned int sub_op_code = 0;


	//All of these variables are used for decoding the instructions. 
	//All are initialized to zero. (rs, rt, rd, shift amount (sa), immediate (im), offset)
	int rs = 0; 
	int rt = 0; 
	int rd = 0; 
	int sa = 0; 
	int im = 0;	
	int offset = 0;
	

	//These two variables were used to convert the immediate / offset into  signed 16 bit ints.  
	int im_2 = 0; 
	int offset_2 = 0;
	

	//Start of switch statement for instructions with opcodes. 
	switch (op_code){
			

			//Case zero represent the "SPECIAL" instructions. 
			//Will mask the instruction in order to determine the sub opcode (i.e. what instruction it is).
			case 0:	

				sub_op_code = instr & 0x3f; 
				
				//Imbedded switch statement for "SPECIAL" instructions. Based on sub opcode, will perform the corresponding instruction.
				switch (sub_op_code){ 
						

						//Case for ADD operation. Will shift / mask each set of bits within the instruction and save them into their corresponding variable. 
						//Then, uses each value to go into regs array. (regs stores actual value at each location)
						//Will then perform the operation and store the value into the proper register.
						case ADD_OP:
							rs = (instr >> 21) & 0x1f; 
							rt = (instr >> 16) & 0x1f; 
							rd = (instr >> 11) & 0x1f; 
							regs[rd] = regs[rs] + regs[rt];
							break;
						

						//Case for AND operation. Same thing as ADD, except it performs bitwise AND operation. 
						case AND_OP: 
							rs = (instr >> 21) & 0x1f; 
							rt = (instr >> 16) & 0x1f; 
							rd = (instr >> 11) & 0x1f; 
							regs[rd] = regs[rs] & regs[rt]; 
							break;
						

						//Case for OR operation. Also, same thing as ADD, except it performs bitwise OR operation.
						case OR_OP: 
							rs = (instr >> 21) & 0x1f; 
							rt = (instr >> 16) & 0x1f; 
							rd = (instr >> 11) & 0x1f;
							regs[rd] = regs[rs] | regs[rt]; 
							break;
						

						//Case for SLL (Shift Left Logical).
						//Will shift regs[rt] left by the shift amount (sa) and store into rd. 
						case SLL_OP:
							rt = (instr >> 16) & 0x1f; 
							rd = (instr >> 11) & 0x1f; 
							sa = (instr >> 6 ) & 0x1f; 
							regs[rd] = regs[rt] << sa; 
							break; 
						

						//Case for SLT (Set on Less Than). 
						//Shifts / masks everything same way as before, but now theres an if/else statement. 
						//If the value at regs[rs] < regs[rt], it will record the boolean result into regs[rd]. (True = 1. False = 0.)
						case SLT_OP: 
							rs = (instr >> 21) & 0x1f; 
							rt = (instr >> 16) & 0x1f; 
							rd = (instr >> 11) & 0x1f; 

							if ((regs[rs] < regs[rt])){
								regs[rd] = true;
								break;
							}
							else{
								regs[rd] = false; 
								break;
							}
						

						//Case for SRA (Shift Right Arithmetic). 
						//Shifts / masks everything same way as before, but now we must account for sign extension. 
						//In order to duplicate the sign bit when shifting regs[rt], must cast it as a 32bit signed int. 
						case SRA_OP: 
							rt = (instr >> 16) & 0x1f;
							rd = (instr >> 11) & 0x1f; 
							sa = (instr >> 6 ) & 0x1f;								
							regs[rd] = int32_t(regs[rt]) >> sa; 
							break; 
						

						//Case for SRL (Shift Right Logical).
						//Nothing fancy is required for this instruction. 
						//Will simply shift regs[rt] right by the shift amount (sa). 
						case SRL_OP: 
							rt = (instr >> 16) & 0x1f; 
							rd = (instr >> 11) & 0x1f; 
							sa = (instr >> 6 ) & 0x1f; 
							regs[rd] = regs[rt] >> sa; 
							break; 


						//Default case is used if an unknown/unsupported instruction is read. Will return the cout statement below.
						default:	
							cout << "Unknown Instruction....Continuing." << endl; 
							break;
					}

				break;	

			
			//Case for ADDI. 
			//Uses im_2 to store the result of masking the instruction by 0xffff.
			//Then, casts im_2 into a 16bit signed int and stores that into im. 
			//This then allows us to add a 16bit signed int immediate to regs[rs]. 
			case ADDI_OP:
				im_2 = instr & 0xffff; 
				im = (int16_t)im_2; 
				rs = (instr >> 21) & 0x1f; 
				rt = (instr >> 16) & 0x1f;
				regs[rt] = regs[rs] + im; 
				break; 
			

			//Case for BEQ (Branch on Equal).
			//For the offset, it will mask the instruction by 0xffff and then cast that as a signed 16bit int. 
			//Then, an if/else statement is used to check if the conditions are met. 
			//If regs[rs] == regs[rt], then it will multiply the offset by 4 and add it to the pc. Else, it will break. 
			case BEQ_OP:
				rs = (instr >> 21) & 0x1f; 
				rt = (instr >> 16) & 0x1f;	
				offset_2 = instr & 0xffff;
				offset = (int16_t)offset_2;
	
				if ((regs[rs] == regs[rt])){
					pc += (offset*4); 	
					break;
				}
					
				else{
					break;
				}


			//Case for BNE (Branch Not Equal). 
			//Same thing as BEQ, except the if/else statement condition is differerent; 
			//If regs[rs] != regs[rt], then it will multiply the offset by 4 and add it to the pc. Else, it will break. 
			case BNE_OP:
				rs = (instr >> 21) & 0x1f; 
				rt = (instr >> 16) & 0x1f; 
				offset_2 = instr & 0xffff;
				offset = (int16_t)offset_2;

				if ((regs[rs] != regs[rt])){
					pc += (offset*4); 
					break;
				}

				else{
					break;
				}
			

			//Default case is used if an unknown/unsupported instruction is read. Will return the cout statement below.
			default:
				cout << "Unknown Instruction....Continuing." << endl;
				break; 	
			}


	//This will make sure to constantly have the $zero register = 0, as it is in MIPS. 
	regs[0] = 0;


	//Will increment the pc after every instruction. Will return true as long as there are more instructions left to run. 
	pc += 4; 
	return true;					
}




int main(int argc, char* argv[]){
	
	//Three string variables. Will be used for string stream. 
	//Input_2 and fileName are specifically used for the load command. 
	string input;
	string input_2; 
	string fileName; 
	
	//Initializes a variable to access the struct with.
	//Also sets Machine pointer to nullptr. 
	Machine m; 
	Machine* mips_mach= nullptr;


	//While loop containing input commands.
	while(true){
			

			//Output / Input for the interface.
			//Initializes stringstream
			cout << "> ";
			getline(cin, input);
			istringstream iss(input);
			iss >> input_2; 
			iss >> fileName;	


			//Regs command calls the Print_Registers() function. 
			if(input == "regs"){

				if (mips_mach == nullptr){
					printf("No machine loaded \n");
				}	
			
				else{	
					m.Print_Registers();				
				}
			}


			//Reg command will take user input (meaning name of a register), call the reg function, and then output the value inside the register in both decimal and hex. 
			//Casts val as a signed int so its able to properly retrieve / print negatives.
			else if(input  == "reg"){

				if (mips_mach == nullptr){
					printf("No machine loaded \n");
				}


				else{
					string name; 		
					unsigned int val;
					getline(cin, name);	
				 
					m.reg(name, val);

					printf(" 0x%08x ", val);	
					cout << "( " << (int32_t)val << ")" <<  ' '; 
					cout << endl; 
				}	
			} 


			//Next command will call the run_instructions() function and run a single instruction at a time. 
			else if( input == "next"){

				if (mips_mach == nullptr){
					printf("No machine loaded \n");
				}


				else{		
					m.run_instructions();
				}
			}


			//Run command will call the run_instructions() function and run all of the remaining instruction. 
			//Does this through the use of a while loop. As long as pc/4 < num_instructions, will continue to execute instructions. 
			else if (input == "run"){

				if (mips_mach == nullptr){
					printf("No machine loaded \n");
				}


				while((m.pc/4) < m.num_instructions){
					m.run_instructions();
				}
			}


			//Load command will load a new file.
			else if( input_2  == "load"){


				//Creates a new machine and allocates the memory for it. 
				mips_mach = new Machine; 
				

				//Opens the input file as "rb" (read binary). 
				FILE *fin = fopen(fileName.c_str(), "rb"); 
				if (nullptr == fin){
					printf("Unable to open file \n");  
				} 


				//Will fseek to the end of the file and then ftell in order to get the number of bytes in the file. 
				//Then, fseeks back to the begining of the file. 
				fseek(fin, 0, SEEK_END); 
				int	f_tell = ftell(fin); 
				fseek(fin, 0, SEEK_SET);
				

				//Dividing the number of bytes found by 4 to get the total number of instructions.
				//Will then store this in the num_instructions variable in the struct. 
				m.num_instructions = f_tell / 4; 
				

				//Allocates the amount of memory needed to store the instructions into the instructions pointer array.
				//Will then read in all of the instructions from the file into the instructions pointer array.
				m.instructions = new unsigned int[m.num_instructions];
				fread(m.instructions, sizeof(unsigned int), m.num_instructions, fin); 
				

				fclose(fin);
			}	


			//If the input = quit, it will exit the program. 
			else if( input == "quit"){
				break;
			}

	}
	

	//Deletes all of the memory allocated.
	delete [] m.instructions;
	delete  mips_mach;
	return 0; 
}
