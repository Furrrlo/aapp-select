# Advanced Algorithm Challenge - Deterministic Select

First challenge for the course "Advanced Algorithms and Parallel Programming" 2024/2025 @ PoliMi, focused on the first part of the course.

Implement the deterministic algorithm for the Select-ith problem (see [the relevant slides](docs/lecture8-Select-ith.pdf))

Material to be delivered:

- Copy of the Colab with code .pynb (no link)
  - Use comments
  - 24h grace period for configuration errors
- Short PDF report (max 2 pages)
  - Experimental setup
  - Performance measurements
  - Explanation of design choices
  - No screenshots of the code!

Goals:

- Display the asymptotic complexity using the Google benchmark library
- Provide sufficient tests to validate the implementation
- Compare with the randomized version and demonstrate its functionality with the tests used

Further rules can be found [here](docs/challenge_aapp_1.pdf)

**Grade**: 3/3

**Comment**: no issues

For a list of possible issues highlighted by the professor afterwards, see [here](docs/Challenge%20-%20results.pdf)

The only possible issue our project had is that, while our project worked even on arrays with distinct elements, it was not guaranteed to be O(n). As highlighted in the benchmark code to also support O(n) with dupes we would need to use a [three-way partition](https://en.wikipedia.org/wiki/Median_of_medians#Partition_helper_functions) which was not what was used in the slides, so we decided against it. Luckily, we were not penalized.

The final complete report in pdf form, including benchmarks results, can be found [here](https://github.com/Furrrlo/aapp-select/releases/download/v1.0/Challenge1.Report.pdf).

## Design choices

The C code closely resembles the pseudo code seen in class, with minor differences which we will highlight here.

If the function is called with len < 5, we compute the required rank over a small array by using the qsort algorithm to find it, as the algorithm wouldn’t otherwise work.

The calculation of the median of each group of 5 is implemented using a method which uses at most 6 comparisons, which can be considered constant time. This is because it was unspecified in the slides, and using a sorting-based naïve method would increase both the times and the 'c' coefficient in the benchmarks.

The medians of all the groups are stored in the first n/5 positions of the array, avoiding allocating additional memory.
This is because memory allocation on the heap has a non-deterministic time complexity, so it’s best to avoid it. The algorithm should still work correctly, as by storing in the first positions, we do not interfere with successive median calculations. This optimization is also done in the [Wikipedia pseudocode of the method](https://en.wikipedia.org/wiki/Median_of_medians#Algorithm).

## Building and running

The project uses make.

To execute tests run `make test`, to execute benchmarks run `make benchmark`.

Since the expected final deliverable is a colab notebook, to build the notebook for submissions run `make notebook`. This will tar the relevant files, then base64 the archive and slap the string into the `template.ipynb` file, creating a new `notebook.ipynb` file. If for some reason you only need the base64 string, run `make b64`.
