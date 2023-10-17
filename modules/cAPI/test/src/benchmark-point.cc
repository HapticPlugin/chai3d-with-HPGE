#include "benchpress.hpp"
#include "HPGE.h"
#include "chai3d.h"

BENCHMARK("example", [](benchpress::context* ctx) {
	for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		std::cout << "hello" << std::endl;
	}
})
