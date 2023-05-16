// Copyright (c) 2022-2023 Simons Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0.txt
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Authors: jasonkaye

#include "dlr_kernels.hpp"
#include <numbers>

using namespace std;
using namespace std::numbers;

namespace cppdlr {

  double k_it(double t, double om) {

    if (t >= 0) {
      return k_it_abs(t, om);
    } else {
      return k_it_abs(-t, -om);
    }
  }

  double k_it_abs(double t, double om) {

    if (om >= 0.0) {
      return exp(-t * om) / (1.0 + exp(-om));
    } else {
      return exp((1.0 - t) * om) / (1.0 + exp(om));
    }
  }

  std::complex<double> k_if(int n, double om) { return -1.0 / (n * pi * 1i - om); }

} // namespace cppdlr
