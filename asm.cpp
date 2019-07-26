#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <map> 
#include <stdio.h>
#include <stdlib.h>

using namespace std;

/*
 * Max file size in bytes.
 */
#define SIZE 256

static int totalLines = 0;
static map<string, int> ht;
static unsigned char toReturn[SIZE];
static int ctr, orgpain = 0;
static vector<string> output; 
static vector<string> al;

/**
* Bit Shift value.
*/    
static int SHIFT = 4;
    
/**
* binary opcodes 
*/
static unsigned short HLT = 0b00000000;
static unsigned short EXT = 0b0000;
static unsigned short LDA = 0b0001;
static unsigned short LDI = 0b0010;
static unsigned short STA = 0b0011;
static unsigned short STI = 0b0100;
static unsigned short ADD = 0b0101;
static unsigned short SUB = 0b0110;
static unsigned short JMP = 0b0111;
static unsigned short JMZ = 0b1000;
static unsigned short AND = 0b1001;
static unsigned short IOR = 0b1010;
static unsigned short XOR = 0b1011;
static unsigned short ADL = 0b1100;
static unsigned short ADC = 0b1101; 
static unsigned short SBB = 0b1110; 
static unsigned short NEG = 0b11110000;
static unsigned short COM = 0b11110001; 
static unsigned short CLR = 0b11110010;
static unsigned short SET = 0b11110011;
static unsigned short RTL = 0b11110100;
static unsigned short RTR = 0b11110101;
static unsigned short LSL = 0b11110110; 
static unsigned short LSR = 0b11110111;
static unsigned short ASR = 0b11111000;
static unsigned short TST = 0b11111001;
static unsigned short CLC = 0b11111010;
static unsigned short SEC = 0b11111011;
static unsigned short TCA = 0b11111100;
static unsigned short TVA = 0b11111101;
static unsigned short JAL = 0b11111110;
static unsigned short NOP = 0b11111111;

static unsigned short DAT = 0b100000000;
static unsigned short ORG = 0b100000001;

/*trim leading and trailing whitespace from string*/
string trimwhitespace(string str)
{
    str.erase(0, str.find_first_not_of(' '));       //prefixing spaces       
    str.erase(str.find_last_not_of(' ')+1);         //surfixing spaces    
    return str;
}

/*the two functions below work together to split a string, given a delimiting character*/
template<typename Out>
void split(const string &s, char delim, Out result) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        *(result++) = item;
    }
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, back_inserter(elems));
    return elems;
}

/**
* Cross check a string
* with the possible commands.
* @param s
  string to be checked.
* @return
  true if string matches an actual command.
  false otherwise.
*/
static bool crosscheckCmds(string s) {

    string possibleCmds[] = {"HLT", "EXT", "LDA", "LDI", "STA", "STI", "ADD", "SUB", "JMP", "JMZ", "AND", "IOR", "XOR", "ADL", "ADC", "SBB", "NEG", 
                              "COM", "CLR", "SET", "RTL", "RTR", "LSL", "LSR", "ASR", "TST", "CLC", "SEC", "TCA", "TVA", "JAL", "NOP", "DAT"};        
    
    for (int i = 0; i<33; i++) {
    	//cout << possibleCmds[i] << " being compared to " << s << "\n";
        if (possibleCmds[i].compare(s) == 0) {
        	//cout << possibleCmds[i] << " being compared to " << s << "\n";
            return true;
        }
    }
       
    return false;
}

//my version of strtol
static int mystoi(string s) {
    //use strtol with 0
    const char *s2 = s.c_str();
    char *end;
    int num = strtol(s2, &end, 0);
    return num; 
}

//to check if Org was encountered, and if it was encountered, update int orgpain
static bool isOrg(string s) {
    //cout << "in ORG " <<  "\n";
    string tocheck = trimwhitespace(s);
    vector<string> splitter4 = split(tocheck, ' ');  
    if (splitter4[0].compare("ORG") == 0) {
        //cout << "in ORG 2 " <<  "\n";
        string fororg = (trimwhitespace(splitter4[1]));
        //cout << "in ORG 3 " <<  "\n";
        if ((mystoi(fororg)) < SIZE) {
            orgpain = mystoi(fororg);
            return true;
        }       
    }
    return false;
}

