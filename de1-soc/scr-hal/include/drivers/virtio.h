/*
 * Copyright (C) 2020, Syntacore Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/// @file
/// @brief virtio console interface
/// Syntacore SCR* infra
///
/// @copyright Copyright (C) 2020, Syntacore Ltd.
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///     http://www.apache.org/licenses/LICENSE-2.0
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.

#ifndef SCR_BSP_VIRTIO_H
#define SCR_BSP_VIRTIO_H

#ifndef __ASSEMBLER__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void virtio_console_init(void);
void virtio_console_putchar(int ch);
int virtio_console_getchar(void);
void virtio_console_tx_flush(void);
void virtio_console_shutdown(void);

#ifdef PLF_VIRTIO_CONSOLE
// console i/o
static inline void plf_con_tx_flush(void)
{
    virtio_console_tx_flush();
}

static inline int plf_con_put(int ch)
{
    virtio_console_putchar(ch);

    return ch;
}

static inline int plf_con_getch_nowait(void)
{
    return virtio_console_getchar();
}

static inline void plf_con_init(void)
{
    virtio_console_init();
}
#endif // PLF_VIRTIO_CONSOLE

#ifdef __cplusplus
}
#endif
#endif // !__ASSEMBLER__
#endif // SCR_BSP_VIRTIO_H
