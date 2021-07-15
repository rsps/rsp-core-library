#include <sstream>
#include <version.h>

using namespace std;

string get_library_version() {
    stringstream ss;

    ss << LIB_VERSION_MAJOR << "." << LIB_VERSION_MINOR << ".0";

    return ss.getstring();
}