//read file contents and populate vector and map
static int readFile(istream &InFile) {
		
	string line = "";	

	while (getline(InFile,line)) { //not sure if this works

		//cout << "this is line:" << line << "\n";
		//for(auto elem : ht)  {
		   //cout << "map element: " << elem.first << "\n";
		//}
		totalLines++;

        if (ctr >= SIZE) {
            fprintf(stderr, "The supplied program is too large. Exiting.\n");
            return 4;
        }
        
        //if ; is not the first character in line and line is bigger than 0
        if ((line.find(";") != 0) && (line.length() != 0)) {

        	//cout << "not an empty line\n";        	
        
            string line2 = trimwhitespace(line);
            //cout << "this is line2:" << line2 << "\n";

            vector<string> splitter0 = split(line2, ';'); 
            //cout << "splitter0" << "\n";

            vector<string> splitter = split(splitter0[0], ':'); 
            //cout << "splitter" << "\n";

            vector<string> splitter2 = split(line2, '\t');   
            //cout << "splitter2" << "\n";

            //cout << "this is splitter2[0]:" << splitter2[0] << "\n";
            //cout << "this is splitter2[1]:" << splitter2[1] << "\n";              
            //cout << "splitter3" << "\n";
                
            //label found
            if (splitter.size() > 1) {

                //cout << "label found " << "\n";            	

            	//if ht doesn't already have this label
                if (ht.find(trimwhitespace(splitter[0])) == ht.end()) {
                	//cout << "no duplicate label yet\n";
                    ht.insert({trimwhitespace(splitter[0]), ctr});
					//just testing
                } else {                	
                    fprintf(stderr, "Repeated label. Program exiting. \n");
                    return 7;
                }

                if (splitter[1].length() != 0) {
                    if(isOrg(splitter[1])) {
                        fprintf(stderr, "Invalid Label. ORG encountered at label. \n");
                        return 7;                      
                    }
                }

                vector<string> splitter3 = split(splitter2[1], ' ');  

                if (crosscheckCmds(trimwhitespace(splitter3[0]))) { 
                    //cout << "we got here too\n";   
                    al.push_back(trimwhitespace(splitter2[1]));     
                    ctr++; 
                }

                //problem area 
            } else if (line2.find(":") == string::npos)  {   

                vector<string> splitter3 = split(splitter2[1], ' ');           	
            	//cout << "we got here\n";
            	//cout << "This is splitter2[0]: " << splitter2[0] << "\n"; 
                //cout << "This is splitter2[1]: " << splitter2[1] << "\n";
                //cout << "This is splitter2[2]: " << splitter2[2] << "\n";
                if (splitter2.size() > 2) {
                    splitter2[1] = splitter2[1] + " " + splitter2[2];
                }
                if (!isOrg(splitter2[1])) {
                    if (crosscheckCmds(trimwhitespace(splitter3[0]))) { 
                        //cout << "we got here too\n";   
                        al.push_back(trimwhitespace(splitter2[1]));     
                        ctr++; 
            	    }  
                    else {
                        fprintf(stderr, "An invalid op-code was encountered. \n");
                        return 5; 
                    } 
                }             
            } else if (splitter.size() == 1) {
                    fprintf(stderr, "Empty label exception.\n");
                    return 7;
            }                        
            
            //ctr++;            

        }
        
    }

	return 0;

}


