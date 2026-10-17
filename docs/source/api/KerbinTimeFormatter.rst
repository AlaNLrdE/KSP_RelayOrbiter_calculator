KerbinTimeFormatter
===================

:Header: ``hpp/KerbinTimeFormatter.hpp``
:Source: ``src/KerbinTimeFormatter.cpp``
:Kind: ``final`` class
:Implements: :cpp:class:`ITimeFormatter`
:Tests: ``utest/test_KerbinTimeFormatter.cpp`` (12 tests)

.. uml:: ../diagrams/classes/KerbinTimeFormatter.puml
   :caption: KerbinTimeFormatter

.. cpp:class:: KerbinTimeFormatter final : public ITimeFormatter

   Formats durations using the in-game Kerbin calendar: a **6-hour day** and
   a **426-day year**.

   .. cpp:member:: static constexpr double HOUR = 3'600.0

      Seconds per hour.

   .. cpp:member:: static constexpr double DAY = 6.0 * HOUR

      Seconds per Kerbin day — 21,600.

   .. cpp:member:: static constexpr double YEAR = 426.0 * DAY

      Seconds per Kerbin year — 9,201,600.

   .. cpp:function:: std::string format(double seconds) const override

      Returns ``"<y>y <d>d <h>h <m>m <s>s"`` with integer years, days, hours
      and minutes and seconds to one decimal (``%.1f``). Every field is always
      printed, even when zero.

      Algorithm: take ``|seconds|``; then for each unit from years down to
      minutes, take the integer quotient and keep the remainder with
      ``std::fmod``. The remaining seconds are printed with one decimal.

Examples
--------

.. list-table::
   :header-rows: 1

   * - Input (s)
     - Output
   * - ``0``
     - ``0y 0d 0h 0m 0.0s``
   * - ``60``
     - ``0y 0d 0h 1m 0.0s``
   * - ``21600``
     - ``0y 1d 0h 0m 0.0s``
   * - ``21549.425`` (Kerbin day)
     - ``0y 0d 5h 59m 9.4s``
   * - ``2·YEAR + 3·DAY + HOUR + 5·60 + 7.5``
     - ``2y 3d 1h 5m 7.5s``
   * - ``1.46``
     - ``0y 0d 0h 0m 1.5s``
   * - ``1.44``
     - ``0y 0d 0h 0m 1.4s``

Edge cases
----------

* **Negative input** is formatted as its absolute value: ``-90`` →
  ``0y 0d 0h 1m 30.0s``. The sign is lost.
* **Rounding does not carry**: ``59.96`` → ``0y 0d 0h 0m 60.0s`` rather than
  ``0y 0d 0h 1m 0.0s``, because seconds are rounded only when printed.
* The rounding test uses ``1.46``/``1.44`` rather than ``1.45``, because
  ``1.45`` is stored as ``1.4499…`` in IEEE-754 and would round down.

Test coverage
-------------

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - Test
     - Verifies
   * - ``OneHourIs3600Seconds`` / ``OneDayIsSixHours`` / ``OneYearIs426Days``
     - The calendar constants
   * - ``ZeroSeconds`` / ``ExactlyOneSecond`` / ``ExactlyOneMinute`` / ``ExactlyOneHour`` / ``ExactlyOneDay`` / ``ExactlyOneYear``
     - Unit boundaries
   * - ``MixedValue``
     - All fields non-zero
   * - ``KerbinRotPeriodFormatsCorrectly``
     - Kerbin's sidereal day
   * - ``FractionalSecondsRoundedToOneDecimal``
     - One-decimal rounding
