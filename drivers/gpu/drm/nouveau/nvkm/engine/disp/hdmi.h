/* SPDX-License-Identifier: MIT */
#ifndef __NVKM_DISP_HDMI_H__
#define __NVKM_DISP_HDMI_H__
#include "ior.h"

struct packed_hdmi_infoframe {
	u32 dwords;
	u32 data[9];
};

void pack_hdmi_infoframe(struct packed_hdmi_infoframe *packed_frame,
			 u8 *raw_frame, ssize_t len);
#endif
