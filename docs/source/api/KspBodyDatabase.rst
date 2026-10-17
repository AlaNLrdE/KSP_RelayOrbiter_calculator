KspBodyDatabase
===============

:Header: ``hpp/KspBodyDatabase.hpp``
:Source: ``src/KspBodyDatabase.cpp``
:Kind: ``final`` class
:Implements: :cpp:class:`IBodyDatabase`
:Tests: ``utest/test_KspBodyDatabase.cpp`` (19 tests)

.. uml:: ../diagrams/classes/KspBodyDatabase.puml
   :caption: KspBodyDatabase

.. cpp:class:: KspBodyDatabase final : public IBodyDatabase

   The 17 stock KSP bodies, hard-coded in a static table. Every instance
   shares the same table, so constructing one is free.

   .. cpp:member:: static const std::vector<CelestialBody> bodies_

      The catalogue, defined at namespace scope in ``KspBodyDatabase.cpp`` and
      initialised before ``main``. Entries appear in this order: Kerbol, Moho,
      Eve, Gilly, Kerbin, Mun, Minmus, Duna, Ike, Dres, Jool, Laythe, Vall,
      Tylo, Bop, Pol, Eeloo. See :ref:`bodies` for the values.

   .. cpp:function:: std::optional<CelestialBody> find(std::string_view name) const override

      Linear scan comparing :cpp:member:`CelestialBody::name` with ``name``.
      :math:`O(n)`, :math:`n = 17`.

   .. cpp:function:: std::vector<CelestialBody> all() const override

      Returns a copy of :cpp:member:`bodies_` in table order.

   .. cpp:function:: std::vector<CelestialBody> children_of(std::string_view parent_name) const override

      Filters bodies whose ``parent`` is set and equal to ``parent_name``,
      then sorts them with ``std::sort`` by ``*orbital_radius``.
      :math:`O(n + k \log k)` with :math:`k \le 7`.

      The comparator dereferences ``orbital_radius`` without checking; every
      body with a parent in the table has one.

Adding a body
-------------

Append one brace-initialised entry to ``bodies_`` in
``src/KspBodyDatabase.cpp``. Field order matches :cpp:class:`CelestialBody`.
The entry below is illustrative only — take real values from the mod's
configuration:

.. code-block:: cpp

   //  name     radius      mu            atm   rot_period  soi           orbital_radius   parent
   { "Sarnus", 5'300'000.0, 8.2117744e13, 0.0, 28'500.0, 2'358'000'000.0, 125'798'522'368.0, "Kerbol" },

No other code changes are needed: the body list, the altitude menu, the SOI
checks, and :cpp:func:`KspBodyDatabase::children_of` all pick it up. Update
``ContainsAllSeventeenBodies`` and the per-parent child-count tests.

Test coverage
-------------

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - Test
     - Verifies
   * - ``ContainsAllSeventeenBodies``
     - ``all().size() == 17``
   * - ``FindKerbinReturnsBody``
     - Lookup by name succeeds
   * - ``FindKerbinRadius`` / ``FindKerbinMu`` / ``FindKerbinAtmosphere`` / ``FindKerbinSoi``
     - Field values for Kerbin
   * - ``FindNonExistentBodyReturnsNullopt``
     - Unknown name → ``nullopt``
   * - ``FindKerbolHasNoSoi`` / ``FindKerbolHasNoParent``
     - Root body has empty optionals
   * - ``FindMunParentIsKerbin``
     - Parent link
   * - ``ChildrenOfKerbinHasTwoBodies`` / ``…ContainsMun`` / ``…ContainsMinmus``
     - Kerbin's moons
   * - ``ChildrenOfKerbinSortedByOrbitalRadius``
     - Mun before Minmus
   * - ``ChildrenOfKerbolHasSevenPlanets`` / ``ChildrenOfKerbolInCorrectOrder``
     - Moho, Eve, Kerbin, Duna, Dres, Jool, Eeloo
   * - ``ChildrenOfJoolHasFiveMoons``
     - Laythe, Vall, Tylo, Bop, Pol
   * - ``ChildrenOfDunaHasOnemoon``
     - Ike
   * - ``BodyWithNoChildrenReturnsEmpty``
     - Leaf body → empty vector
