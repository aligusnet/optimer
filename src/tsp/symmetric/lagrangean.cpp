// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "lagrangean.h"

#include <cstring>
#include <cmath>

#include <numeric>

namespace stsp {
    LagrangeanRelaxation::LagrangeanRelaxation(size_t max_dimension) {
        matrix_.resize(max_dimension * max_dimension);
    }

    MSOneTree::Solution<value_type> LagrangeanRelaxation::solve(
            const std::vector<value_type> &initial_matrix
            , size_t dimension
            , value_type upper_bound
            , size_t max_iter) {
        static const size_t TOUR_DEGREE = 2;
        static const value_type ALPHA_START_VALUE = 2;

        MSOneTree::Solution<value_type> solution;
        std::memcpy(matrix_.data(), initial_matrix.data()
                    , sizeof(value_type)*dimension*dimension);

        std::vector<value_type> lambda(dimension, value_type());
        std::vector<int> gradient(dimension);

        value_type alpha = ALPHA_START_VALUE;
        value_type alpha_reduce = (alpha-0.01) / max_iter;
        while (max_iter--) {
            auto solution = MSOneTree::solve(matrix_.data(), dimension);

            std::fill(gradient.begin(), gradient.end(), 0);
            for (const auto &edge : solution.edges) {
                ++gradient[edge.first];
                ++gradient[edge.second];
            }

            value_type lagrangean = 0;
            for (const auto &edge : solution.edges) {
                lagrangean += matrix_[edge.first*dimension + edge.second];
            }
            lagrangean -= 2*std::accumulate(lambda.begin()
                                            , lambda.end()
                                            , value_type());

            solution.value = lagrangean;

            if (lagrangean > upper_bound) {
                return solution;
            }

            auto pos = std::find_if(gradient.begin(), gradient.end()
                                    , [](int g) {return g != TOUR_DEGREE;});
            if (pos == gradient.end()) {
                solution.value = lagrangean;
                return solution;
            }

            value_type sum_gradient = std::accumulate(gradient.begin()
                    , gradient.end()
                    , value_type()
                    , [](const value_type &sum, const value_type &val) {
                        return sum + (val-TOUR_DEGREE)*(val-TOUR_DEGREE);
                    });
            value_type step = alpha*(upper_bound - lagrangean)/sum_gradient;

            for (size_t i = 0; i < dimension; ++i) {
                lambda[i] += step*gradient[i];
            }

            alpha -= alpha_reduce;

            for (size_t i = 0; i < dimension; ++i) {
                for (size_t j = 0; j < dimension; ++j) {
                    if (i == j) {
                        matrix_[i*dimension+j] = M_VAL;
                    } else {
                        matrix_[i*dimension+j] = initial_matrix[i*dimension+j]
                        + (lambda[i] + lambda[j]);
                    }
                }
            }
        }
        return solution;
    }
}  // namespace stsp
