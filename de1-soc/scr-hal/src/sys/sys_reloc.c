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
/// @brief platform code/data relocation func
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

#include <sys/errno.h>
#include <string.h>

#include "memasm.h"

struct Plf_reloc_info {
    char *start;
    char *end;
    char *load;
};

extern char __relrodata_start[] __attribute__((weak));
extern char __relrodata_end[] __attribute__((weak));
extern char __relrodata_load_start[] __attribute__((weak));
extern char __reldata_start[] __attribute__((weak));
extern char __reldata_end[] __attribute__((weak));
extern char __reldata_load_start[] __attribute__((weak));

volatile const __initconst struct Plf_reloc_info ro_reloc = {
    __relrodata_start, __relrodata_end, __relrodata_load_start
};
volatile const __initconst struct Plf_reloc_info data_reloc = {
    __reldata_start, __reldata_end, __reldata_load_start
};

void plf_init_relocate(void) __attribute__((__section__(".init.text.plf_init_relocate")));

void plf_init_relocate(void)
{
    if (ro_reloc.start != ro_reloc.load) {
        memcpy(ro_reloc.start, ro_reloc.load, (size_t)(ro_reloc.end - ro_reloc.start));
    }
    if (data_reloc.start != data_reloc.load) {
        memcpy(data_reloc.start, data_reloc.load, (size_t)(data_reloc.end - data_reloc.start));
    }
}
