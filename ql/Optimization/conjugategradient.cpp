

/*
 Copyright (C) 2001, 2002 Nicolas Di C�sar�

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/
/*! \file conjugategradient.cpp
    \brief Conjugate gradient optimization method

    \fullpath
    ql/Optimization/%conjugategradient.cpp
*/

#include "ql/Optimization/conjugategradient.hpp"

namespace QuantLib {

    namespace Optimization {

        void ConjugateGradient::Minimize(OptimizationProblem &P) {
            bool EndCriteria = false;

            // function and squared norm of gradient values;
            double f, fold, g2, gold2;
            double c;
            double normdiff;
            // classical initial value for line-search step
            double t = 1.0;

            // reference X as the optimization problem variable
            Array& X = x();
            Array& SearchDirection = searchDirection();
            // Set g at the size of the optimization problem search direction
            int sz = searchDirection().size();
            Array g(sz), d(sz), sddiff(sz);

            f = P.valueAndGradient(g, X);
            SearchDirection = -g;
            g2 = DotProduct (g, g);

            do {
                // Linesearch
                t = (*lineSearch_)(P, t, f, g2);

                if (lineSearch_->succeed ()) {
                    // Updates
                    d = SearchDirection;
                    // New point
                    X = lineSearch_->lastX ();
                    // New function value
                    fold = f;
                    f = lineSearch_->lastFunctionValue ();
                    // New gradient and search direction vectors
                    g = lineSearch_->lastGradient ();
                    // orthogonalization coef
                    gold2 = g2;
                    g2 = lineSearch_->lastGradientNorm2 ();
                    c = g2 / gold2;
                    // conjugate gradient search direction
                    sddiff = (-g + c * d) - SearchDirection;
                    normdiff = QL_SQRT (DotProduct (sddiff, sddiff));
                    SearchDirection = -g + c * d;
                    // End criteria
                    EndCriteria = endCriteria()(iterationNumber_,
                        fold, QL_SQRT (gold2), f, QL_SQRT(g2), normdiff);

                    // Increase interation number
                    iterationNumber()++;
                }
            } while ((EndCriteria == false) && (lineSearch_->succeed()));

            if (!lineSearch_->succeed())
                throw Error("ConjugateGradient::Minimize(), line-search failed!");
        }

    }

}
