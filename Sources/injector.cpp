#include "indCustomFunc.h"

extern "C" uint8_t* __stdcall mutate_self_text(size_t* out_len);

int main() {
	// 1. Load and decrypt the payload

	size_t szPayload = 0;
	uint8_t* payload = mutate_self_text(&szPayload);

	if (!payload || szPayload == 0) {
		errormsg("mutate_self_text() failed");
		return EXIT_FAILURE;
	}

	size_t

		// 2. Compute and patch values

		// 3. Inject into target
}