/*
	Copyright (C) 2014 CurlyMo & lvdp

	This file is part of pilight.

	pilight is free software: you can redistribute it and/or modify it under the
	terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later
	version.

	pilight is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
	A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with pilight. If not, see	<http://www.gnu.org/licenses/>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../pilight.h"
#include "common.h"
#include "dso.h"
#include "log.h"
#include "protocol.h"
#include "hardware.h"
#include "binary.h"
#include "gc.h"
#include "arctech_dusk.h"

static void arctechDuskCreateMessage(int id, int unit, int state, int all) {
	arctech_dusk->message = json_mkobject();
	json_append_member(arctech_dusk->message, "id", json_mknumber(id, 0));
	if(all == 1) {
		json_append_member(arctech_dusk->message, "all", json_mknumber(all, 0));
	} else {
		json_append_member(arctech_dusk->message, "unit", json_mknumber(unit, 0));
	}

	if(state == 1) {
		json_append_member(arctech_dusk->message, "state", json_mkstring("dawn"));
	} else {
		json_append_member(arctech_dusk->message, "state", json_mkstring("dusk"));
	}
}

static void arctechDuskParseBinary(void) {
	int unit = binToDecRev(arctech_dusk->binary, 28, 31);
	int state = arctech_dusk->binary[27];
	int all = arctech_dusk->binary[26];
	int id = binToDecRev(arctech_dusk->binary, 0, 25);

	arctechDuskCreateMessage(id, unit, state, all);
}

#ifndef MODULE
__attribute__((weak))
#endif
void arctechDuskInit(void) {

	protocol_register(&arctech_dusk);
	protocol_set_id(arctech_dusk, "arctech_dusk");
	protocol_device_add(arctech_dusk, "kaku_dusk", "KlikAanKlikUit Dusk Sensor");
	protocol_plslen_add(arctech_dusk, 277);

	arctech_dusk->devtype = DUSK;
	arctech_dusk->hwtype = RF433;
	arctech_dusk->pulse = 4;
	arctech_dusk->rawlen = 132;
	arctech_dusk->lsb = 3;

	options_add(&arctech_dusk->options, 'u', "unit", OPTION_HAS_VALUE, DEVICES_ID, JSON_NUMBER, NULL, "^([0-9]{1}|[1][0-5])$");
	options_add(&arctech_dusk->options, 'i', "id", OPTION_HAS_VALUE, DEVICES_ID, JSON_NUMBER, NULL, "^([0-9]{1,7}|[1-5][0-9]{7}|6([0-6][0-9]{6}|7(0[0-9]{5}|10([0-7][0-9]{3}|8([0-7][0-9]{2}|8([0-5][0-9]|6[0-3]))))))$");
	options_add(&arctech_dusk->options, 't', "dusk", OPTION_NO_VALUE, DEVICES_STATE, JSON_STRING, NULL, NULL);
	options_add(&arctech_dusk->options, 'f', "dawn", OPTION_NO_VALUE, DEVICES_STATE, JSON_STRING, NULL, NULL);

	arctech_dusk->parseBinary=&arctechDuskParseBinary;
}

#ifdef MODULE
void compatibility(struct module_t *module) {
	module->name = "arctech_dusk";
	module->version = "1.2";
	module->reqversion = "5.0";
	module->reqcommit = "84";
}

void init(void) {
	arctechDuskInit();
}
#endif
