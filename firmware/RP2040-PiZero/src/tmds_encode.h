#ifndef _TMDS_ENCODE_H
#define _TMDS_ENCODE_H

#include <stdint.h>
#include "dvi_config_defs.h"

void tmds_encode_1bpp(const uint32_t *pixbuf, uint32_t *symbuf, uint32_t n_pix);
void tmds_encode_2bpp(const uint32_t *pixbuf, uint32_t *symbuf, uint32_t n_pix);
void tmds_encode_4bpp(const uint32_t *pixbuf, uint32_t *symbuf, uint32_t n_pix);
void tmds_encode_8bpp(const uint32_t *pixbuf, uint32_t *symbuf, uint32_t n_pix);
void tmds_encode_16bpp(const uint32_t *pixbuf, uint32_t *symbuf, uint32_t n_pix);

void tmds_encode_1bpp_reduced(const uint32_t *pixbuf, uint32_t *symbuf, uint32_t n_pix);
void tmds_encode_2bpp_reduced(const uint32_t *pixbuf, uint32_t *symbuf, uint32_t n_pix);
void tmds_encode_4bpp_reduced(const uint32_t *pixbuf, uint32_t *symbuf, uint32_t n_pix);
void tmds_encode_8bpp_reduced(const uint32_t *pixbuf, uint32_t *symbuf, uint32_t n_pix);
void tmds_encode_16bpp_reduced(const uint32_t *pixbuf, uint32_t *symbuf, uint32_t n_pix);

#endif
