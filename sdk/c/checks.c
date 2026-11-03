/* SPDX-FileCopyrightText: 2026 Jiffly */
/* SPDX-License-Identifier: MIT OR Apache-2.0 */

#include <meow/checks.h>

#include <stddef.h>
#include <string.h>

meow_status_t meow_check_array(const void *data, size_t count, size_t element_size) {
    if (element_size == 0 || count > SIZE_MAX / element_size || (count != 0 && data == NULL)) {
        return MEOW_ERROR_INVALID_ARGUMENT;
    }
    return MEOW_OK;
}

meow_status_t
meow_check_header(const void *table, uint32_t available, uint32_t minimum, uint32_t revision) {
    meow_header_t header;

    if (table == NULL || minimum < sizeof(header)) {
        return MEOW_ERROR_INVALID_ARGUMENT;
    }
    if (available < sizeof(header)) {
        return MEOW_ERROR_BUFFER_TOO_SMALL;
    }
    memcpy(&header, table, sizeof(header));
    if (header.abi_revision != revision) {
        return MEOW_ERROR_INCOMPATIBLE_ABI;
    }
    if (header.struct_size < minimum || header.struct_size > available) {
        return MEOW_ERROR_BUFFER_TOO_SMALL;
    }
    return MEOW_OK;
}

meow_status_t meow_check_plugin_api(const meow_plugin_api_t *api, uint32_t available) {
    meow_status_t status = meow_check_header(api, available, sizeof(*api), MEOW_ABI_REVISION);
    if (status != MEOW_OK) {
        return status;
    }
    if (api->describe == NULL || api->create == NULL || api->query_interface == NULL ||
        api->bind == NULL || api->start == NULL || api->stop == NULL || api->destroy == NULL) {
        return MEOW_ERROR_INVALID_ARGUMENT;
    }
    return MEOW_OK;
}

meow_status_t meow_set_error(meow_error_t *error, const meow_bytes_t *message) {
    if (error == NULL) {
        return MEOW_OK;
    }
    error->size = 0;
    error->required = 0;
    if (message == NULL || meow_check_array(message->data, message->size, 1) != MEOW_OK ||
        meow_check_array(error->buffer.data, error->buffer.capacity, 1) != MEOW_OK) {
        return MEOW_ERROR_INVALID_ARGUMENT;
    }
    error->required = message->size;
    error->size = message->size < error->buffer.capacity ? message->size : error->buffer.capacity;
    if (error->size != 0) {
        memcpy(error->buffer.data, message->data, error->size);
    }
    return MEOW_OK;
}
