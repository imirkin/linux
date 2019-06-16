// SPDX-License-Identifier: MIT
#include "hdmi.h"

void pack_hdmi_infoframe(struct packed_hdmi_infoframe *packed_frame,
			 u8 *raw_frame, ssize_t len)
{
	u8 buffer[32];
	int i;

	/* Make room for an extra byte in the header */
	memcpy(buffer + 1, raw_frame, len);
	memmove(&buffer[0], &buffer[1], 3);
	buffer[3] = 0;

	for (i = 0; i < DIV_ROUND_UP(len + 1, 4); i++)
		packed_frame->data[i] = get_unaligned_le32(&buffer[i * 4]);
	packed_frame->dwords = i;
	for (; i < ARRAY_SIZE(packed_frame->data); i++)
		packed_frame->data[i] = 0;
}
