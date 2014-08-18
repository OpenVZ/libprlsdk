//
// Author: Roman Pen
//

#include <signal.h>

namespace google_breakpad {

bool WriteLightcore(const char* path_, pid_t pid_, const void* blob_, size_t cb_);

} // namespace google_breakpad
