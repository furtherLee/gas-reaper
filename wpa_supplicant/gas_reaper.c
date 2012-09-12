#include "includes.h"

#include "common.h"
#include "utils/eloop.h"
#include "common/ieee802_11_defs.h"
#include "common/gas.h"
#include "wpa_supplicant_i.h"
#include "driver_i.h"
#include "offchannel.h"
#include "gas_query.h"
#include "gas_reaper.h"

int reaper_reap_initial_req(struct wpa_supplicant *wpa_s, const u8* sa, const u8* da, u8 dialog_token, int freq){
  static const int len = 12;
  static const int dialog_pos = 1;
  u8 template[] = { 0x04, 0x0b, 0x00, 0x41, 0x00,
		    0x00, 0x6c, 0x02, 0x7f, 0x00,
		    0x00, 0x00 };
  template[dialog_pos] = dialog_token;
  wpa_printf(MSG_INFO, "Reap a STA " MACSTR " request to " MACSTR, MAC2STR(sa), MAC2STR(da));
  
  return wpa_s->driver->send_action(wpa_s->drv_priv, freq, 
				    0, sa, da, da, 
				    template, len, 1);
}

int reaper_reap_comeback_req(struct wpa_supplicant *wpa_s, const u8* sa, const u8* da, u8 dialog_token, int freq){

  // TODO
  return 0;
}
