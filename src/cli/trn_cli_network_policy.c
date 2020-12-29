// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file trn_cli_net.c
 * @author Catherine Lu (@clu2)
 *         
 * @brief CLI subcommands related to networks
 *
 * @copyright Copyright (c) 2020 The Authors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "trn_cli.h"

int trn_cli_update_transit_network_policy_subcmd(CLIENT *clnt, int argc, char *argv[])
{
	ketopt_t om = KETOPT_INIT;
	struct cli_conf_data_t conf;
	cJSON *json_str = NULL;

	if (trn_cli_read_conf_str(&om, argc, argv, &conf)) {
		return -EINVAL;
	}

	char *buf = conf.conf_str;
	json_str = trn_cli_parse_json(buf);
	if (json_str == NULL) {
		return -EINVAL;
	}
	int counter = cJSON_GetArraySize(json_str); 

	int *rc;
	struct rpc_trn_vsip_cidr_t cidr_list[counter];
	char rpc[] = "update_transit_network_policy_1";

	for (int i = 0; i < counter; i++)
	{
		struct rpc_trn_vsip_cidr_t cidrval;
		cidrval.interface = conf.intf;
		cidrval.count = counter;
		cJSON *policy = cJSON_GetArrayItem(json_str, i);
		int err = trn_cli_parse_network_policy_cidr(policy, &cidrval);

		if (err != 0) {
			print_err("Error: parsing network policy config.\n");
			return -EINVAL;
		}

		cidr_list[i] = cidrval;
	}
	cJSON_Delete(json_str);

	rc = update_transit_network_policy_1(cidr_list, clnt);
	if (rc == (int *)NULL) {
		print_err("RPC Error: client call failed: update_transit_network_policy_1.\n");
		return -EINVAL;
	}

	if (*rc != 0) {
		print_err(
			"Error: %s fatal daemon error, see transitd logs for details.\n",
			rpc);
		return -EINVAL;
	}

	for (int k = 0; k < counter; k++)
	{
		dump_network_policy(&cidr_list[k]);
	}
	print_msg("update_transit_network_policy_1 successfully updated network policy\n");
	
	return 0;
}

int trn_cli_delete_transit_network_policy_subcmd(CLIENT *clnt, int argc, char *argv[])
{
	ketopt_t om = KETOPT_INIT;
	struct cli_conf_data_t conf;
	cJSON *json_str = NULL;

	if (trn_cli_read_conf_str(&om, argc, argv, &conf)) {
		return -EINVAL;
	}

	char *buf = conf.conf_str;
	json_str = trn_cli_parse_json(buf);
	if (json_str == NULL) {
		return -EINVAL;
	}
	int counter = cJSON_GetArraySize(json_str); 

	int *rc;
	struct rpc_trn_vsip_cidr_key_t cidrkeys[counter];
	char rpc[] = "delete_transit_network_policy_1";

	for (int i = 0; i < counter; i++)
	{
		struct rpc_trn_vsip_cidr_key_t cidrkey;
		cidrkey.interface = conf.intf;
		cidrkey.count = counter;
		cJSON *policy = cJSON_GetArrayItem(json_str, i);
		int err = trn_cli_parse_network_policy_cidr_key(json_str, &cidrkey);

		if (err != 0) {
			print_err("Error: parsing network policy config.\n");
			return -EINVAL;
		}
		cidrkeys[i] = cidrkey;
	}
	cJSON_Delete(json_str);

	rc = delete_transit_network_policy_1(cidrkeys, clnt);
	if (rc == (int *)NULL) {
		print_err("RPC Error: client call failed: delete_transit_network_policy_1.\n");
		return -EINVAL;
	}

	if (*rc != 0) {
		print_err(
			"Error: %s fatal daemon error, see transitd logs for details.\n",
			rpc);
		return -EINVAL;
	}

	print_msg("delete_transit_network_policy_1 successfully deleted network policy cidr.\n");

	return 0;
}

