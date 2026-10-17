.. _overview:

Overview
========

The problem
-----------

A relay network in *Kerbal Space Program* (KSP) needs satellites spread evenly
around a body so that at least one is always in line of sight. Three satellites
at 120° spacing is the minimum for full equatorial coverage.

Launching three separate rockets and phasing each one by hand is slow and
error-prone. Carrying all three on one vessel is cheaper, but then each
satellite must be released at precisely the right moment so that the final
spacing comes out even.

The method
----------

Instead of timing each release against an angle, the carrier is put on an
**elliptical parking orbit** whose apoapsis touches the target circular orbit
and whose period is exactly :math:`\tfrac{2}{3}` of the circular period:

.. math::

   T_e = \tfrac{2}{3}\,T_c

The carrier releases a satellite at apoapsis, the satellite circularizes, and
the carrier keeps going. One parking-orbit later the carrier is back at
apoapsis, while the released satellite has travelled :math:`240°` around the
circular orbit — leaving the carrier exactly :math:`120°` behind it. Releasing
the second and third satellites on the next two passes completes an evenly
spaced constellation without ever measuring an angle in flight.

.. uml:: diagrams/seq_deployment.puml
   :caption: In-game deployment procedure

The full derivation is in :ref:`theory`.

What the calculator gives you
-----------------------------

For a chosen body and target altitude the tool reports:

* **Circular orbit** — altitude, radius, orbital velocity, and period of the
  final satellite orbit.
* **Elliptical parking orbit** — apoapsis and periapsis altitudes and radii,
  semi-major axis, period, and the :math:`T_e/T_c` ratio as a sanity check.
* **Velocities** — circular velocity and the carrier's speed at apoapsis and
  periapsis (useful for planning the burns).
* **Deployment procedure** — an 8-step checklist with the exact altitudes and
  wait times filled in.
* **Safety warnings** — when the parking orbit's periapsis would hit the
  surface or dip into the atmosphere, along with the minimum safe target
  altitude.

Helpful altitude presets are offered for every body: synchronous orbit, the
orbital altitude of one of its moons (or planets, for Kerbol), the midpoint
between two of them, or any custom altitude.

Scope
-----

* All 17 stock KSP bodies are built in (see :ref:`bodies`).
* Times are shown in the Kerbin calendar (6-hour days, 426-day years) as well
  as raw seconds.
* The calculator is purely interactive (stdin/stdout); there is no batch or
  file mode yet. :ref:`extending` describes how one could be added.
* Only the 3-satellite, 120° configuration is supported.

History
-------

The calculator started as a single Python script. It was ported to C++20 with
a SOLID, interface-based design and a GoogleTest suite (66 tests), after which
the Python version was deprecated and removed.
