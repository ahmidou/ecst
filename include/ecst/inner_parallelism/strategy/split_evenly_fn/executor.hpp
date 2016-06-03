// Copyright (c) 2015-2016 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0
// http://vittorioromeo.info | vittorio.romeo@outlook.com

#pragma once

#include <ecst/config.hpp>
#include <ecst/aliases.hpp>
#include <ecst/signature.hpp>
#include <ecst/debug.hpp>
#include <ecst/inner_parallelism/utils.hpp>

ECST_INNER_PARALLELISM_STRATEGY_NAMESPACE
{
    namespace split_evenly_fn
    {
        namespace impl
        {
            template <typename TParameters>
            struct executor
            {
                using parameters = TParameters;

                template <typename TInstance, typename TContext,
                    typename TFAdapter, typename TF>
                void execute(
                    TInstance& inst, TContext& ctx, TFAdapter&& fa, TF&& f)
                {
                    namespace ss = signature::system;

                    auto getter = parameters::subtask_count_getter();
                    auto split_count = getter();

                    auto per_split = inst.subscribed_count() / split_count;

                    ELOG(                                                   // .
                        debug::lo_instance_parallelism()                    // .
                            << "split_evenly_fn:(" << inst.system_id()      // .
                            << "):\n\tsubscribed_count()="                  // .
                            << inst.subscribed_count()                      // .
                            << "\n\tper_split=" << per_split                // .
                            << "\n\tsplit_count=" << split_count << "\n\n"; // .
                        );

                    inst.prepare_and_wait_n_subtasks(split_count, [&](auto& b)
                        {
                            auto adapted_subtask(fa(b, ctx, FWD(f)));

                            // Builds and runs the subtasks.
                            utils::execute_split_runtime(
                                inst.subscribed_count(), per_split, split_count,
                                adapted_subtask);
                        });
                }
            };
        }
    }
}
ECST_INNER_PARALLELISM_STRATEGY_NAMESPACE_END
