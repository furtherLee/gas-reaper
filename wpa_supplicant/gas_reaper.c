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

int reaper_reap_initial_req(struct wpa_supplicant *wpa_s, const u8* sa, const u8* da, u8 dialog_token){

  // TODO
  return 0;
}

int reaper_reap_comeback_req(struct wpa_supplicant *wpa_s, const u8* sa, const u8* da, u8 dialog_token){

  // TODO
  return 0;
}
