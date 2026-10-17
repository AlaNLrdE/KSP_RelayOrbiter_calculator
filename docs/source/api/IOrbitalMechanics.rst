IOrbitalMechanics
=================

:Header: ``hpp/IOrbitalMechanics.hpp``
:Kind: interface (abstract class)
:Implemented by: :cpp:class:`OrbitalMechanics`
:Used by: :cpp:class:`ConstellationCalculator`, :cpp:class:`Application`
          (altitude menu)

.. uml:: ../diagrams/classes/IOrbitalMechanics.puml
   :caption: IOrbitalMechanics and its collaborators

.. cpp:class:: IOrbitalMechanics

   Pure orbital-mechanics computations with no knowledge of the body database
   or the user interface. All methods are ``const`` and ``[[nodiscard]]``;
   implementations are expected to be stateless and side-effect free.

   .. cpp:function:: virtual ~IOrbitalMechanics() = default

   .. cpp:function:: virtual double orbital_period(double semi_major_axis, double mu) const = 0

      Orbital period from Kepler's third law:

      .. math:: T = 2\pi \sqrt{\frac{a^3}{\mu}}

      :param semi_major_axis: semi-major axis :math:`a` in metres (the radius,
                              for a circular orbit)
      :param mu: gravitational parameter :math:`\mu` in m³/s²
      :returns: period in seconds

   .. cpp:function:: virtual double vis_viva(double r, double semi_major_axis, double mu) const = 0

      Orbital speed at distance :math:`r` from the centre on an orbit with
      semi-major axis :math:`a`:

      .. math:: v = \sqrt{\mu \left(\frac{2}{r} - \frac{1}{a}\right)}

      :param r: current distance from the body's centre, metres
      :param semi_major_axis: semi-major axis of the orbit, metres
      :param mu: gravitational parameter, m³/s²
      :returns: speed in m/s

      With :math:`r = a` this reduces to the circular velocity
      :math:`\sqrt{\mu/r}`.

   .. cpp:function:: virtual std::optional<double> sync_orbit_altitude(const CelestialBody &body) const = 0

      Altitude above the surface at which the orbital period equals the body's
      sidereal rotation period:

      .. math::

         r_{sync} = \left(\mu\left(\frac{T_{rot}}{2\pi}\right)^2\right)^{1/3},
         \qquad h_{sync} = r_{sync} - R

      :param body: body providing :cpp:member:`CelestialBody::mu`,
                   :cpp:member:`CelestialBody::rot_period` and
                   :cpp:member:`CelestialBody::radius`
      :returns: altitude in metres, or ``std::nullopt`` if no usable
                synchronous orbit exists

      The SOI is **not** checked here; callers compare the result with
      :cpp:func:`soi_max_altitude`.

   .. cpp:function:: virtual std::optional<double> soi_max_altitude(const CelestialBody &body) const = 0

      Highest altitude still inside the body's sphere of influence:

      .. math:: h_{SOI,\max} = r_{SOI} - R

      :param body: body providing :cpp:member:`CelestialBody::soi` and
                   :cpp:member:`CelestialBody::radius`
      :returns: altitude in metres, or ``std::nullopt`` when the body has no
                SOI (Kerbol)