//replace label values with their corresponding counter values from the hashtable.
static int parseList(vector<string> al, map<string, int> ht2) {      
   
        
    for (int i = 0; i < ctr; i++) {
        vector<string> splits = split(al[i], ' ');
        //cout << "parselist instance " << i << " with al[i] " << al[i] << "\n"; 
        int var = 0;

        if (!(splits.size() < 2)) {
            //this is the case like LDA ret                                                                         
            if (ht2.find(trimwhitespace(splits[1])) != ht2.end()) { 
                var = ((ht2.find(trimwhitespace(splits[1])))->second);//splits[1] is ret
            } else {//this is the case like LDA 04
                try {
                    var = mystoi((trimwhitespace(splits[1])));//strtol - splits[1] is "04" 
                    if ((trimwhitespace(splits[0]).compare("ORG") == 0) || (trimwhitespace(splits[0]).compare("DAT") == 0)) {
                        if ((var < -128) || (var > 255)) {
                            fprintf(stderr, "An invalid argument was encountered. System exiting.\n");
                            return 6;
                        }                        
                    }
                    else {
                        if ((var < -8) || (var > 15)) {
                            fprintf(stderr, "An invalid argument was encountered. System exiting.\n");
                            return 6;
                        }  
                    }
                } catch (...) {
                    fprintf(stderr, "Label was never defined. System exiting.\n");
                    return 7;
                }                
            }
            
            if (trimwhitespace(splits[0]).compare("DAT") == 0) {
                output.push_back(trimwhitespace(splits[0]) + " " + splits[1]);
            } 
            else {
        	   output.push_back(trimwhitespace(splits[0]) + " " + to_string(var));//strtol
            }
        } else {
            output.push_back(trimwhitespace(splits[0]) + " ");
        }

        //cout << "vector output has " << output[i] << "\n"; 
    }

    return 0;
       
}


//Convert vector of assembly instructions to byte array for output.
static int convert(vector<string> code) {	

    unsigned char b;
    unsigned char c;

    for (int k = 0; k < orgpain; k++) {
        toReturn[k] = 0b00000000; 
    }
    int oldorgpain = orgpain;

    //cout << "convert part 1 reached \n";

	map<string, unsigned short> conv;
    conv.insert({"LDA", LDA});
    conv.insert({"LDI", LDI});
    conv.insert({"STA", STA});
    conv.insert({"STI", STI});
    conv.insert({"ADD", ADD});
    conv.insert({"SUB", SUB});
    conv.insert({"JMP", JMP});
    conv.insert({"JMZ", JMZ});
    conv.insert({"DAT", DAT});    
    conv.insert({"ORG", ORG}); 
    conv.insert({"HLT", HLT}); 
    conv.insert({"EXT", EXT}); 
    conv.insert({"AND", AND}); 
    conv.insert({"IOR", IOR}); 
    conv.insert({"XOR", XOR}); 
    conv.insert({"ADL", ADL}); 
    conv.insert({"ADC", ADC}); 
    conv.insert({"SBB", SBB}); 
    conv.insert({"NEG", NEG}); 
    conv.insert({"COM", COM}); 
    conv.insert({"CLR", CLR}); 
    conv.insert({"SET", SET}); 
    conv.insert({"RTL", RTL}); 
    conv.insert({"RTR", RTR}); 
    conv.insert({"LSL", LSL}); 
    conv.insert({"LSR", LSR}); 
    conv.insert({"ASR", ASR}); 
    conv.insert({"TST", TST}); 
    conv.insert({"CLC", CLC}); 
    conv.insert({"SEC", SEC}); 
    conv.insert({"TCA", TCA}); 
    conv.insert({"TVA", TVA}); 
    conv.insert({"JAL", JAL}); 
    conv.insert({"NOP", NOP}); 
    
    for (int j = 0; j < ctr; j++) {

        //cout << "convert part 2 reached, j is " << j << " \n";

        vector<string> splitter = split(code[j], ' '); //WRONG: splitting LDA 04 (don't worry about spaces, taken care of by parselist)

        if (!(splitter.size() < 2)) { //there is an argument with the opcode

            //cout << "convert part 3 reached \n";
            //cout << code[j] << " \n";

            unsigned short bytecode = ((conv.find(trimwhitespace(splitter[0]))->second));//bytecode = bytecode for LDA which is 0b0001


            if (((conv.find(splitter[0]))->first).compare("ORG") == 0) {

                isOrg(code[j]);

                for (int d = 0; d < orgpain; d++) {
                    toReturn[d+j+oldorgpain] = 0b00000000; 
                }

            }

            if (((conv.find(splitter[0]))->first).compare("EXT") == 0) { //EXT check
                if (mystoi(splitter[1]) == 0) {
                    fprintf(stderr, "EXT is 0.\n");
                    return 5; 
                }
            }

            //cout << (conv.find(splitter[0])->second) << " \n";

            if (0b00000000 == (conv.find(splitter[0])->second)) { //HLT check  
                if (((conv.find(splitter[0]))->first).compare("EXT") != 0) { //EXT check              
                    fprintf(stderr, "Invalid argument.\n");
                    return 6;     
                }            
            }

            if ((conv.find(splitter[0])->second) >= 0b11110000) { //opcode check 
                if ((conv.find(splitter[0])->second) != 0b100000000) {  
                     if ((conv.find(splitter[0])->second) != 0b100000001) {
                        fprintf(stderr, "Invalid argument.\n");
                        return 6;       
                    }    
                }      
            }

            if (mystoi(splitter[1]) >= SIZE) {
                fprintf(stderr, "Address is too large. System exiting.\n"); //checking if the value of 04 is greater than 256
                return 6;
            }        
            if ((bytecode == 0b100000000) || (bytecode == 0b00000000) || (bytecode > 0b1110)) {
                b = (0b11111111 & (mystoi(splitter[1]))); 
            }
            else {
                c = (mystoi(splitter[1]) & 0b00001111);//lower 4 bits
                b = ((bytecode << SHIFT) | c); //we can only take lower 4 bits of splitter[1]
            } 

        }
        else { //for opcodes without argument

            //cout << "convert part 4 reached \n";

            unsigned short bytecode = ((conv.find(splitter[0]))->second);//bytecode = bytecode for LDA which is 0b0001

            if (((conv.find(splitter[0]))->first).compare("EXT") == 0) { //EXT check                
                    fprintf(stderr, "EXT is invalid.\n");
                    return 5;                 
            }

             b = bytecode;

        }

        if (j+orgpain < SIZE) {
            toReturn[j+orgpain] = b;             
        }
        else {
            fprintf(stderr, "Address is too large. System exiting.\n"); 
            return 6;
        }
           
    }    

    for (int w = (ctr + oldorgpain + orgpain); w < SIZE; w++) {
        toReturn[w] = 0b00000000; 
    }        

    //cout << "convert part 5 reached \n";

    return 0; 
    
}


