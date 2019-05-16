#include "Utils.h"

#include <iomanip>
#include <string>
#include <sstream>

namespace Utils {
    std::string hexify(int number) {    
        std::stringbuf buf;
        std::ostream os(&buf);


        os << "0x" << std::setfill('0') << std::hex << number;

        return buf.str();
    }
}
