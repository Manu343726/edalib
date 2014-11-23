#include "timing.hpp"

//Static fields definitions
std::vector<timing_manager::snapshot> timing_manager::timing_stack;
std::vector<timing_manager::snapshot> timing_manager::timing_registry;
std::function<void(const timing_manager::snapshot&)> timing_manager::_on_start;
std::function<void(const timing_manager::snapshot&)> timing_manager::_on_finish;
