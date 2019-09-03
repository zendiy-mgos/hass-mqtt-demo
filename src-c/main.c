/*
 * Copyright (c) 2019 ZenDIY
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdbool.h>
#include <math.h>
#include "mgos.h"
#include "mgos_gpio.h"
#include "mgos_hass_mqtt.h"

bool sw_on_state_set(HA_ENTITY_HANDLE handle, 
                     enum ha_toggle_state state,
                     void *user_data) {
  (void) handle;
  (void) user_data;

  bool gpio_value;
  switch(state) {
    case ON:
      gpio_value = mgos_sys_config_get_board_led1_active_high();
      break;
    case OFF:
      gpio_value = !mgos_sys_config_get_board_led1_active_high();
      break;
    default:
      return false;
  };
  mgos_gpio_write(mgos_sys_config_get_board_led1_pin(), gpio_value);
  return true;
}

bool sw_on_state_get(HA_ENTITY_HANDLE handle,
                     HA_ENTITY_BSTATE entity_state,
                     void *user_data) {
  (void) handle;
  (void) user_data;
  
  enum ha_toggle_state state;
  bool gpio_value = mgos_gpio_read(mgos_sys_config_get_board_led1_pin());
  if (mgos_sys_config_get_board_led1_active_high()) {
    state = (gpio_value ? ON : OFF);
  } else {
    state = (gpio_value ? OFF : ON);
  }

  size_t heap_size = mgos_get_heap_size();
  size_t free_heap_size = mgos_get_free_heap_size();
  int free_heap_size_p = round(((float)free_heap_size) / ((float)heap_size) * 100);
  int used_heap_size_p = 100 - free_heap_size_p;
  size_t total_space = mgos_vfs_get_space_total("/");
  size_t free_space = mgos_vfs_get_space_free("/");
  int free_space_p = round(((float)free_space) / ((float)total_space) * 100);
  int used_space_p = 100 - free_space_p;

  return mgos_hass_entity_bstate_setf(entity_state, state,
    "{ mem:{ total:%d, free:%d, free_p:%d, used_p:%d }, fs:{ total:%d, free:%d, free_p:%d, used_p:%d }, cpu:{ freq:%d }, sys_uptime:%f, device_id:%Q }",
    heap_size,
    free_heap_size,
    free_heap_size_p,
    used_heap_size_p,
    total_space,
    free_space,
    free_space_p,
    used_space_p,
    mgos_get_cpu_freq(),
    mgos_uptime(),
    mgos_sys_config_get_device_id());
}

bool hs_on_state_get(HA_ENTITY_HANDLE handle,
                     HA_ENTITY_XSTATE entity_state,
                     void *user_data) {
  (void) handle;
  (void) user_data;
  return mgos_hass_entity_fstate_set(entity_state, mgos_rand_range(0, 100), NULL);
}

bool ml_on_state_get(HA_ENTITY_HANDLE handle,
                     HA_ENTITY_XSTATE entity_state,
                     void *user_data) {
  (void) handle;
  (void) user_data;

  size_t heap_size = mgos_get_heap_size();
  size_t free_heap_size = mgos_get_free_heap_size();
  int free_heap_size_p = round(((float)free_heap_size) / ((float)heap_size) * 100);
  
  return mgos_hass_entity_dstate_set(entity_state, free_heap_size_p, NULL);
}

bool sp_on_state_get(HA_ENTITY_HANDLE handle,
                     HA_ENTITY_XSTATE entity_state,
                     void *user_data) {
  (void) handle;
  (void) user_data;

  enum ha_toggle_state state;
  bool gpio_value = mgos_gpio_read(mgos_sys_config_get_board_led1_pin());
  if (mgos_sys_config_get_board_led1_active_high()) {
    state = (gpio_value ? ON : OFF);
  } else {
    state = (gpio_value ? OFF : ON);
  }

  return mgos_hass_entity_sstate_setf(entity_state,
    (state == ON ? "OPEN" : "CLOSE"), "{gpio_value: %d}", gpio_value);
}


bool dr_on_state_get(HA_ENTITY_HANDLE handle,
                     HA_ENTITY_BSTATE entity_state,
                     void *user_data) {
  (void) handle;
  (void) user_data;

  enum ha_toggle_state state;
  bool gpio_value = mgos_gpio_read(mgos_sys_config_get_board_led1_pin());
  if (mgos_sys_config_get_board_led1_active_high()) {
    state = (gpio_value ? ON : OFF);
  } else {
    state = (gpio_value ? OFF : ON);
  }

  return mgos_hass_entity_bstate_set(entity_state, state, NULL);
}

enum mgos_app_init_result mgos_app_init(void) {
  mgos_gpio_setup_output(mgos_sys_config_get_board_led1_pin(),
    !mgos_sys_config_get_board_led1_active_high());

  /* Create and initialze switch.luce_atmosfera */
  ha_entity_cfg_t swe = HA_ENTITY_CFG("luce_atmosfera");
  ha_mqtt_switch_cfg_t sw_cfg = MK_HA_MQTT_SWITCH_CFG();
  sw_cfg.switch_cfg.inching_timeout = 5000;
  HA_ENTITY_HANDLE sw = mgos_hass_switch_create(&swe, &sw_cfg);
  if (sw != NULL) {
    mgos_hass_switch_on_state_set(sw, sw_on_state_set, NULL);
    mgos_hass_switch_on_state_get(sw, sw_on_state_get, NULL);
  }

  /* Create and initialze sensor.umidita_soggiorno */
  ha_entity_cfg_t hse = HA_ENTITY_CFG("umidita_soggiorno");
  ha_mqtt_sensor_cfg_t hs_cfg = MK_HA_MQTT_SENSOR_CFG();
  hs_cfg.sens_cfg.state_pubf = "%.1f";
  HA_ENTITY_HANDLE hs = mgos_hass_sensor_create(&hse, &hs_cfg);
  if (hs != NULL) {
    mgos_hass_sensor_on_state_get(hs, hs_on_state_get, NULL);
  }

  /* Create and initialze sensor.stato_porta */
  ha_entity_cfg_t spe = HA_ENTITY_CFG("stato_porta");
  ha_mqtt_sensor_cfg_t sp_cfg = MK_HA_MQTT_SENSOR_CFG();
  sp_cfg.sens_cfg.state_pubf = "%s";
  sp_cfg.pub_cfg.timer_timeout = 2000;
  HA_ENTITY_HANDLE sp = mgos_hass_sensor_create(&spe, &sp_cfg);
  if (sp != NULL) {
    mgos_hass_sensor_on_state_get(sp, sp_on_state_get, NULL);
  }

  /* Create and initialze sensor.memoria_libera */
  ha_entity_cfg_t mle = HA_ENTITY_CFG("memoria_libera");
  ha_mqtt_sensor_cfg_t ml_cfg = MK_HA_MQTT_SENSOR_CFG();
  ml_cfg.sens_cfg.state_pubf = "%d";
  ml_cfg.pub_cfg.timer_timeout = 10000;
  HA_ENTITY_HANDLE ml = mgos_hass_sensor_create(&mle, &ml_cfg);
  if (ml != NULL) {
    mgos_hass_sensor_on_state_get(ml, ml_on_state_get, NULL);
  }

  /* Create and initialze binary_sensor.porta_blindata */
  ha_entity_cfg_t dre = HA_ENTITY_CFG("porta_blindata");
  ha_mqtt_bsensor_cfg_t dr_cfg = MK_HA_MQTT_BSENSOR_CFG();
  dr_cfg.pub_cfg.timer_timeout = 2000;
  HA_ENTITY_HANDLE dr = mgos_hass_bsensor_create(&dre, &dr_cfg);
  if (dr != NULL) {
    mgos_hass_bsensor_on_state_get(dr, dr_on_state_get, NULL);
  }

  return MGOS_APP_INIT_SUCCESS;
}
