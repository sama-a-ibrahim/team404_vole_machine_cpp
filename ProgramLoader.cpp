#include "ProgramLoader.h"
#include <fstream>

LoadOutcome ProgramLoader :: LoadFromFile (const std:: string& path) const{
    std::ifstream file(path);
    if(!file. is_open()) {
        LoadOutcome out;
        out.status = LoadStatus:: FILE_NOT_FOUND;
        out.message = "Cannot open file" + path;
        return out;
    }
    return LoadFromStream(file);
}

LoadOutcome ProgramLoader :: LoadFromStream (std:: istream& in) const{
    LoadOutcome out;
    std:: string line;
    unsigned lineNo = 0 ;
    while (std :: getline(in,line)) {
        lineNo++ ;
        size_t semi = line.find(';');
        if (semi!= std :: string :: npos){
            line = line.substr(0 , semi);
        }

        size_t first = line.find_first_not_of(" \t\r\n");
        if(first == std:: string :: npos) continue;

        size_t last = line.find_last_not_of(" \t\r\n");
        line = line.substr(first , last-first +1);
        if(line[0] == ';') continue;

        auto instr = Instruction::fromHexString(line);
        if(!instr) {
            out.status = LoadStatus:: PARSE_ERROR;
            out.message = " Line " + std::to_string(lineNo) + " : invalid hex digits " + line ;
            out.line = lineNo;
            out.instructions.clear();
            return out;
        }

        if (!instr -> isValid()) {
            out.status = LoadStatus :: INVALID_INSTRUCTION;
            out.message = " Line " + std::to_string(lineNo) + " : invalid instruction " + line;
            out.line = lineNo;
            out.instructions.clear();
            return out;
        }
        out.instructions.push_back(*instr);
    }
    return out;
}