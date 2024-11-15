/*
 * Copyright (C) 2015, Syntacore Ltd.
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
/// @brief PMP init function
/// Syntacore SCR* infra
///
/// @copyright Copyright (C) 2015, Syntacore Ltd.
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///     http://www.apache.org/licenses/LICENSE-2.0
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.

#include "drivers/pmp.h"
#include "utils.h"

#if PLF_PMP_SUPPORT

#ifdef PLF_EARLY_MEM_MAP
const scr_mem_region_info mem_early_regions[] = {PLF_EARLY_MEM_MAP};
#endif // PLF_EARLY_MEM_MAP

void pmp_early_init(void)
{
#ifdef PLF_EARLY_MEM_MAP
    setup_pmp_regions(mem_early_regions, ARRAY_SIZE(mem_early_regions));
#endif // PLF_EARLY_MEM_MAP
}

void pmp_init(void)
{
#ifdef PLF_MEM_MAP
    setup_pmp_regions(mem_regions, mem_region_count);
#endif // PLF_MEM_MAP
}

#endif // PLF_PMP_SUPPORT

