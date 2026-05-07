#ifndef _DVI_SERIALISER_H
#define _DVI_SERIALISER_H

#include "dvi_config_defs.h"
#include "hardware/pio.h"

// Public API
void dvi_serialiser_init(struct dvi_serialiser_cfg *cfg);
void dvi_serialiser_enable(struct dvi_serialiser_cfg *cfg, bool enable);
void dvi_serialiser_set_pio_program(struct dvi_serialiser_cfg *cfg, const pio_program_t *program);

// Internal functions
void dvi_serialiser_program_init(PIO pio, uint sm, uint offset, uint pin_base, uint num_pins, uint clk_pio);

#endif