//Print state (registers and memory) of the SCRAM to standard output.
static void dump(unsigned char input[], FILE *fo, int c)
{	
    //cout << "dump reached \n";
	fwrite(input, 1, c, fo);	
}


int main(int argc, char * argv[]) {	

    int forreadfile, forparselist, forconvert;      

    if (argc > 3) {
        fprintf (stderr, "The wrong number of arguments were given.\n");
        return 1;
    }

    ifstream InFile;

    if (argc == 1) {        
        forreadfile = readFile(cin);
    }
    else {
        InFile.open(argv[1]); 
        if (!(InFile.is_open())) {  /* check file */
            fprintf (stderr, "The input file cannot be open or cannot be read. \n");
            return 2;
        }
    }                

    if (InFile.is_open()) {
    	forreadfile = readFile(InFile);
        if (forreadfile == 4) {
            return 4;
        }
        if (forreadfile == 7) {
            return 7;
        }
        if (forreadfile == 5) {
            return 5;
        }
        if (forreadfile == 8) {
            return 8;
        }
    	InFile.close();    	
    }	

    // for testing

    //for (int i = 0; i < al.size(); i++) {
    	//cout << al[i] << "\n";
    //}        

    //problems with new stuff
    forparselist = parseList(al, ht);
    if (forparselist == 7) {
        return 7;
    }   
    if (forparselist == 6) {
        return 6;
    }

    // for testing

    //for (int i = 0; i < output.size(); i++) {
        //cout << output[i] << "\n";
    //}    

    forconvert = convert(output);  
    if (forconvert == 6) {
        return 6;
    } 
    if (forconvert == 5) {
        return 5;
    }   

    //dump the 256 char array in its current state, if need be
    if (argc == 3) {
    	FILE *fp2 = fopen(argv[2], "w"); 
    	if (!fp2) {  // check file 
        	fprintf (stderr, "The output file cannot be opened or cannot be written. \n");
        	return 2;
   		}    	
   		dump(toReturn, fp2, SIZE);
    	fclose(fp2); 
    }
    else dump(toReturn, stdout, SIZE);  

	return 0;

}
