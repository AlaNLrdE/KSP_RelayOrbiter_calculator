OrbitalMechanics
================

:Header: ``hpp/OrbitalMechanics.hpp``
:Source: ``src/OrbitalMechanics.cpp``
:Kind: ``final`` class
:Implements: :cpp:class:`IOrbitalMechanics`
:Tests: ``utest/test_OrbitalMechanics.cpp`` (15 tests)

.. uml:: ../diagrams/classes/OrbitalMechanics.puml
   :caption: OrbitalMechanics

.. cpp:class:: OrbitalMechanics final : public IOrbitalMechanics

   Closed-form implementation of :cpp:class:`IOrbitalMechanics`. Stateless —
   it has no data members, so one instance can be shared freely and every
   method is :math:`O(1)`.

   .. cpp:function:: double orbital_period(double semi_major_axis, double mu) const override

      Computes ``2π · sqrt(a³ / μ)`` using ``std::numbers::pi`` and
      ``std::sqrt``. The cube is written as ``a * a * a`` rather than
      ``std::pow(a, 3)``.

   .. cpp:function:: double vis_viva(double r, double semi_major_axis, double mu) const override

      Computes ``sqrt(μ · (2/r − 1/a))``. No domain check is performed: if
      ``r > 2a`` the argument is negative and the result is ``NaN``. That
      cannot happen for the orbits this program builds.

   .. cpp:function:: std::optional<double> sync_orbit_altitude(const CelestialBody &body) const override

      Computes ``cbrt(μ · (T_rot / 2π)²) − R`` with ``std::cbrt``.

      .. important::

         This implementation **always returns a value**. Although the
         interface allows ``std::nullopt``, the result is not checked against
         the surface or atmosphere. Every stock body's synchronous altitude is
         above its surface and atmosphere, so this does not matter in practice
         (and the test ``SyncOrbitKerbolReturnsValue`` asserts that a value is
         returned even for Kerbol).

   .. cpp:function:: std::optional<double> soi_max_altitude(const CelestialBody &body) const override

      Returns ``*body.soi − body.radius``, or ``std::nullopt`` when
      :cpp:member:`CelestialBody::soi` is empty.

Reference values
----------------

.. list-table::
   :header-rows: 1

   * - Call
     - Result
   * - ``sync_orbit_altitude(Kerbin)``
     - ≈ 2,863,334 m
   * - ``soi_max_altitude(Kerbin)``
     - 83,559,286 m
   * - ``soi_max_altitude(Mun)``
     - 2,229,559 m
   * - ``soi_max_altitude(Kerbol)``
     - ``std::nullopt``
   * - ``orbital_period(R_Kerbin + h_sync, μ_Kerbin)``
     - ≈ 21,549.425 s (Kerbin's rotation period)

Test coverage
-------------

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - Test
     - Verifies
   * - ``KeplerFormula_T2_mu_equals_4pi2_a3``
     - :math:`T^2\mu = 4\pi^2 a^3`
   * - ``PeriodScalesWithKeplerLaw``
     - Doubling :math:`a` multiplies :math:`T` by :math:`2^{3/2}` (tol. 1e-9)
   * - ``KerbinKSOPeriodEqualsRotationPeriod``
     - Period at synchronous altitude equals rotation period (±0.01 s)
   * - ``EllipticalPeriodRatioIsTwoThirds``
     - :math:`T_e/T_c = 2/3` when :math:`a_e = r_c(2/3)^{2/3}` (tol. 1e-9)
   * - ``CircularOrbitIdentity``
     - ``vis_viva(r, r, μ) == sqrt(μ/r)`` (tol. 1e-6)
   * - ``PeriapsisSpeedGreaterThanApoapsis``
     - Speed is higher at periapsis
   * - ``ApoapsisSpeeedLessThanCircular``
     - Carrier is slower than circular at apoapsis
   * - ``EnergyConservationAlongOrbit``
     - Specific energy :math:`v^2/2 - \mu/r` equals :math:`-\mu/2a` at both
       apsides (relative 1e-9)
   * - ``SyncOrbitAltitudeKerbinApprox2863km``
     - Kerbin synchronous altitude ≈ 2,863,334 m (±500 m)
   * - ``SyncOrbitPeriodMatchesBodyRotation``
     - Round-trip period check (±0.01 s)
   * - ``SyncOrbitKerbolReturnsValue``
     - Kerbol returns a value
   * - ``SyncOrbitAboveSurface``
     - Kerbin's synchronous altitude is above surface and atmosphere
   * - ``SoiMaxAltitudeKerbolReturnsNullopt``
     - No SOI → ``nullopt``
   * - ``SoiMaxAltitudeKerbinEqualsSOIMinusRadius``
     - Kerbin SOI limit (±1 m)
   * - ``SoiMaxAltitudeMun``
     - Mun SOI limit (±1 m)
