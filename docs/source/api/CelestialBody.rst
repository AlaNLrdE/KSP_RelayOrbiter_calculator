CelestialBody
=============

:Header: ``hpp/CelestialBody.hpp``
:Kind: plain data structure (aggregate)

.. uml:: ../diagrams/classes/CelestialBody.puml
   :caption: CelestialBody and its users

.. cpp:struct:: CelestialBody

   Physical and orbital parameters of one celestial body. It has no behaviour
   and no invariants enforced in code; the catalogue in
   :cpp:class:`KspBodyDatabase` is the source of valid instances.

   Being an aggregate, it can be built with designated initializers, which is
   how the test fixtures create bodies:

   .. code-block:: cpp

      CelestialBody kerbin{
          .name           = "Kerbin",
          .radius         = 600'000.0,
          .mu             = 3.5316e12,
          .atm            = 70'000.0,
          .rot_period     = 21'549.425,
          .soi            = 84'159'286.0,
          .orbital_radius = 13'599'840'256.0,
          .parent         = "Kerbol",
      };

   .. cpp:member:: std::string name

      Unique, case-sensitive body name (``"Kerbin"``). Used as the lookup key
      by :cpp:func:`IBodyDatabase::find` and as the parent reference by other
      bodies.

   .. cpp:member:: double radius

      Mean equatorial radius in metres. Converts between altitude (above the
      surface) and radius (from the centre).

   .. cpp:member:: double mu

      Standard gravitational parameter :math:`\mu = GM` in m³/s². Every
      period and velocity depends on it.

   .. cpp:member:: double atm

      Altitude of the top of the atmosphere in metres, or ``0.0`` for an
      airless body. Used to reject custom altitudes inside the atmosphere and
      to compute :cpp:member:`OrbitalResult::atm_ok` and
      :cpp:member:`OrbitalResult::min_h_atm`.

   .. cpp:member:: double rot_period

      Sidereal rotation period in seconds. Only used to compute the
      synchronous orbit altitude.

   .. cpp:member:: std::optional<double> soi

      Sphere-of-influence radius in metres, measured from the body's centre.
      ``std::nullopt`` for Kerbol, which has no practical SOI limit.

   .. cpp:member:: std::optional<double> orbital_radius

      Semi-major axis of the body's orbit around its parent, in metres.
      ``std::nullopt`` for Kerbol. Used to sort children and to offer the
      "at a moon's orbital distance" and "midpoint" altitude presets.

   .. cpp:member:: std::optional<std::string> parent

      Name of the body this one orbits, or ``std::nullopt`` for Kerbol.
      Together these fields form the body hierarchy shown in :ref:`bodies`.

Invariants assumed by the code
------------------------------

* ``radius > 0`` and ``mu > 0``.
* ``soi``, ``orbital_radius`` and ``parent`` are either all set or all empty
  (empty only for the root body).
* If ``soi`` is set, ``*soi > radius``.
* ``parent``, when set, names another body in the same catalogue.

Breaking these does not cause compile errors but produces meaningless results
or, for :cpp:func:`KspBodyDatabase::children_of`, dereferences an empty
optional.
