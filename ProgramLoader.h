#pragma once
#include <istream>
#include <string>
#include "LoadOutcome.h"

class ProgramLoader{
    public:
    LoadOutcome LoadFromFile(const std:: string& path) const;
    LoadOutcome LoadFromStream ( std:: istream& in) const;

};
