.. _theory:

Orbital Mechanics
=================

This page derives every number the calculator reports. Symbols are listed in
the :ref:`glossary`.

.. figure:: ../images/orbit_geometry.svg
   :alt: To-scale drawing of the circular and parking orbits
   :width: 100%

   Target circular orbit and carrier parking orbit, drawn to scale

.. uml:: diagrams/orbit_geometry.puml
   :caption: How the orbit parameters relate to each other

The core constraint
-------------------

Let the target circular orbit have radius :math:`r_c` and period :math:`T_c`,
and let the carrier's elliptical parking orbit have period :math:`T_e`.

At :math:`t = 0` the carrier is at apoapsis and releases SAT-1, which
circularizes at :math:`r_c`. After one parking orbit the carrier is back at
apoapsis. In that time SAT-1 has moved

.. math::

   \theta_{\text{SAT-1}} = \frac{T_e}{T_c} \cdot 360°

along the circular orbit. For the carrier to be exactly 120° *behind* SAT-1,
SAT-1 must have advanced :math:`360° - 120° = 240°`:

.. math::

   \frac{T_e}{T_c} = \frac{240°}{360°} = \frac{2}{3}

Every later release repeats the same geometry, so each satellite is placed
120° behind the previous one.

.. note::

   Choosing :math:`T_e/T_c = 2/3` (a shorter parking orbit) rather than
   :math:`4/3` (a longer one) keeps the parking orbit *inside* the target orbit,
   so the carrier never leaves the body's SOI.

Deriving the parking orbit
--------------------------

Kepler's third law
~~~~~~~~~~~~~~~~~~

.. math::

   T = 2\pi \sqrt{\frac{a^3}{\mu}}

so for two orbits around the same body

.. math::

   \frac{T_e}{T_c} = \left(\frac{a_e}{r_c}\right)^{3/2} = \frac{2}{3}
   \quad\Longrightarrow\quad
   a_e = r_c \left(\frac{2}{3}\right)^{2/3} \approx 0.76314\, r_c

Apoapsis and periapsis
~~~~~~~~~~~~~~~~~~~~~~

Satellites are released at apoapsis, which must lie on the target orbit:

.. math::

   r_{apo} = r_c

From :math:`a_e = (r_{apo} + r_{peri}) / 2`:

.. math::

   r_{peri} = 2 a_e - r_c = r_c \left( 2\left(\tfrac{2}{3}\right)^{2/3} - 1 \right)
   \approx 0.52629\, r_c

Altitudes are radii minus the body radius :math:`R`:

.. math::

   h_{apo} = h_{target}, \qquad h_{peri} = r_{peri} - R

Velocities
~~~~~~~~~~

All speeds come from the **vis-viva equation**:

.. math::

   v = \sqrt{\mu \left( \frac{2}{r} - \frac{1}{a} \right)}

.. list-table::
   :header-rows: 1
   :widths: 30 30 40

   * - Quantity
     - :math:`r`, :math:`a`
     - Meaning
   * - :math:`v_c`
     - :math:`r_c`, :math:`r_c`
     - Circular velocity, :math:`\sqrt{\mu / r_c}`
   * - :math:`v_{apo}`
     - :math:`r_c`, :math:`a_e`
     - Carrier speed at apoapsis (slowest point)
   * - :math:`v_{peri}`
     - :math:`r_{peri}`, :math:`a_e`
     - Carrier speed at periapsis (fastest point)

Useful Δv figures follow directly:

* **Entering the parking orbit** (from the circular orbit at apoapsis):
  :math:`\Delta v = v_c - v_{apo}` retrograde.
* **Circularizing each satellite** after release:
  :math:`\Delta v = v_c - v_{apo}` prograde.

For Kerbin at 1000 km that is :math:`1485.68 - 1233.77 \approx 251.9` m/s.

Minimum safe altitude
---------------------

The periapsis must clear the surface. Writing
:math:`f = 2(2/3)^{2/3} - 1 \approx 0.52629`:

