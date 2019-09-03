# Demo app for using the 'hass-mqtt' Mongoose-OS library

## Overview

This is demo app, serves as a skeleton for building Mongoose OS firmware for Home Assistant compatible devices.

## GET STARTED
Build up you device in few minutes following samples below. Mind that your device, like ESP8266 or ESP32, can host more than one of supported Home Assistant MQTT entity simultaneously.
### Make a BINARY SENSOR
#### 1. Configure your Home Assistant
Add following configuration lines to the `configuration.yaml` file.

    binary_sensor:
      - platform: mqtt
        name: "My first sensor"
        state_topic: "hass/things/MYDEVICE_01/my_first_sensor/state"
        availability_topic: "hass/things/MYDEVICE_01/availability"
        json_attributes_topic: "hass/things/MYDEVICE_01/my_first_sensor/attributes"
        device_class: door
#### 2. Implement your Mongoose-OS firmware
Include the library in your `mos.yml` file.

    libs:
      - origin: https://github.com/zendiy-mgos/hass-mqtt
Set device ID and MQTT settings in your `mos.yml` file.

    config_schema:
      - ["device.id", "MYDEVICE_01"]
      - ["mqtt.server", "<your_hass_ip_address>:8883"]
      - ["mqtt.user", "<your_hass_mqtt_broker_username>"]
      - ["mqtt.pass", "your_hass_mqtt_broker_password"]
      - ["mqtt.keep_alive", 15]
      - ["mqtt.will_topic", "hass/things/${device_id}/availability"]
      - ["mqtt.will_message", "offline"]
Set library settings in your `mos.yml` file.

    config_schema:
      - ["hass.mqtt.publish.attribs_topic", "hass/things/${device_id}/${object_id}/attributes"]
      - ["hass.mqtt.publish.state_topic", "hass/things/${device_id}/${object_id}/state"]
        # Publish binary-sensor value every 10 seconds
      - ["hass.publish.interval", 10000]
**C/C++ CODE**: write the `main.c` file if you are using C/C++ language for implementing your firmware.

	#include <stdbool.h> 
	#include "mgos.h"
	#include "mgos_hass_mqtt.h"
	
	enum ha_toggle_state sensor_state_read() {       
	  /* Read binary-sensor here and return ON,
	     OFF or UNKNOWN according sensor's readings */
	  return UNKNOWN;
	}
	
	bool on_state_get(HA_ENTITY_HANDLE handle,
	                  HA_ENTITY_BSTATE entity_state,
	                  void *user_data) {
	  (void) handle;
	  (void) user_data;

	  /* Read binary-sensor value */
	  enum ha_toggle_state state = sensor_state_read();
	
	  return mgos_hass_entity_bstate_set(entity_state, state, NULL);
	}
	
	enum mgos_app_init_result mgos_app_init(void) {
	  /* Set binary-sensor configurations */
	  ha_entity_cfg_t e = HA_ENTITY_CFG("my_first_sensor");   
	  ha_mqtt_bsensor_cfg_t cfg = MK_HA_MQTT_BSENSOR_CFG();	  
	  
	  /* Create and initialze 'binary_sensor.my_first_sensor' */ 
	  HA_ENTITY_HANDLE h = mgos_hass_bsensor_create(&e, &cfg);
	  if (h == NULL) return MGOS_APP_INIT_ERROR;	  
	  
	  mgos_hass_bsensor_on_state_get(h, on_state_get, NULL);
	  
	  return MGOS_APP_INIT_SUCCESS;
	}
**JavaScript CODE**: write the `init.js` file if you are implementing a JavaScript firmware.

	load('api_hass_mqtt.js');
	
	function sensorStateRead() {
	  /* Read binary-sensor here and return Hass.toggleState.ON,
	     Hass.toggleState.OFF or Hass.toggleState.UNKNOWN
	     according sensor's readings */
	  return Hass.toggleState.UNKNOWN;
	}
	
	/* Set binary-sensor configurations */
	let e = { object_id: "my_first_sensor" };
	
	/* Create and initialze 'binary_sensor.my_first_sensor' */
	let s = Hass.BSENSOR.create(e);
	if (s) {
	  s.onStateGet(function(handle, entity_state, userdata) {
	    let state = sensorStateRead();
	    return Hass.entityToggleStateSet(entity_state, state);
	  }, null);
	}
