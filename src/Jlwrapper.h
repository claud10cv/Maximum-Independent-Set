#include "jlcxx/jlcxx.hpp"

static void max_indep_set(jlcxx::ArrayRef<int> _src, jlcxx::ArrayRef<int> _dst, jlcxx::ArrayRef<int> _sol);
JLCXX_MODULE define_julia_module(jlcxx::Module& mod);