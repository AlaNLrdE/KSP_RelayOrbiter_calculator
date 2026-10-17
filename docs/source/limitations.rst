.. _limitations:

Known Limitations
=================

Behaviour of the current code that is surprising, incomplete, or not aligned
with the design. Each item was confirmed by running the program or reading the
source.

Input handling
--------------

End of input causes an infinite loop
   :ref:`read_line <internals>` returns an empty string on EOF, which every
   prompt treats as invalid and re-asks forever. Piping input that ends early
   (or pressing Ctrl-D) leaves the program spinning, printing
   ``Invalid selection`` repeatedly. Checking ``std::cin`` after ``getline``
   and exiting would fix it.

Very large menu numbers crash the program
   Menu input is checked with ``is_digits`` and then converted with
   ``std::stoul``. A digit string too large for ``unsigned long`` (for example
   ``99999999999999999999``) throws ``std::out_of_range``, which is not
   caught, and the program terminates.

Partial numbers are accepted for custom altitudes
   ``std::stod`` stops at the first invalid character, so ``12abc`` is read
   as 12 km.

``nan`` and ``inf`` are accepted where no SOI limit applies
   ``std::stod`` parses ``nan`` and ``inf``. ``nan`` fails every comparison,
   so it passes all validation; for Kerbol (no SOI limit) ``inf`` passes too.
   The report then prints ``nan`` values.

Body names are case-sensitive
   ``kerbin`` is rejected; only ``Kerbin`` works.

Altitude presets skip some checks
   The synchronous-orbit preset is checked against the SOI but not the
   atmosphere; the child-distance and midpoint presets are not checked at all.
   With the stock bodies these values are always valid, but a modded body
   could produce one that is not.

Design deviations
-----------------

``Application`` creates its own ``OrbitalMechanics``
   :cpp:func:`Application::run` instantiates :cpp:class:`OrbitalMechanics`
   directly for the altitude menu instead of receiving an
   :cpp:class:`IOrbitalMechanics`. This is the only concrete instantiation
   outside ``main.cpp``, and it means the physics used by the menu cannot be
   swapped.

``Application::time_formatter_`` is unused
   The time formatter is injected into :cpp:class:`Application` but never
   used there.

``OrbitalMechanics::sync_orbit_altitude`` never returns ``nullopt``
   The interface documents ``nullopt`` for a synchronous orbit below the
   surface or atmosphere, and the menu has a branch for it, but the
   implementation never produces it. The branch is unreachable.

Unit comments in ``OrbitalResult.hpp``
   ``min_h_surface`` and ``min_h_atm`` are commented as kilometres but hold
   metres.

Output
------

Kerbin time rounding does not carry
   :cpp:func:`KerbinTimeFormatter::format` rounds seconds only when printing,
   so 59.96 s prints as ``0m 60.0s``.

Negative durations lose their sign
   :cpp:func:`KerbinTimeFormatter::format` formats the absolute value.

Testing and build
-----------------

No tests for the interactive layer
   :cpp:class:`Application` and :cpp:class:`ConsoleReportFormatter` use
   ``std::cin`` / ``stdout`` directly and are not covered by unit tests.

Header changes do not trigger rebuilds
   ``build/Makefile`` has no header dependency tracking. Run
   ``make -C build clean`` after editing a header. Adding ``-MMD -MP`` to
   ``CXXFLAGS`` and including the generated ``.d`` files would fix this.

Scope
-----

* Only three satellites at 120°. Other counts need a different period ratio:
  for :math:`n` satellites, :math:`T_e/T_c = (n-1)/n`.
* Two-body Keplerian physics only, matching KSP's patched conics.
