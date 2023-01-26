/*
Copyright (c) 2015-2020, Intel Corporation
Copyright (c) 2019-2020, University of Utah

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

//////////////////////////////////////////////////////////////////////////////
#include "options.h"
#include "program.h"
#include "utils.h"

#include <iostream>
#include <string>
#include <vector>

using namespace yarpgen;

int main(int argc, char *argv[]) {
    OptionParser::initOptions();
    OptionParser::parse(argc, argv);

    Options &options = Options::getInstance();
    rand_val_gen = std::make_shared<RandValGen>(options.getSeed());
    options.setSeed(rand_val_gen->getSeed());

    if (options.getMutationKind() == MutationKind::EXPRS ||
        options.getMutationKind() == MutationKind::ALL) {
        rand_val_gen->setMutationSeed(options.getMutationSeed());
    }

    ProgramGenerator new_program;

    // NOTE(cummins): Quick hack to dump the test function and external
    // declarations to stdout.
    auto emit_ctx = std::make_shared<EmitCtx>();

    if (options.isC()) {
        MinCall::emitCDefinition(emit_ctx, std::cout);
        MaxCall::emitCDefinition(emit_ctx, std::cout);
    }
    if (options.isCXX())
        std::cout << "#include <algorithm>\n";
    else if (options.isSYCL()) {
        std::cout << "#include <CL/sycl.hpp>\n";
        std::cout << "#if defined(FPGA) || defined(FPGA_EMULATOR)\n";
        std::cout << "    #include <CL/sycl/intel/fpga_extensions.hpp>\n";
        std::cout << "#endif\n";
    }

    new_program.emitExtDecl(emit_ctx, std::cout);
    new_program.emitTest(emit_ctx, std::cout);

    return 0;
}
