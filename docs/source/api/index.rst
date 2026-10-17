.. _api:

API Reference
=============

Every type in ``hpp/`` has its own page with a class diagram, the full
declaration, member-by-member documentation, and the tests that cover it.

Units are SI throughout: **metres**, **seconds**, **m/s**, and **m³/s²**.
Conversion to kilometres happens only when printing.

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - Type
     - Kind
     - Purpose
   * - :cpp:class:`CelestialBody`
     - struct
     - Physical and orbital parameters of one body.
   * - :cpp:class:`OrbitalResult`
     - struct
     - Every value computed for one constellation.
   * - :cpp:class:`IOrbitalMechanics`
     - interface
     - Kepler period, vis-viva, synchronous altitude, SOI limit.
   * - :cpp:class:`OrbitalMechanics`
     - class
     - Closed-form implementation of :cpp:class:`IOrbitalMechanics`.
   * - :cpp:class:`IBodyDatabase`
     - interface
     - Read-only body catalogue.
   * - :cpp:class:`KspBodyDatabase`
     - class
     - The 17 stock KSP bodies.
   * - :cpp:class:`IConstellationCalculator`
     - interface
     - Derives the parking orbit for a target altitude.
   * - :cpp:class:`ConstellationCalculator`
     - class
     - Implements the :math:`T_e = \tfrac{2}{3}T_c` derivation.
   * - :cpp:class:`IReportFormatter`
     - interface
     - Presents an :cpp:class:`OrbitalResult`.
   * - :cpp:class:`ConsoleReportFormatter`
     - class
     - Prints the four-section report to stdout.
   * - :cpp:class:`ITimeFormatter`
     - interface
     - Turns seconds into a human-readable duration.
   * - :cpp:class:`KerbinTimeFormatter`
     - class
     - Kerbin calendar: 6-hour days, 426-day years.
   * - :cpp:class:`Application`
     - class
     - Interactive loop tying everything together.

.. toctree::
   :maxdepth: 1
   :caption: Value types

   CelestialBody
   OrbitalResult

.. toctree::
   :maxdepth: 1
   :caption: Orbital mechanics

   IOrbitalMechanics
   OrbitalMechanics

.. toctree::
   :maxdepth: 1
   :caption: Body catalogue

   IBodyDatabase
   KspBodyDatabase

.. toctree::
   :maxdepth: 1
   :caption: Constellation calculation

   IConstellationCalculator
   ConstellationCalculator

.. toctree::
   :maxdepth: 1
   :caption: Output

   IReportFormatter
   ConsoleReportFormatter
   ITimeFormatter
   KerbinTimeFormatter

.. toctree::
   :maxdepth: 1
   :caption: Application

   Application
   internals
