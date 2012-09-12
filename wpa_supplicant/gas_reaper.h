#ifndef _GAS_REAPER_H_
#define _GAS_REAPER_H_

struct wpa_supplicant;

int reaper_reap_initial_req(struct wpa_supplicant *wpa_s, const u8* sa, const u8* da, u8 dialog_token);

int reaper_reap_comeback_req(struct wpa_supplicant *wpa_s, const u8* sa, const u8* da, u8 dialog_token);

#endif