int trn_cli_update_transit_network_policy_enforcement_subcmd(CLIENT *clnt, int argc, char *argv[])
{
	ketopt_t om = KETOPT_INIT;
	struct cli_conf_data_t conf;
	cJSON *json_str = NULL;

	if (trn_cli_read_conf_str(&om, argc, argv, &conf)) {
		return -EINVAL;
	}

	char *buf = conf.conf_str;
	json_str = trn_cli_parse_json(buf);

	if (json_str == NULL) {
		return -EINVAL;
	}

	int *rc;
	struct rpc_trn_vsip_enforce_t enforce;
	char rpc[] = "update_transit_network_policy_enforcement_1";
	enforce.interface = conf.intf;

	int err = trn_cli_parse_network_policy_enforcement(json_str, &enforce);
	cJSON_Delete(json_str);

	if (err != 0) {
		print_err("Error: parsing network policy enforcement config.\n");
		return -EINVAL;
	}

	rc = update_transit_network_policy_enforcement_1(&enforce, clnt);
	if (rc == (int *)NULL) {
		print_err("RPC Error: client call failed: update_transit_network_policy_enforcement_1 for local ip: 0x%x .\n",
					enforce.local_ip);
		return -EINVAL;
	}

	if (*rc != 0) {
		print_err(
			"Error: %s fatal daemon error, see transitd logs for details.\n",
			rpc);
		return -EINVAL;
	}

	dump_enforced_policy(&enforce);
	print_msg("update_transit_network_policy_enforcement_1 successfully updated network policy for local ip: 0x%x for interface %s \n",
				enforce.local_ip, enforce.interface);

	return 0;
}

int trn_cli_delete_transit_network_policy_enforcement_subcmd(CLIENT *clnt, int argc, char *argv[])
{
	ketopt_t om = KETOPT_INIT;
	struct cli_conf_data_t conf;
	cJSON *json_str = NULL;

	if (trn_cli_read_conf_str(&om, argc, argv, &conf)) {
		return -EINVAL;
	}

	char *buf = conf.conf_str;
	json_str = trn_cli_parse_json(buf);

	if (json_str == NULL) {
		return -EINVAL;
	}

	int *rc;
	struct rpc_trn_vsip_enforce_t enforce;
	char rpc[] = "delete_transit_network_policy_enforcement_1";
	enforce.interface = conf.intf;

	int err = trn_cli_parse_network_policy_enforcement(json_str, &enforce);
	cJSON_Delete(json_str);

	if (err != 0) {
		print_err("Error: parsing network policy enforcement config.\n");
		return -EINVAL;
	}

	rc = delete_transit_network_policy_enforcement_1(&enforce, clnt);
	if (rc == (int *)NULL) {
		print_err("RPC Error: client call failed: delete_transit_network_policy_enforcement_1 for local ip: 0x%x.\n",
					enforce.local_ip);
		return -EINVAL;
	}

	if (*rc != 0) {
		print_err(
			"Error: %s fatal daemon error, see transitd logs for details.\n",
			rpc);
		return -EINVAL;
	}

	dump_enforced_policy(&enforce);
	print_msg("delete_transit_network_policy_enforcement_1 successfully deleted network policy for local ip: 0x%x for interface %s\n",
					enforce.local_ip, enforce.interface);

	return 0;
}

