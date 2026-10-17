.. _testing:

Testing
=======

The project uses `GoogleTest <https://github.com/google/googletest>`_, pulled
in as a git submodule and compiled from source by ``build/Makefile``.

Running
-------

.. code-block:: sh

   make -C build test                                         # build + run all
   ./build/obj/run_tests                                      # rerun without rebuilding
   ./build/obj/run_tests --gtest_filter='KspBodyDatabase.*'   # one suite
   ./build/obj/run_tests --gtest_filter='*Periapsis*'         # by pattern
   ./build/obj/run_tests --gtest_list_tests                   # list only

Expected result::

   [==========] 66 tests from 4 test suites ran.
   [  PASSED  ] 66 tests.

Structure
---------

.. uml:: diagrams/test_structure.puml
   :caption: Test files and the code they exercise

.. list-table::
   :header-rows: 1
   :widths: 30 10 60

   * - Suite
     - Tests
     - Focus
   * - ``OrbitalMechanics``
     - 15
     - Physics identities: Kepler's law, vis-viva, energy conservation,
       synchronous altitude, SOI limit.
   * - ``KspBodyDatabase``
     - 19
     - Catalogue completeness, field values, lookups, parent/child links and
       ordering.
   * - ``ConstellationCalculator``
     - 20
     - Every :cpp:class:`OrbitalResult` field, velocity ordering, safety flags
       and minimum altitudes.
   * - ``KerbinTimeFormatter``
     - 12
     - Calendar constants, unit boundaries, rounding.

The test list for each class is on its API page:
:cpp:class:`OrbitalMechanics`, :cpp:class:`KspBodyDatabase`,
:cpp:class:`ConstellationCalculator`, :cpp:class:`KerbinTimeFormatter`.

Fixtures
--------

``utest/TestFixtures.hpp`` provides inline factories in namespace
``fixtures``:

.. list-table::
   :header-rows: 1

   * - Function
     - Body
     - Why it is useful
   * - ``make_kerbin()``
     - Kerbin
     - Has an atmosphere, a parent, and moons.
   * - ``make_mun()``
     - Mun
     - Airless, small; synchronous orbit outside the SOI.
   * - ``make_kerbol()``
     - Kerbol
     - Root body: no SOI, no parent.

Building bodies by hand keeps the physics tests independent of
:cpp:class:`KspBodyDatabase`: a typo in the catalogue cannot break an
orbital-mechanics test.

Conventions
-----------

* One test file per production class: ``utest/test_<Class>.cpp``. The
  Makefile picks up every ``utest/test_*.cpp`` automatically.
* Plain ``TEST(Suite, Name)`` — no fixture classes are needed because
  the code under test is stateless.
* Floating-point comparisons use ``EXPECT_NEAR`` with an explicit tolerance,
  or ``EXPECT_DOUBLE_EQ`` for values that should be bit-identical.
* Tests describe *properties* (``PeriapsisVelocityGreaterThanApoApsis``,
  ``EnergyConservationAlongOrbit``) as well as reference values, so they stay
  meaningful if constants are tuned.
* :cpp:class:`ConstellationCalculator` tests inject a real
  :cpp:class:`OrbitalMechanics` rather than a mock; the physics is fast and
  deterministic.

Development history
-------------------

The C++ port was written test-first:

#. **Red** — headers, stub sources, and all four test suites were committed;
   the suites compiled, and 52 of 66 tests failed.
#. **Green** — :cpp:class:`OrbitalMechanics`, then
   :cpp:class:`KspBodyDatabase` and :cpp:class:`ConstellationCalculator`, then
   :cpp:class:`KerbinTimeFormatter` were implemented until all 66 passed.
#. :cpp:class:`Application` and :cpp:class:`ConsoleReportFormatter` were
   added last.

What is not tested
------------------

* :cpp:class:`Application` — reads ``std::cin`` directly.
* :cpp:class:`ConsoleReportFormatter` — writes ``stdout`` directly.
* The file-local helpers in ``Application.cpp`` (see :ref:`internals`).

Making the streams injectable would allow these to be covered with scripted
input and captured output; see :ref:`limitations`.

Adding a test
-------------

.. code-block:: cpp

   // utest/test_ConstellationCalculator.cpp
   TEST(ConstellationCalculator, PeriapsisIsAboutHalfTheTargetRadius) {
       ConstellationCalculator calc(std::make_shared<OrbitalMechanics>());
       const auto r = calc.calculate(fixtures::make_mun(), 500'000.0);
       EXPECT_NEAR(r.r_peri / r.r_c, 0.52629, 1e-5);
   }

New ``test_*.cpp`` files need no Makefile change.
