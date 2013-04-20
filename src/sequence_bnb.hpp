// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef __SEQUENCE_BNB__
#define __SEQUENCE_BNB__

#include <stack>
#include <vector>
#include <iostream>

#include "defs.h"
#include "bnb.hpp"
#include "tree.hpp"
#include "timer.hpp"
#include "stats.h"

template <typename SolverProvider, typename NodesContainer = LifoContainer>
class SequenceBNB {
    typedef typename SolverProvider::Solver Solver;
    typedef typename Solver::Set Set;
    typedef typename Solver::Solution Solution;
    typedef typename Solver::InitialData InitialData;
    
 public:
    Solution solve(const InitialData &data
        , size_t max_branches = -1, value_type record = M_VAL);

    void print_stats(std::ostream &os) const {
        std::cout << "\nStats: \n" << m_stats << std::endl;        
    }

 private:
    Stats m_stats;
};

template <typename SolverProvider, typename NodesContainer>
typename SolverProvider::Solver::Solution
    SequenceBNB<SolverProvider, NodesContainer>::solve(
    const InitialData &data, size_t max_branches, value_type record) {
    static const size_t MIN_RANK_VALUE = 2;

    m_stats.clear();
    Solution sol;
    if (data.rank > MIN_RANK_VALUE) {
        MemoryManager<Set> mm;
        mm.init(data.rank*data.rank*data.rank*data.rank);
        Node<Set> *node = mm.alloc(NULL);
        Solution initSol;

        Solver *solver = SolverProvider::get_solver();
        solver->init(data, &mm);
        solver->get_initial_node(node);
        solver->get_initial_solution(&initSol);
        record = initSol.value;

        auto nodes = make_nodes_container<SolverProvider>(NodesContainer());
        std::vector<Node<Set> * > tmp_nodes;
        nodes.push(node);
        Timer timer;
        while (!nodes.empty() /*&& this->stats.branches < max_branches*/) {
            node = nodes.top();
            nodes.pop();

            solver->branch(node, record, tmp_nodes, sol, m_stats);
            for (auto &set: tmp_nodes) {
                nodes.push(set);
            }
            tmp_nodes.clear();
        }
        m_stats.seconds = timer.elapsed_seconds();

        SolverProvider::free_solver(solver);
    }

    sol.value = record;
    sol.route.clear();
    return std::move(sol);
}

#endif //__SEQUENCE_BNB__
