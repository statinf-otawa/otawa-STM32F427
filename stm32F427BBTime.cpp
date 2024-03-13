/*
 *	STM32 module implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2017, IRIT UPS.
 *
 *	OTAWA is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	OTAWA is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OTAWA; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <otawa/events/StandardEventBuilder.h>
#include <otawa/etime/EdgeTimeBuilder.h>
#include <otawa/prop/DynIdentifier.h>
#include <elm/sys/Path.h>
#include <otawa/loader/arm.h>
#include <elm/io/FileOutput.h>
#include <elm/data/Vector.h>
#include "M4FCycleTiming.h"
#include "armCortexM4_operand.h"

namespace otawa { namespace stm32 {
    using namespace elm::io;
    extern p::id<bool> WRITE_LOG;
    typedef enum {
        FE    = 0,
        DE    = 1,
        EXE   = 2,
        WR    = 3,
        CNT   = 4
    } pipeline_stage_t;
    
    class M4ExeGraph: public etime::EdgeTimeGraph {
	public:
		
		M4ExeGraph(WorkSpace* ws,
                 ParExeProc* proc, 
                 Vector<Resource* >* hw_resources, 
				 ParExeSequence* seq,
                 const PropList& props,
                 FileOutput* out, 
                 elm::Vector<Address>* unknown_inst_address) : etime::EdgeTimeGraph(ws, proc, hw_resources, seq, props), 
                                                                exec_f(0), exec_m4(0), _unknown_inst_address(unknown_inst_address), _out(out) {
			
			// Try to find arm loader with arm information
			DynIdentifier<arm::Info* > id("otawa::arm::Info::ID");
			info = id(_ws->process());
			if (!info)
				throw Exception("ARM loader with otawa::arm::INFO is required !");
			// Get memory configuration
			mem = hard::MEMORY_FEATURE.get(ws);
			ASSERTP(mem, "Memory feature not found");
		}

        /*
			Write to the log file, some info about the instructions whose
			cycle timing info has not been found.
		*/
        // TODO: finish this
		void dumpUnknowInst() {
			if (_out == nullptr)
				return;
			// for (InstIterator inst(this); inst(); inst++) {
			// 	if (!getInstCycleTiming(inst->inst())->unknown)
			// 		continue;
				
			// 	auto addr = inst->inst()->address();
			// 	if (_unknown_inst_address->contains(addr))
			// 		continue;
			// 	_unknown_inst_address->add(addr);
			// 	*_out << addr << "; " << inst->inst() << endl;
			// }
		}

        void addEdgesForPipelineOrder() override {
            ParExeGraph::addEdgesForPipelineOrder();
			// Add latency penalty to Exec-FU nodes
            for (InstIterator inst(this); inst(); inst++) {
				// get cycle_time_info of inst
                m4f_time_t* inst_cycle_timing = getInstCycleTiming(inst->inst());
                ot::time inst_cost = inst_cycle_timing->ex_cost;
                if (inst_cycle_timing->multi) {
                    if (!inst->inst()->isFloat()) { // this check is not really necessary.
                        inst_cost += getInstNReg(inst->inst());
                    }
                }
                if (inst_cost > 1)
                    inst->firstFUNode()->setLatency(inst_cost - 1);

            }
        }

        
        void build() override {

            for (ParExePipeline::StageIterator pipeline_stage(_microprocessor->pipeline()); pipeline_stage(); pipeline_stage++) {
				if (pipeline_stage->name() == "Fetch") {
					stage[FE] = *pipeline_stage;
				} else if (pipeline_stage->name() == "Decode") {
					stage[DE] = *pipeline_stage;
				} else if (pipeline_stage->name() == "EXE") {
					stage[EXE] = *pipeline_stage;
					for (int i = 0; i < pipeline_stage->numFus(); i++) {
						ParExePipeline* fu = pipeline_stage->fu(i);
						if (fu->firstStage()->name().startsWith("EXEC_F")) {
							exec_f = fu;
						} else if (fu->firstStage()->name().startsWith("EXEC_M4")) {
							exec_m4 = fu;
						} else
							ASSERTP(false, fu->firstStage()->name());
						
					}
				} else if (pipeline_stage->name() == "Write") {
					stage[WR] = *pipeline_stage;
				} 

			}
			ASSERTP(stage[FE], "No 'Prefetch' stage found");
			ASSERTP(stage[DE], "No 'Decode' stage found");
			ASSERTP(stage[EXE], "No 'EXE' stage found");
			ASSERTP(stage[WR], "No 'Write back' stage found");
			ASSERTP(exec_f, "No FPU fu found");
			ASSERTP(exec_m4, "No M4 fu found");
			
            // Build the execution graph 
			createSequenceResources();
			createNodes();
			addEdgesForPipelineOrder();
			addEdgesForFetch();
			addEdgesForProgramOrder();
			addEdgesForMemoryOrder();
			addEdgesForDataDependencies();
            
			// dumpUnknowInst();
        }

        private:
            otawa::arm::Info* info;
            const hard::Memory* mem;
            ParExeStage* stage[CNT];
            ParExePipeline *exec_f, *exec_m4;
            FileOutput* _out = nullptr;
            elm::Vector<Address>* _unknown_inst_address = nullptr;


            /*
			Attempts to decode an instruction and return the corresponding behavior "cycle timing behavior".
			
			    inst: Instruction decode.
            */
            m4f_time_t* getInstCycleTiming(Inst* inst) {
                void* inst_info = info->decode(inst);
                m4f_time_t* inst_cycle_timing = xilinxM4FTime(inst_info);
                info->free(inst_info);
                return inst_cycle_timing;
            }
            
            t::uint32 getInstNReg(Inst* inst) {
                void* inst_info = info->decode(inst);
                t::uint32 inst_n_reg = armV7_NReg(inst_info);
                info->free(inst_info);
                return inst_n_reg;
		    }

    };

    class BBTimerSTM32M4F: public etime::EdgeTimeBuilder {
	public:
		static p::declare reg;
		BBTimerSTM32M4F(void): etime::EdgeTimeBuilder(reg) { }

	protected:
		virtual void configure(const PropList& props) {
			etime::EdgeTimeBuilder::configure(props);
			write_log = WRITE_LOG(props);
			_props = props;
		}
		void setup(WorkSpace* ws) override {
			etime::EdgeTimeBuilder::setup(ws);
			const hard::CacheConfiguration* cache_config = hard::CACHE_CONFIGURATION_FEATURE.get(ws);
			if (cache_config)
				throw ProcessorException(*this, "Cache support is not implemented for the Cortex M4");

			if (write_log) {
				sys::Path log_file_path = sys::Path(ws->process()->program()->name() + ".log");
				bool write_header = (log_file_path.exists()) ? false : true;
				log_stream = new FileOutput(log_file_path, true);
				if (write_header)
					*log_stream << "########################################################" << endl
								<< "# Static analysis on " << ws->process()->program()->name() << endl
								<< "# Overestimated instructions" << endl
								<< "# Address (hex); Instruction" << endl
								<< "########################################################" << endl;
				else
					*log_stream << endl; // sep
				
				unknown_inst_address = new elm::Vector<Address>();
			}
		}

		etime::EdgeTimeGraph* make(ParExeSequence* seq) override {
			M4ExeGraph* graph = new M4ExeGraph(workspace(), _microprocessor, ressources(), seq, _props, log_stream, unknown_inst_address);
			graph->build();
			return graph;
		}


		virtual void clean(ParExeGraph* graph) {
			log_stream->flush();
			delete graph;
		}
	private:
		PropList _props;
		FileOutput* log_stream = nullptr;
		bool write_log = 0;
		elm::Vector<Address>* unknown_inst_address = nullptr;
	};

	

	p::declare BBTimerSTM32M4F::reg = p::init("otawa::stm32::BBTimerSTM32M4F", Version(1, 0, 0))
										.extend<etime::EdgeTimeBuilder>()
										.maker<BBTimerSTM32M4F>();
	
} // namespace stm32
} // namespace otawa