.. math::

   r_{peri} = f\,r_c > R
   \quad\Longrightarrow\quad
   h_{target} > \frac{R}{f} - R \approx 0.9001\, R

For a body with an atmosphere of height :math:`h_{atm}` the periapsis must
clear :math:`R + h_{atm}` instead:

.. math::

   h_{target} > \frac{R + h_{atm}}{f} - R

Because :math:`R + h_{atm} > R`, the atmosphere limit is always the stricter
one when an atmosphere exists.

.. list-table:: Minimum target altitudes
   :header-rows: 1

   * - Body
     - Surface limit
     - Atmosphere
     - Atmosphere limit
     - Binding limit
   * - Kerbin
     - 540.1 km
     - 70 km
     - 673.1 km
     - **673.1 km**
   * - Mun
     - 180.0 km
     - none
     - —
     - **180.0 km**
   * - Minmus
     - 54.0 km
     - none
     - —
     - **54.0 km**
   * - Duna
     - 288.0 km
     - 50 km
     - 383.0 km
     - **383.0 km**
   * - Eve
     - 630.1 km
     - 90 km
     - 801.1 km
     - **801.1 km**
   * - Laythe
     - 450.1 km
     - 50 km
     - 545.1 km
     - **545.1 km**
   * - Jool
     - 5,400.7 km
     - 200 km
     - 5,780.7 km
     - **5,780.7 km**

These are the values reported as ``min_h_surface`` and ``min_h_atm`` in
:cpp:class:`OrbitalResult`.

Synchronous orbit
-----------------

A synchronous orbit has a period equal to the body's sidereal rotation period
:math:`T_{rot}`. Solving Kepler's law for the radius:

.. math::

   r_{sync} = \left( \mu \left( \frac{T_{rot}}{2\pi} \right)^2 \right)^{1/3},
   \qquad h_{sync} = r_{sync} - R

For Kerbin this gives the familiar 2,863.33 km.

Sphere of influence limit
-------------------------

A satellite must stay inside the body's SOI, so the highest usable altitude is

.. math::

   h_{SOI,\max} = r_{SOI} - R

Kerbol has no SOI limit in this model.

.. _sync-outside-soi:

Bodies whose synchronous orbit is outside the SOI
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Slowly rotating or low-mass bodies have synchronous orbits beyond their SOI,
where a satellite would escape. The menu flags these and refuses the option.

.. list-table::
   :header-rows: 1

   * - Body
     - Sync orbit
     - SOI limit
     - Ratio
   * - Mun
     - 2,970.6 km
     - 2,229.6 km
     - 1.33×
   * - Minmus
     - 3,670.4 km
     - 2,187.4 km
     - 1.68×
   * - Ike
     - 1,133.9 km
     - 919.6 km
     - 1.23×
   * - Vall
     - 3,593.8 km
     - 2,106.4 km
     - 1.71×
   * - Bop
     - 2,588.2 km
     - 1,156.1 km
     - 2.24×
   * - Pol
     - 2,415.1 km
     - 998.1 km
     - 2.42×
   * - Laythe
     - 4,686.3 km
     - 3,223.6 km
     - 1.45×
   * - Tylo
     - 14,157.9 km
     - 10,256.5 km
     - 1.38×
   * - Moho
     - 18,173.2 km
     - 9,396.7 km
     - 1.93×

Numerical notes
---------------

* All formulas are closed-form; there is no iteration or numerical solver.
* :math:`(2/3)^{2/3}` is computed with ``std::pow`` at double precision
  (~15–16 significant digits). The test suite checks
  :math:`T_e/T_c = 2/3` to :math:`10^{-9}` and conservation of specific orbital
  energy along the parking orbit to a relative :math:`10^{-9}`.
* The model is pure two-body Keplerian motion: it ignores the parent body's
  gravity, SOI transitions, and non-spherical bodies, exactly like KSP's
  patched-conic physics.
