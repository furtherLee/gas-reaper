/*
 * Copyright (c) 2009, Atheros Communications
 * Copyright (c) 2011, Qualcomm Atheros
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 *
 * Derived from P2P service discovery (p2p_sd.c)
 */

#include "includes.h"

#include "common.h"
#include "eloop.h"
#include "common/ieee802_11_common.h"
#include "common/ieee802_11_defs.h"
#include "common/gas.h"
#include "common/wpa_ctrl.h"
#include "wpa_supplicant_i.h"
#include "driver_i.h"
#include "config.h"
#include "bss.h"
#include "gas_query.h"
#include "interworking.h"
#include "hs20_supplicant.h"


void wpas_hs20_add_indication(struct wpabuf *buf)
{
	wpabuf_put_u8(buf, WLAN_EID_VENDOR_SPECIFIC);
	wpabuf_put_u8(buf, 5);
	wpabuf_put_be24(buf, OUI_WFA);
	wpabuf_put_u8(buf, HS20_INDICATION_OUI_TYPE);
	wpabuf_put_u8(buf, 0x00); /* Hotspot Configuration */
}


struct wpabuf * hs20_build_anqp_req(u32 stypes, const u8 *payload,
				    size_t payload_len)
{
	struct wpabuf *buf;
	u8 *len_pos;

	buf = gas_anqp_build_initial_req(0, 100 + payload_len);
	if (buf == NULL)
		return NULL;

	len_pos = gas_anqp_add_element(buf, ANQP_VENDOR_SPECIFIC);
	wpabuf_put_be24(buf, OUI_WFA);
	wpabuf_put_u8(buf, HS20_ANQP_OUI_TYPE);
	if (stypes == BIT(HS20_STYPE_NAI_HOME_REALM_QUERY)) {
		wpabuf_put_u8(buf, HS20_STYPE_NAI_HOME_REALM_QUERY);
		if (payload)
			wpabuf_put_data(buf, payload, payload_len);
	} else {
		u8 i;
		wpabuf_put_u8(buf, HS20_STYPE_QUERY_LIST);
		for (i = 0; i < 32; i++) {
			if (stypes & BIT(i))
				wpabuf_put_u8(buf, i);
		}
	}
	gas_anqp_set_element_len(buf, len_pos);

	gas_anqp_set_len(buf);

	return buf;
}


int hs20_anqp_send_req(struct wpa_supplicant *wpa_s, const u8 *dst, u32 stypes,
		       const u8 *payload, size_t payload_len)
{
	struct wpabuf *buf;
	int ret = 0;
	int freq;
	struct wpa_bss *bss;
	int res;

	freq = wpa_s->assoc_freq;
	bss = wpa_bss_get_bssid(wpa_s, dst);
	if (bss)
		freq = bss->freq;
	if (freq <= 0)
		return -1;

	wpa_printf(MSG_DEBUG, "HS20: ANQP Query Request to " MACSTR " for "
		   "subtypes 0x%x", MAC2STR(dst), stypes);

	buf = hs20_build_anqp_req(stypes, payload, payload_len);
	if (buf == NULL)
		return -1;

	res = gas_query_req(wpa_s->gas, dst, freq, buf, anqp_resp_cb, wpa_s);
	if (res < 0) {
		wpa_printf(MSG_DEBUG, "ANQP: Failed to send Query Request");
		ret = -1;
	} else
		wpa_printf(MSG_DEBUG, "ANQP: Query started with dialog token "
			   "%u", res);

	wpabuf_free(buf);
	return ret;
}


void hs20_parse_rx_hs20_anqp_resp(struct wpa_supplicant *wpa_s,
				  const u8 *sa, const u8 *data, size_t slen)
{
	const u8 *pos = data;
	u8 subtype;
	struct wpa_bss *bss = wpa_bss_get_bssid(wpa_s, sa);

	if (slen < 1)
		return;

	subtype = *pos++;
	slen--;

	switch (subtype) {
	case HS20_STYPE_CAPABILITY_LIST:
		wpa_msg(wpa_s, MSG_INFO, "RX-HS20-ANQP " MACSTR
			" HS Capability List", MAC2STR(sa));
		wpa_hexdump_ascii(MSG_INFO, "HS Capability List", pos, slen);
		break;
	case HS20_STYPE_OPERATOR_FRIENDLY_NAME:
		wpa_msg(wpa_s, MSG_INFO, "RX-HS20-ANQP " MACSTR
			" Operator Friendly Name", MAC2STR(sa));
		wpa_hexdump_ascii(MSG_INFO, "oper friendly name", pos, slen);
		if (bss) {
			wpabuf_free(bss->hs20_operator_friendly_name);
			bss->hs20_operator_friendly_name =
				wpabuf_alloc_copy(pos, slen);
		}
		break;
	case HS20_STYPE_WAN_METRICS:
		wpa_msg(wpa_s, MSG_INFO, "RX-HS20-ANQP " MACSTR
			" WAN Metrics", MAC2STR(sa));
		wpa_hexdump_ascii(MSG_INFO, "WAN Metrics", pos, slen);
		if (bss) {
			wpabuf_free(bss->hs20_wan_metrics);
			bss->hs20_wan_metrics = wpabuf_alloc_copy(pos, slen);
		}
		break;
	case HS20_STYPE_CONNECTION_CAPABILITY:
		wpa_msg(wpa_s, MSG_INFO, "RX-HS20-ANQP " MACSTR
			" Connection Capability", MAC2STR(sa));
		wpa_hexdump_ascii(MSG_INFO, "conn capability", pos, slen);
		if (bss) {
			wpabuf_free(bss->hs20_connection_capability);
			bss->hs20_connection_capability =
				wpabuf_alloc_copy(pos, slen);
		}
		break;
	default:
		wpa_printf(MSG_DEBUG, "HS20: Unsupported subtype %u", subtype);
		break;
	}
}
