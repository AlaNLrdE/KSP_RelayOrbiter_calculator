.. _extending:

Extending the Calculator
========================

Every extension point is an interface. Add a class, wire it in
``src/main.cpp``, and leave :cpp:class:`Application` untouched.

.. list-table::
   :header-rows: 1
   :widths: 35 30 35

   * - Goal
     - Implement
     - Wire in
   * - Add a modded body
     - — (data only)
     - ``src/KspBodyDatabase.cpp``
   * - Load bodies from a file
     - :cpp:class:`IBodyDatabase`
     - ``main.cpp``
   * - Use Earth time (24 h days, 365 d years)
     - :cpp:class:`ITimeFormatter`
     - ``main.cpp``
   * - Output JSON / CSV / Markdown
     - :cpp:class:`IReportFormatter`
     - ``main.cpp``
   * - Change the physics model
     - :cpp:class:`IOrbitalMechanics`
     - ``main.cpp`` (and ``Application::run`` — see note)

Adding a body
-------------

Append an entry to ``KspBodyDatabase::bodies_``; see
:cpp:class:`KspBodyDatabase` for the field order. The body list, menus, and
checks pick it up automatically. Keep the invariants listed on
:cpp:class:`CelestialBody`.

A new time format
-----------------

.. code-block:: cpp

   // hpp/EarthTimeFormatter.hpp
   #pragma once
   #include "ITimeFormatter.hpp"

   class EarthTimeFormatter final : public ITimeFormatter {
   public:
       [[nodiscard]] std::string format(double seconds) const override;

       static constexpr double HOUR = 3'600.0;
       static constexpr double DAY  = 24.0  * HOUR;
       static constexpr double YEAR = 365.0 * DAY;
   };

.. code-block:: cpp

   // src/main.cpp
   auto time_fmt = std::make_shared<EarthTimeFormatter>();

A new report format
-------------------

.. code-block:: cpp

   class JsonReportFormatter final : public IReportFormatter {
   public:
       void print(const OrbitalResult& r) const override {
           std::printf("{\"body\":\"%s\",\"h_target\":%.3f,\"h_peri\":%.3f,"
                       "\"T_e\":%.3f,\"surface_ok\":%s,\"atm_ok\":%s}\n",
                       r.body_name.c_str(), r.h_target, r.h_peri, r.T_e,
                       r.surface_ok ? "true" : "false",
                       r.atm_ok     ? "true" : "false");
       }
   };

A batch mode
------------

Because :cpp:func:`IConstellationCalculator::calculate` is pure, a
non-interactive mode only needs a different front end:

.. code-block:: cpp

   // relay_calculator --body Kerbin --altitude-km 1000
   auto body = db->find(body_name);
   if (!body) { /* error */ }
   formatter->print(calculator->calculate(*body, altitude_km * 1000.0));

Validation of the altitude (atmosphere, SOI) would need to be moved out of
``ask_altitude`` into a reusable function first.

.. note::

   :cpp:func:`Application::run` constructs its own
   :cpp:class:`OrbitalMechanics` for the altitude menu. A replacement physics
   model must also be used there until that dependency is injected.

Checklist
---------

#. Add the header to ``hpp/`` and the source to ``src/``. The Makefile
   compiles every ``src/*.cpp`` automatically.
#. Add ``utest/test_<Class>.cpp``.
#. Wire the class in ``src/main.cpp``.
#. Document it: add ``docs/source/api/<Class>.rst`` and
   ``docs/source/diagrams/classes/<Class>.puml``, update
   ``diagrams/class_overview.puml``, and add the page to ``api/index.rst``.
#. ``make -C build && make -C docs html``.
