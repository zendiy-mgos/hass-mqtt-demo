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

load('api_config.js');
load('api_gpio.js');
load('api_sys.js');
load('api_math.js');
load('api_hass_mqtt.js');

function relayStateGet() {
  let gpio_value = GPIO.read(Cfg.get('board.led1.pin'));
  if (Cfg.get('board.led1.active_high')) {
    return (gpio_value ? Hass.toggleState.ON : Hass.toggleState.OFF);
  } else {
    return (gpio_value ? Hass.toggleState.OFF : Hass.toggleState.ON);
  }
}

/* Initialize the LED GPIO */
GPIO.setup_output(Cfg.get('board.led1.pin'),
  !Cfg.get('board.led1.active_high'));


/* Create and initialze switch.luce_atmosfera */
let swe = { object_id: "luce_atmosfera" };
let sw_cfg = {
  switch_cfg: {
    inching_timeout: 5000
  }
};
let sw = Hass.SWITCH.create(swe, sw_cfg);
if (sw) {
  sw.onStateSet(function(handle, state, userdata) {
    let gpio_value;
    if (state === Hass.toggleState.ON) {
      gpio_value = Cfg.get('board.led1.active_high');
    } else if (state === Hass.toggleState.OFF) {
      gpio_value = !Cfg.get('board.led1.active_high');
    } else {
      return false;
    }
    GPIO.write(Cfg.get('board.led1.pin'), gpio_value);
    return true;
  }, null);

  sw.onStateGet(function(handle, entity_state, userdata) {
    let state = relayStateGet();
    let heap_size = Sys.total_ram();
    let free_heap_size = Sys.free_ram();
    let free_heap_size_p = Math.round(free_heap_size / heap_size * 100);
    let used_heap_size_p = 100 - free_heap_size_p;
    
    let atribs = {
      mem: {
        total: heap_size,
        free: free_heap_size,
        free_p: free_heap_size_p,
        used_p: used_heap_size_p
      },
      sys_uptime: Sys.uptime(),
      device_id: Cfg.get('device.id')
    };
    return Hass.entityToggleStateSet(entity_state, state,
      JSON.stringify(atribs));
  }, null);
}

/* Create and initialze sensor.umidita_soggiorno */
let hse = { object_id: "umidita_soggiorno" };
let hs_cfg = { sens_cfg: { state_pubf: "%.1f" } };
let hs = Hass.SENSOR.create(hse, hs_cfg);
if (hs) {
  hs.onStateGet(function(handle, entity_state, userdata) {
    return Hass.entityXStateSet(entity_state, (Math.random() * 100));
  }, null);
}

/* Create and initialze sensor.stato_porta */
let spe = { object_id: "stato_porta" };
let sp_cfg = {
  sens_cfg: { state_pubf: "%s" },
  pub_cfg: { timer_timeout: 2000 }
};
let sp = Hass.SENSOR.create(spe, sp_cfg);
if (sp) {
  sp.onStateGet(function(handle, entity_state, userdata) {
    let state = (relayStateGet() === Hass.toggleState.ON ? "OPEN" : "CLOSE");
    return Hass.entityXStateSet(entity_state, state,
      JSON.stringify({ free_ram: Sys.free_ram() }));
  }, null);
}

/* Create and initialze sensor.memoria_libera */
let mle = { object_id: "memoria_libera" };
let ml_cfg = {
  sens_cfg: { state_pubf: "%d" },
  pub_cfg: { timer_timeout: 10000 }
};
let ml = Hass.SENSOR.create(mle, ml_cfg);
if (ml) {
  ml.onStateGet(function(handle, entity_state, userdata) {   
    let heap_size = Sys.total_ram();
    let free_heap_size = Sys.free_ram();
    let free_heap_size_p = Math.round(free_heap_size / heap_size * 100);

    return Hass.entityXStateSet(entity_state, free_heap_size_p);
  }, null);
}

/* Create and initialze binary_sensor.porta_blindata */
let dre = { object_id: "porta_blindata" };
let dr_cfg = { pub_cfg: { timer_timeout: 2000 } };
let dr = Hass.BSENSOR.create(dre, dr_cfg);
if (dr) {
  let s = dr.onStateGet(function(handle, entity_state, userdata) {   
    let state = relayStateGet();
    return Hass.entityToggleStateSet(entity_state, state);
  }, null);
}