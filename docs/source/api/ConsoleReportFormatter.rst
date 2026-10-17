ConsoleReportFormatter
======================

:Header: ``hpp/ConsoleReportFormatter.hpp``
:Source: ``src/ConsoleReportFormatter.cpp``
:Kind: ``final`` class
:Implements: :cpp:class:`IReportFormatter`
:Depends on: :cpp:class:`ITimeFormatter`
:Tests: none (output is verified manually)

.. uml:: ../diagrams/classes/ConsoleReportFormatter.puml
   :caption: ConsoleReportFormatter and its file-local helpers

.. cpp:class:: ConsoleReportFormatter final : public IReportFormatter

   Prints the fixed-width, four-section report to ``stdout`` with
   ``std::printf``.

   .. cpp:function:: explicit ConsoleReportFormatter(std::shared_ptr<ITimeFormatter> time_formatter)

      :param time_formatter: used to render :math:`T_c` and :math:`T_e`;
                             stored in :cpp:member:`time_formatter_`. Must not
                             be null.

   .. cpp:function:: void print(const OrbitalResult &result) const override

      Writes, in order:

      #. A 62-character ``=`` rule, the title
         ``RESULTS — <body> / target altitude <h> km``, and another rule.
      #. **CIRCULAR ORBIT** — altitude, radius, orbital velocity, period.
      #. **ELLIPTICAL PARKING ORBIT** — apoapsis altitude (marked
         ``(= target)``) and radius, periapsis altitude and radius, semi-major
         axis, period, and the :math:`T_e/T_c` ratio to six decimals.
      #. **VELOCITIES** — circular velocity and carrier speed at both apsides.
      #. **DEPLOYMENT PROCEDURE** — eight numbered steps with the target
         altitude, periapsis altitude, and :math:`T_e` filled in.
      #. If the periapsis is unsafe, a warning block framed by ``!`` rules (see
         below).
      #. A closing ``=`` rule.

   .. cpp:member:: private std::shared_ptr<ITimeFormatter> time_formatter_

      Injected time formatter.

Number formatting
-----------------

.. list-table::
   :header-rows: 1

   * - Quantity
     - Format
     - Example
   * - Distances
     - metres ÷ 1000, ``%.2f``, followed by ``km``
     - ``1221.03  km``
   * - Velocities
     - ``%.3f``, followed by ``m/s``
     - ``1233.768  m/s``
   * - Periods
     - :cpp:func:`ITimeFormatter::format` then raw seconds ``%.2f s``
     - ``0y 0d 1h 15m 11.1s  (4511.10 s)``
   * - Ratio
     - ``%.6f``
     - ``0.666667``
   * - Minimum altitudes (warning)
     - ``%.1f km``
     - ``673.1 km``

Labels are left-aligned in a 32-character column (``%-32s``) followed by
``": "``.

Periapsis status
----------------

The periapsis altitude row carries a suffix chosen from the safety flags:

.. list-table::
   :header-rows: 1

   * - ``surface_ok``
     - ``atm_ok``
     - Row suffix
     - Warning block
   * - true
     - true
     - ``km  OK``
     - none
   * - true
     - false
     - ``km    INSIDE ATMOSPHERE``
     - "Periapsis is inside the atmosphere!" + ``min_h_atm``
   * - false
     - *any*
     - ``km    BELOW SURFACE``
     - "Periapsis is underground!" + ``min_h_surface``

File-local helpers
------------------

These ``static`` functions in ``ConsoleReportFormatter.cpp`` are not part of
the public API:

``W``
   ``constexpr int = 62`` — width of the horizontal rules.

``row(label, value, unit = "", warn = "")``
   Prints one ``label : value  unit  warn`` line, omitting empty parts.

``fmt_km(metres, decimals = 2)``
   Converts metres to a kilometre string.

``fmt_ms(v)``
   Formats a velocity with three decimals.
