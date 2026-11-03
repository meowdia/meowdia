/* SPDX-FileCopyrightText: 2026 Jiffly */
/* SPDX-License-Identifier: MIT OR Apache-2.0 */

#include <meow/plugin.h>
#include <stddef.h>

#if !defined(__linux__) || UINTPTR_MAX != UINT64_MAX
#error "These layout checks require 64-bit Linux"
#endif

#define TYPE(t, size, alignment)                     \
    _Static_assert(sizeof(t) == (size), #t " size"); \
    _Static_assert(_Alignof(t) == (alignment), #t " alignment")
#define FIELD(t, member, offset) \
    _Static_assert(offsetof(t, member) == (offset), #t "." #member " offset")

TYPE(meow_status_t, 4, 4);
TYPE(size_t, 8, 8);
TYPE(meow_handle_t, 8, 8);
TYPE(meow_header_t, 8, 4);
FIELD(meow_header_t, struct_size, 0);
FIELD(meow_header_t, abi_revision, 4);
TYPE(meow_bytes_t, 16, 8);
FIELD(meow_bytes_t, data, 0);
FIELD(meow_bytes_t, size, 8);
TYPE(meow_buffer_t, 16, 8);
FIELD(meow_buffer_t, data, 0);
FIELD(meow_buffer_t, capacity, 8);
TYPE(meow_error_t, 32, 8);
FIELD(meow_error_t, buffer, 0);
FIELD(meow_error_t, size, 16);
FIELD(meow_error_t, required, 24);
TYPE(meow_capability_t, 24, 8);
FIELD(meow_capability_t, id, 0);
FIELD(meow_capability_t, revision, 16);
FIELD(meow_capability_t, reserved, 20);
TYPE(meow_requirement_t, 48, 8);
FIELD(meow_requirement_t, name, 0);
FIELD(meow_requirement_t, capability, 16);
FIELD(meow_requirement_t, flags, 40);
FIELD(meow_requirement_t, reserved, 44);
TYPE(meow_plugin_descriptor_t, 72, 8);
FIELD(meow_plugin_descriptor_t, header, 0);
FIELD(meow_plugin_descriptor_t, id, 8);
FIELD(meow_plugin_descriptor_t, version, 24);
FIELD(meow_plugin_descriptor_t, provides, 40);
FIELD(meow_plugin_descriptor_t, provides_count, 48);
FIELD(meow_plugin_descriptor_t, requirements, 56);
FIELD(meow_plugin_descriptor_t, requirements_count, 64);
TYPE(meow_host_api_t, 24, 8);
FIELD(meow_host_api_t, header, 0);
FIELD(meow_host_api_t, context, 8);
FIELD(meow_host_api_t, log, 16);
TYPE(meow_binding_t, 64, 8);
FIELD(meow_binding_t, name, 0);
FIELD(meow_binding_t, capability, 16);
FIELD(meow_binding_t, provider, 40);
FIELD(meow_binding_t, api, 48);
FIELD(meow_binding_t, api_size, 56);
FIELD(meow_binding_t, reserved, 60);
TYPE(meow_plugin_api_t, 64, 8);
FIELD(meow_plugin_api_t, header, 0);
FIELD(meow_plugin_api_t, describe, 8);
FIELD(meow_plugin_api_t, create, 16);
FIELD(meow_plugin_api_t, query_interface, 24);
FIELD(meow_plugin_api_t, bind, 32);
FIELD(meow_plugin_api_t, start, 40);
FIELD(meow_plugin_api_t, stop, 48);
FIELD(meow_plugin_api_t, destroy, 56);
_Static_assert(MEOW_ABI_REVISION == 1, "experimental revision");
_Static_assert(MEOW_OK == 0 && MEOW_ERROR_INTERNAL < 0, "signed status values");