### Make a SENSOR
#### 1. Configure your Home Assistant
Add following configuration lines to the `configuration.yaml` file.

    sensor:
      - platform: mqtt
        name: "My first sensor"
        state_topic: "hass/things/MYDEVICE_01/my_first_sensor/state"
        availability_topic: "hass/things/MYDEVICE_01/availability"
        json_attributes_topic: "hass/things/MYDEVICE_01/my_first_sensor/attributes"
        unit_of_measurement: "%"
#### 2. Implement your Mongoose-OS firmware
Include the library in your `mos.yml` file.

    libs:
      - origin: https://github.com/zendiy-mgos/hass-mqtt
Set device ID and MQTT settings in your `mos.yml` file.

    config_schema:
      - ["device.id", "MYDEVICE_01"]
      - ["mqtt.server", "<your_hass_ip_address>:8883"]
      - ["mqtt.user", "<your_hass_mqtt_broker_username>"]
      - ["mqtt.pass", "your_hass_mqtt_broker_password"]
      - ["mqtt.keep_alive", 15]
      - ["mqtt.will_topic", "hass/things/${device_id}/availability"]
      - ["mqtt.will_message", "offline"]
Set library settings in your `mos.yml` file.

    config_schema:
      - ["hass.mqtt.publish.attribs_topic", "hass/things/${device_id}/${object_id}/attributes"]
      - ["hass.mqtt.publish.state_topic", "hass/things/${device_id}/${object_id}/state"]
        # Publish sensor value every 10 seconds
      - ["hass.publish.interval", 10000]
**C/C++ CODE**: write the `main.c` file if you are using C/C++ language for implementing your firmware.

	#include <stdbool.h> 
	#include "mgos.h"
	#include "mgos_hass_mqtt.h"
	
	double sensor_state_read() {       
	  /* Read sensor here and return measured value */
	  return 0.0;
	}
	
	bool on_state_get(HA_ENTITY_HANDLE handle,
	                  HA_ENTITY_XSTATE entity_state,
	                  void *user_data) {
	  (void) handle;
	  (void) user_data;

	  /* Read sensor value */
	  double state = sensor_state_read();
	
	  return mgos_hass_entity_fstate_set(entity_state, state, NULL);
	}
	
	enum mgos_app_init_result mgos_app_init(void) {
	  /* Set binary-sensor configurations */
	  ha_entity_cfg_t e = HA_ENTITY_CFG("my_first_sensor");   
	  ha_mqtt_sensor_cfg_t cfg = MK_HA_MQTT_SENSOR_CFG();	  
	  
	  /* Create and initialze 'sensor.my_first_sensor' */ 
	  HA_ENTITY_HANDLE h = mgos_hass_sensor_create(&e, &cfg);
	  if (h == NULL) return MGOS_APP_INIT_ERROR;	  
	  
	  mgos_hass_sensor_on_state_get(h, on_state_get, NULL);
	  
	  return MGOS_APP_INIT_SUCCESS;
	}
**JavaScript CODE**: write the `init.js` file if you are implementing a JavaScript firmware.

	load('api_hass_mqtt.js');
	
	function sensorStateRead() {
	  /* Read sensor here and return measured value */
	  return 0.0;
	}
	
	/* Set binary-sensor configurations */
	let e = { object_id: "my_first_sensor" };
	
	/* Create and initialze 'sensor.my_first_sensor' */
	let s = Hass.SENSOR.create(e);
	if (s) {
	  s.onStateGet(function(handle, entity_state, userdata) {
	    let state = sensorStateRead();
	    return Hass.entityXStateSet(entity_state, state);
	  }, null);
	}
