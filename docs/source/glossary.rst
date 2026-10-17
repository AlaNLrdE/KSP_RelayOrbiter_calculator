.. _glossary:

Glossary
========

Symbols
-------

.. list-table::
   :header-rows: 1
   :widths: 15 55 15 15

   * - Symbol
     - Meaning
     - Unit
     - Field
   * - :math:`R`
     - Body radius
     - m
     - ``CelestialBody::radius``
   * - :math:`\mu`
     - Standard gravitational parameter, :math:`GM`
     - m³/s²
     - ``CelestialBody::mu``
   * - :math:`h_{atm}`
     - Atmosphere height
     - m
     - ``CelestialBody::atm``
   * - :math:`T_{rot}`
     - Sidereal rotation period
     - s
     - ``CelestialBody::rot_period``
   * - :math:`h_{target}`
     - Target circular orbit altitude
     - m
     - ``OrbitalResult::h_target``
   * - :math:`r_c`
     - Circular orbit radius
     - m
     - ``OrbitalResult::r_c``
   * - :math:`T_c`
     - Circular orbit period
     - s
     - ``OrbitalResult::T_c``
   * - :math:`v_c`
     - Circular orbit velocity
     - m/s
     - ``OrbitalResult::v_c``
   * - :math:`T_e`
     - Parking orbit period
     - s
     - ``OrbitalResult::T_e``
   * - :math:`a_e`
     - Parking orbit semi-major axis
     - m
     - ``OrbitalResult::a_e``
   * - :math:`r_{apo}`, :math:`h_{apo}`
     - Apoapsis radius / altitude
     - m
     - ``OrbitalResult::r_apo``, ``h_apo``
   * - :math:`r_{peri}`, :math:`h_{peri}`
     - Periapsis radius / altitude
     - m
     - ``OrbitalResult::r_peri``, ``h_peri``
   * - :math:`v_{apo}`, :math:`v_{peri}`
     - Carrier speed at apoapsis / periapsis
     - m/s
     - ``OrbitalResult::v_apo``, ``v_peri``
   * - :math:`f`
     - :math:`2(2/3)^{2/3} - 1 \approx 0.52629`, ratio :math:`r_{peri}/r_c`
     - —
     - local ``factor``

Terms
-----

Altitude
   Distance above the body's surface. Contrast with *radius*.

Apoapsis
   Highest point of an orbit. Satellites are released here.

Carrier
   The vessel that carries all three satellites and flies the parking orbit.

Circularize
   Burn prograde at apoapsis (or retrograde at periapsis) until the orbit is
   circular.

Composition root
   The single place (``main.cpp``) where concrete objects are created and
   wired together.

Kerbin calendar
   KSP's default time display: 6-hour days and 426-day years.

Parking orbit
   The carrier's elliptical orbit, with apoapsis on the target orbit and
   period :math:`\tfrac{2}{3}T_c`.

Periapsis
   Lowest point of an orbit. Must clear the surface and atmosphere.

Radius
   Distance from the body's centre. Radius = altitude + body radius.

Sidereal rotation period
   Time for one full rotation relative to the stars.

Sphere of influence (SOI)
   Region where a body's gravity dominates. A satellite outside it is no
   longer orbiting that body.

Synchronous orbit
   Circular orbit whose period equals the body's sidereal rotation period, so
   the satellite stays over the same longitude. Over the equator it is
   *stationary*.

Vis-viva equation
   :math:`v = \sqrt{\mu(2/r - 1/a)}` — orbital speed at any point of a
   Keplerian orbit.
