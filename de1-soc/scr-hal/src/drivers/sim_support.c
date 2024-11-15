/*
 * Copyright (C) 2022, Syntacore Ltd.
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
/// @brief implementation of sim console
/// Syntacore SCR* infra
///
/// @copyright Copyright (C) 2022, Syntacore Ltd.
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///     http://www.apache.org/licenses/LICENSE-2.0
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.

#include "arch.h"
#include "drivers/uart_sim.h"

#ifdef PLF_SC_SIM_EXIT
#include <stddef.h>

void sim_halt_handler(int ret_code)
{
    asm volatile(".global sc_exit; sc_exit: nop");
    *(volatile size_t*)PLF_SC_SIM_EXIT = ret_code;
    ifence();
}
#endif // PLF_SC_SIM_EXIT
