ConstellationCalculator
=======================

:Header: ``hpp/ConstellationCalculator.hpp``
:Source: ``src/ConstellationCalculator.cpp``
:Kind: ``final`` class
:Implements: :cpp:class:`IConstellationCalculator`
:Depends on: :cpp:class:`IOrbitalMechanics`
:Tests: ``utest/test_ConstellationCalculator.cpp`` (20 tests)

.. uml:: ../diagrams/classes/ConstellationCalculator.puml
   :caption: ConstellationCalculator

.. cpp:class:: ConstellationCalculator final : public IConstellationCalculator

   Implements the parking-orbit derivation. Periods and velocities are
   delegated to the injected :cpp:class:`IOrbitalMechanics`; the geometry
   (semi-major axis, apsides, safety limits) is computed directly.

   .. cpp:function:: explicit ConstellationCalculator(std::shared_ptr<IOrbitalMechanics> mechanics)

      :param mechanics: physics implementation; stored in
                        :cpp:member:`mechanics_`. Must not be null.

   .. cpp:function:: OrbitalResult calculate(const CelestialBody &body, double h_target) const override

      Computes, in order:

      #. ``r_c = R + h_target``
      #. ``T_c = mechanics_->orbital_period(r_c, μ)``
      #. ``v_c = mechanics_->vis_viva(r_c, r_c, μ)``
      #. ``T_e = T_c · 2/3``
      #. ``a_e = r_c · (2/3)^(2/3)``
      #. ``r_apo = r_c``, ``h_apo = h_target``
      #. ``r_peri = 2·a_e − r_c``, ``h_peri = r_peri − R``
      #. ``v_apo = vis_viva(r_apo, a_e, μ)``, ``v_peri = vis_viva(r_peri, a_e, μ)``
      #. ``surface_ok = h_peri > 0``;
         ``atm_ok = atm ≤ 0 || h_peri > atm``
      #. ``factor = 2·(2/3)^(2/3) − 1``;
         ``min_h_surface = R/factor − R``
      #. If ``atm > 0``: ``min_h_atm = (R + atm)/factor − R``

      Exactly three calls to :cpp:func:`IOrbitalMechanics::vis_viva` and one
      to :cpp:func:`IOrbitalMechanics::orbital_period` are made.
      :math:`T_e` is derived from :math:`T_c` rather than computed from
      :math:`a_e`, so the ratio is exactly :math:`2/3` up to one rounding.

   .. cpp:member:: private std::shared_ptr<IOrbitalMechanics> mechanics_

      Injected physics implementation.

.. uml:: ../diagrams/seq_calculate.puml
   :caption: calculate() call sequence

Worked example
--------------

Kerbin (:math:`R` = 600 km, :math:`\mu` = 3.5316×10¹² m³/s²,
:math:`h_{atm}` = 70 km) at :math:`h_{target}` = 1000 km:

.. list-table::
   :header-rows: 1

   * - Field
     - Value
   * - ``r_c``
     - 1,600,000 m
   * - ``T_c``
     - 6,766.66 s
   * - ``v_c``
     - 1,485.682 m/s
   * - ``T_e``
     - 4,511.10 s
   * - ``a_e``
     - 1,221,029 m
   * - ``r_peri`` / ``h_peri``
     - 842,057 m / 242,057 m
   * - ``v_apo`` / ``v_peri``
     - 1,233.768 / 2,344.294 m/s
   * - ``surface_ok`` / ``atm_ok``
     - ``true`` / ``true``
   * - ``min_h_surface``
     - 540,065 m
   * - ``min_h_atm``
     - 673,073 m

Test coverage
-------------

All tests inject a real :cpp:class:`OrbitalMechanics`; there are no mocks.

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - Test
     - Verifies
   * - ``BodyNamePropagated`` / ``TargetAltitudePropagated``
     - Input echoed into the result
   * - ``CircularOrbitRadiusEqualsRadiusPlusAltitude``
     - ``r_c = R + h``
   * - ``ApoapsisRadiusEqualsCircularRadius`` / ``ApoapsisAltitudeEqualsTargetAltitude``
     - Apoapsis on the target orbit
   * - ``EllipticalPeriodIsTwoThirdsCircular``
     - :math:`T_e/T_c = 2/3`
   * - ``SemiMajorAxisFromKepler``
     - :math:`a_e = r_c(2/3)^{2/3}`
   * - ``PeriapsisRadiusIs2ae_minus_rc`` / ``PeriapsisAltitudeEqualsRperiMinusBodyRadius``
     - Periapsis geometry
   * - ``CircularVelocityFromVisViva``
     - :math:`v_c = \sqrt{\mu/r_c}`
   * - ``ApoapsisVelocityLessThanCircular`` / ``PeriapsisVelocityGreaterThanApoApsis``
     - Velocity ordering
   * - ``SurfaceOkWhenPeriapsisAboveSurface`` / ``SurfaceNotOkWhenPeriapsisBelowSurface``
     - ``surface_ok`` flag
   * - ``AtmOkWhenPeriapsisAboveAtmosphere`` / ``AtmNotOkWhenPeriapsisBelowAtmosphere`` / ``NoAtmBodyAtmOkIsAlwaysTrue``
     - ``atm_ok`` flag
   * - ``MinHSurfaceIsPositive``
     - Surface limit sanity
   * - ``MinHAtmPresentForAtmosphericBody`` / ``MinHAtmAbsentForAirlessBody``
     - ``min_h_atm`` optional
