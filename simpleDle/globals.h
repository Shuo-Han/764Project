#include <map>
#include <shared_mutex>
#include "lockManager.h"
namespace littleBadger{
    extern std::map<int, LockWrapper> lockMap;
    extern std::map<int, std::string> mapStructure;
    
}
