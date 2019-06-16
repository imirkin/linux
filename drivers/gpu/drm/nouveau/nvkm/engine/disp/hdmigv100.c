/*
 * Copyright 2018 Red Hat Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
#include "hdmi.h"

void
gv100_hdmi_ctrl(struct nvkm_ior *ior, int head, bool enable, u8 max_ac_packet,
		u8 rekey, u8 *avi, u8 avi_size, u8 *vendor, u8 vendor_size,
		u8 *drm, u8 drm_size)
{
	struct nvkm_device *device = ior->disp->engine.subdev.device;
	const u32 ctrl = 0x40000000 * enable |
			 max_ac_packet << 16 |
			 rekey;
	const u32 hoff = head * 0x800;
	const u32 hdmi = head * 0x400;
	struct packed_hdmi_infoframe avi_infoframe;
	struct packed_hdmi_infoframe vendor_infoframe;
	struct packed_hdmi_infoframe drm_infoframe;
	int i;

	pack_hdmi_infoframe(&avi_infoframe, avi, avi_size);
	pack_hdmi_infoframe(&vendor_infoframe, vendor, vendor_size);
	pack_hdmi_infoframe(&drm_infoframe, drm, drm_size);

	if (!(ctrl & 0x40000000)) {
		nvkm_mask(device, 0x6165c0 + hoff, 0x40000000, 0x00000000);
		nvkm_mask(device, 0x6f0100 + hdmi, 0x00000001, 0x00000000);
		nvkm_mask(device, 0x6f00c0 + hdmi, 0x00000001, 0x00000000);
		nvkm_mask(device, 0x6f0000 + hdmi, 0x00000001, 0x00000000);
		return;
	}

	/* AVI InfoFrame (AVI). */
	nvkm_mask(device, 0x6f0000 + hdmi, 0x00000001, 0x00000000);
	if (avi_size) {
		for (i = 0; i < 5; i++)
			nvkm_wr32(device, 0x6f0008 + hdmi + i * 4,
				  avi_infoframe.data[i]);
		nvkm_mask(device, 0x6f0000 + hdmi, 0x00000001, 0x00000001);
	}

	/* GENERIC InfoFrame. */
	nvkm_mask(device, 0x6f0040 + hdmi, 0x00000001, 0x00000000);
	if (drm_size) {
		for (i = 0; i < 9; i++)
			nvkm_wr32(device, 0x6f0048 + hdmi + i * 4,
				  drm_infoframe.data[i]);
		nvkm_mask(device, 0x6f0040 + hdmi, 0x00000001, 0x00000001);
	}

	/* Vendor-specific InfoFrame (VSI). */
	nvkm_mask(device, 0x6f0100 + hdmi, 0x00010001, 0x00000000);
	if (vendor_size) {
		for (i = 0; i < 9; i++)
			nvkm_wr32(device, 0x6f0108 + hdmi + i * 4,
				  vendor_infoframe.data[i]);
		nvkm_mask(device, 0x6f0100 + hdmi, 0x00000001, 0x00000001);
	}


	/* General Control (GCP). */
	nvkm_mask(device, 0x6f00c0 + hdmi, 0x00000001, 0x00000000);
	nvkm_wr32(device, 0x6f00cc + hdmi, 0x00000010);
	nvkm_mask(device, 0x6f00c0 + hdmi, 0x00000001, 0x00000001);

	/* Audio Clock Regeneration (ACR). */
	nvkm_wr32(device, 0x6f0080 + hdmi, 0x82000000);

	/* NV_PDISP_SF_HDMI_CTRL. */
	nvkm_mask(device, 0x6165c0 + hoff, 0x401f007f, ctrl);
}