### Make a SWITCH
#### 1. Configure your Home Assistant
Add following configuration lines to the `configuration.yaml` file.

    switch:
      - platform: mqtt
        name: "My first switch"
	command_topic: "hass/things/MYDEVICE_01/my_first_switch/cmd"
        state_topic: "hass/things/MYDEVICE_01/my_first_switch/state"
        availability_topic: "hass/things/MYDEVICE_01/availability"
        json_attributes_topic: "hass/things/MYDEVICE_01/my_first_switch/attributes"
#### 2. Implement your Mongoose-OS firmware
Include the library in your `mos.yml` file.

    libs:
      - origin: https://github.com/zendiy-mgos/hass-mqtt
Set device ID and MQTT settings in your `mos.yml` file.

    config_schema:
      - ["device.id", "MYDEVICE_01"]
      - ["mqtt.server", "<your_hass_ip_address>:8883"]
      - ["mqtt.user", "<your_hass_mqtt_broker_username>"]
      - ["mqtt.pass", "your_hass_mqtt_broker_password"]
      - ["mqtt.keep_alive", 15]
      - ["mqtt.will_topic", "hass/things/${device_id}/availability"]
      - ["mqtt.will_message", "offline"]
Set library settings in your `mos.yml` file.

    config_schema:
      - ["hass.mqtt.publish.attribs_topic", "hass/things/${device_id}/${object_id}/attributes"]
      - ["hass.mqtt.publish.state_topic", "hass/things/${device_id}/${object_id}/state"]
      - ["hass.mqtt.command.topic", "hass/things/${device_id}/${object_id}/cmd"]
**C/C++ CODE**: write the `main.c` file if you are using C/C++ language for implementing your firmware.

	#include <stdbool.h> 
	#include "mgos.h"
	#include "mgos_hass_mqtt.h"
	
	enum ha_toggle_state switch_state_read() {
	  /* Read switch state here and return ON,
	     OFF or UNKNOWN according its state */
	  return UNKNOWN;
	}
	
	bool on_state_get(HA_ENTITY_HANDLE handle,
	                  HA_ENTITY_BSTATE entity_state,
	                  void *user_data) {
	  (void) handle;
	  (void) user_data;

	  /* Read switch state */
	  enum ha_toggle_state state = switch_state_read();
	
	  return mgos_hass_entity_bstate_set(entity_state, state, NULL);
	}
	
	enum mgos_app_init_result mgos_app_init(void) {
	  /* Set switch configurations */
	  ha_entity_cfg_t e = HA_ENTITY_CFG("my_first_switch");   
	  ha_mqtt_switch_cfg_t cfg = MK_HA_MQTT_SWITCH_CFG();	  
	  
	  /* Create and initialze 'switch.my_first_switch' */ 
	  HA_ENTITY_HANDLE h = mgos_hass_switch_create(&e, &cfg);
	  if (h == NULL) return MGOS_APP_INIT_ERROR;	  
	  
	  mgos_hass_switch_on_state_get(h, on_state_get, NULL);
	  mgos_hass_switch_on_state_set(h, on_state_set, NULL);
	  
	  return MGOS_APP_INIT_SUCCESS;
	}
**JavaScript CODE**: write the `init.js` file if you are implementing a JavaScript firmware.

	load('api_hass_mqtt.js');
	
	function sensorStateRead() {
	  /* Read switch state here and return Hass.toggleState.ON,
	     Hass.toggleState.OFF or Hass.toggleState.UNKNOWN
	     according its state */
	  return Hass.toggleState.UNKNOWN;
	}
	
	/* Set switch configurations */
	let e = { object_id: "my_first_switch" };
	
	/* Create and initialze 'switch.my_first_switch' */
	let s = Hass.SWITCH.create(e);
	if (s) {
	  s.onStateGet(function(handle, entity_state, userdata) {
	    let state = sensorStateRead();
	    return Hass.entityToggleStateSet(entity_state, state);
	  }, null);
	}
