#include "dvi_serialiser.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"

// This is the 10-bit TMDS serialiser for DVI. It uses a PIO state machine
// to output the TMDS symbols at the correct rate.

// The PIO program is defined in dvi_serialiser.pio

void dvi_serialiser_init(struct dvi_serialiser_cfg *cfg) {
	// Claim a free state machine on the given PIO
	cfg->sm_tmds[0] = pio_claim_unused_sm(cfg->pio, true);
	cfg->sm_tmds[1] = pio_claim_unused_sm(cfg->pio, true);
	cfg->sm_tmds[2] = pio_claim_unused_sm(cfg->pio, true);

	// Load the TMDS serialiser program
	uint offset = pio_add_program(cfg->pio, &dvi_serialiser_program);
	cfg->prog_offs = offset;

	// Configure the pins
	for (int i = 0; i < N_TMDS_LANES; ++i) {
		gpio_set_function(cfg->pins_tmds[i], GPIO_FUNC_PIO0 + (cfg->pio == pio0 ? 0 : 1));
	}
	gpio_set_function(cfg->pins_clk, GPIO_FUNC_PIO0 + (cfg->pio == pio0 ? 0 : 1));

	// Initialize the state machines
	for (int i = 0; i < N_TMDS_LANES; ++i) {
		dvi_serialiser_program_init(cfg->pio, cfg->sm_tmds[i], offset, cfg->pins_tmds[i], 1, cfg->invert_diffpairs);
	}
}

void dvi_serialiser_enable(struct dvi_serialiser_cfg *cfg, bool enable) {
	for (int i = 0; i < N_TMDS_LANES; ++i) {
		pio_sm_set_enabled(cfg->pio, cfg->sm_tmds[i], enable);
	}
}

uint32_t dvi_single_to_diff(uint32_t in) {
	// Convert single-ended to differential (for debug)
	return (in << 1) | (~in & 1);
}