int trn_cli_update_transit_network_policy_protocol_port_subcmd(CLIENT *clnt, int argc, char *argv[])
{
	ketopt_t om = KETOPT_INIT;
	struct cli_conf_data_t conf;
	cJSON *json_str = NULL;

	if (trn_cli_read_conf_str(&om, argc, argv, &conf)) {
		return -EINVAL;
	}

	char *buf = conf.conf_str;
	json_str = trn_cli_parse_json(buf);

	if (json_str == NULL) {
		return -EINVAL;
	}

	int *rc;
	struct rpc_trn_vsip_ppo_t ppo;
	char rpc[] = "update_transit_network_policy_protocol_port_1";
	ppo.interface = conf.intf;

	int err = trn_cli_parse_network_policy_protocol_port(json_str, &ppo);
	cJSON_Delete(json_str);

	if (err != 0) {
		print_err("Error: parsing network policy protocol port config.\n");
		return -EINVAL;
	}

	rc = update_transit_network_policy_protocol_port_1(&ppo, clnt);
	if (rc == (int *)NULL) {
		print_err("RPC Error: client call failed: update_transit_network_policy_protocol_port_1 for local ip: 0x%x for protocol %d port %d.\n",
					ppo.local_ip, ppo.proto, ppo.port);
		return -EINVAL;
	}

	if (*rc != 0) {
		print_err(
			"Error: %s fatal daemon error, see transitd logs for details.\n",
			rpc);
		return -EINVAL;
	}

	dump_protocol_port_policy(&ppo);
	print_msg("update_transit_network_policy_protocol_port_1 successfully updated network policy for local ip: 0x%x for interface %s \n",
				ppo.local_ip, ppo.interface);

	return 0;
}

int trn_cli_delete_transit_network_policy_protocol_port_subcmd(CLIENT *clnt, int argc, char *argv[])
{
	ketopt_t om = KETOPT_INIT;
	struct cli_conf_data_t conf;
	cJSON *json_str = NULL;

	if (trn_cli_read_conf_str(&om, argc, argv, &conf)) {
		return -EINVAL;
	}

	char *buf = conf.conf_str;
	json_str = trn_cli_parse_json(buf);

	if (json_str == NULL) {
		return -EINVAL;
	}

	int *rc;
	struct rpc_trn_vsip_ppo_key_t ppo_key;
	char rpc[] = "delete_transit_network_policy_protocol_port_1";
	ppo_key.interface = conf.intf;

	int err = trn_cli_parse_network_policy_protocol_port_key(json_str, &ppo_key);
	cJSON_Delete(json_str);

	if (err != 0) {
		print_err("Error: parsing network policy protocol port config.\n");
		return -EINVAL;
	}

	rc = delete_transit_network_policy_protocol_port_1(&ppo_key, clnt);
	if (rc == (int *)NULL) {
		print_err("RPC Error: client call failed: delete_transit_network_policy_protocol_port_1 for local ip: 0x%x for protocol %d port %d.\n",
					ppo_key.local_ip, ppo_key.proto, ppo_key.port);
		return -EINVAL;
	}

	if (*rc != 0) {
		print_err(
			"Error: %s fatal daemon error, see transitd logs for details.\n",
			rpc);
		return -EINVAL;
	}

	print_msg("delete_transit_network_policy_protocol_port_1 successfully deleted network policy for local ip: 0x%x for interface %s \n",
				ppo_key.local_ip, ppo_key.interface);

	return 0;
}

void dump_network_policy(struct rpc_trn_vsip_cidr_t *policy)
{
	print_msg("Interface: %s\n", policy->interface);
	print_msg("Tunnel ID: %ld\n", policy->tunid);
	print_msg("Local IP: %x\n", policy->local_ip);
	print_msg("CIDR Prefix: %d\n", policy->cidr_prefixlen);
	print_msg("CIDR IP: %x\n", policy->cidr_ip);
	print_msg("CIDR Type: %d\n", policy->cidr_type);
	print_msg("bit value: %ld\n", policy->bit_val);
	print_msg("counter: %d\n", policy->count);
}

void dump_enforced_policy(struct rpc_trn_vsip_enforce_t *enforce)
{
	print_msg("Interface: %s\n", enforce->interface);
	print_msg("Tunnel ID: %ld\n", enforce->tunid);
	print_msg("Local IP: %x\n", enforce->local_ip);
}

void dump_protocol_port_policy(struct rpc_trn_vsip_ppo_t *ppo)
{
	print_msg("Interface: %s\n", ppo->interface);
	print_msg("Tunnel ID: %ld\n", ppo->tunid);
	print_msg("Local IP: %x\n", ppo->local_ip);
	print_msg("Protocol: %d\n", ppo->proto);
	print_msg("Port: %x\n", ppo->port);
	print_msg("bit value: %ld\n", ppo->bit_val);
}