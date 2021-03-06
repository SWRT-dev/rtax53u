/* header auto-generated by pidl */

#ifndef _PIDL_HEADER_named_pipe_auth
#define _PIDL_HEADER_named_pipe_auth

#include <stdint.h>

#include "libcli/util/ntstatus.h"

#include "librpc/gen_ndr/netlogon.h"
#include "librpc/gen_ndr/security.h"
#include "librpc/gen_ndr/auth.h"
#ifndef _HEADER_named_pipe_auth
#define _HEADER_named_pipe_auth

#define NAMED_PIPE_AUTH_MAGIC	( "NPAM" )
struct named_pipe_auth_req_info4 {
	const char *client_name;/* [charset(UTF8),unique] */
	const char *client_addr;/* [unique,charset(DOS)] */
	uint16_t client_port;
	const char *server_name;/* [unique,charset(UTF8)] */
	const char *server_addr;/* [charset(DOS),unique] */
	uint16_t server_port;
	struct auth_session_info_transport *session_info;/* [unique] */
};

union named_pipe_auth_req_info {
	struct named_pipe_auth_req_info4 info4;/* [case(4)] */
}/* [switch_type(uint32)] */;

struct named_pipe_auth_req {
	uint32_t length;/* [flag(LIBNDR_FLAG_BIGENDIAN),value(ndr_size_named_pipe_auth_req(r,ndr->flags)-4)] */
	const char *magic;/* [charset(DOS),value(NAMED_PIPE_AUTH_MAGIC)] */
	uint32_t level;
	union named_pipe_auth_req_info info;/* [switch_is(level)] */
}/* [gensize,public] */;

struct named_pipe_auth_rep_info4 {
	uint16_t file_type;
	uint16_t device_state;
	uint64_t allocation_size;
};

union named_pipe_auth_rep_info {
	struct named_pipe_auth_rep_info4 info4;/* [case(4)] */
}/* [switch_type(uint32)] */;

struct named_pipe_auth_rep {
	uint32_t length;/* [value(ndr_size_named_pipe_auth_rep(r,ndr->flags)-4),flag(LIBNDR_FLAG_BIGENDIAN)] */
	const char *magic;/* [value(NAMED_PIPE_AUTH_MAGIC),charset(DOS)] */
	uint32_t level;
	union named_pipe_auth_rep_info info;/* [switch_is(level)] */
	NTSTATUS status;
}/* [public,gensize] */;

#endif /* _HEADER_named_pipe_auth */
#endif /* _PIDL_HEADER_named_pipe_auth */
