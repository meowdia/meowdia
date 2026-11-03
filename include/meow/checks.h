/* SPDX-FileCopyrightText: 2026 Jiffly */
/* SPDX-License-Identifier: MIT OR Apache-2.0 */

#ifndef MEOW_CHECKS_H
#define MEOW_CHECKS_H

#include <meow/plugin.h>

/* Optional, thread-safe helpers. They check sizes; callers must provide valid memory. */
meow_status_t meow_check_array(const void *data, size_t count, size_t element_size);
meow_status_t
meow_check_header(const void *table, uint32_t available, uint32_t minimum, uint32_t revision);
meow_status_t meow_check_plugin_api(const meow_plugin_api_t *api, uint32_t available);

/* Copy a message into error, truncating to fit. NULL error is allowed.
 * The message and destination must not overlap.
 */
meow_status_t meow_set_error(meow_error_t *error, const meow_bytes_t *message);

#endif
