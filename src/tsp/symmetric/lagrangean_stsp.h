// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#ifndef TSP_SYMMETRIC_LAGRANGEAN_STSP_H_
#define TSP_SYMMETRIC_LAGRANGEAN_STSP_H_

#include <bnb/tree.h>
#include <tsp/common/types.h>
#include <tsp/symmetric/common.h>
#include <tsp/symmetric/lagrangean.h>

namespace bnb {
struct Stats;
}  // namespace bnb

namespace stsp {
class LagrangeanSolver {
 public:
    typedef tsp::Set Set;
    typedef tsp::Solution Solution;
    typedef tsp::InitialData InitialData;

    typedef bnb::Node<Set> Node;
    typedef std::vector<Node *> NodeList;

    LagrangeanSolver();

    void init(const InitialData &data, bnb::SearchTree<Set> *mm);
    void get_initial_node(Node *node);
    void get_initial_solution(Solution *sol);
    void branch(const Node *node, value_type &record
            , NodeList &nodes, Solution &sol, bnb::Stats &stats);

 private:
    NodeList branching_rule1(const Node *node);
    NodeList branching_rule2(const Node *node);
    NodeList branching_rule3(const Node *node);
    void transform_node(Node *node, value_type record, bnb::Stats &stats);
    bool build_solution(const Node *node, Solution *solution);
    void get_included_edges(const bnb::Node<LagrangeanSolver::Set> *node
        , std::vector<int> &edge_map) const;

    typedef NodeList (LagrangeanSolver::*branching_rule_ptr) (const Node *);
    branching_rule_ptr branching_rule;

    size_t dimension_;
    std::vector<value_type> matrix_;
    std::vector<value_type> matrix_original_;
    std::vector<int> edge_map_;

    bnb::SearchTree<Set> *search_tree_;
    LagrangeanRelaxation lr_;
    Solution solution_initial_;

    size_t gradient_max_iters_;
    value_type epsilon_;
};
}  // namespace stsp


#endif  // TSP_SYMMETRIC_LAGRANGEAN_STSP_H_
