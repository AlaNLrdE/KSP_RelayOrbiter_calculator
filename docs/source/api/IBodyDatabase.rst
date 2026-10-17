IBodyDatabase
=============

:Header: ``hpp/IBodyDatabase.hpp``
:Kind: interface (abstract class)
:Implemented by: :cpp:class:`KspBodyDatabase`
:Used by: :cpp:class:`Application`

.. uml:: ../diagrams/classes/IBodyDatabase.puml
   :caption: IBodyDatabase and its collaborators

.. cpp:class:: IBodyDatabase

   Narrow, **read-only** interface to the body catalogue. There are no methods
   for adding or modifying bodies; a different catalogue is a different
   implementation. All methods return copies, so callers can never modify the
   catalogue.

   .. cpp:function:: virtual ~IBodyDatabase() = default

   .. cpp:function:: virtual std::optional<CelestialBody> find(std::string_view name) const = 0

      Looks up a body by exact, case-sensitive name.

      :param name: body name, e.g. ``"Kerbin"``
      :returns: a copy of the body, or ``std::nullopt`` if no body has that
                name

   .. cpp:function:: virtual std::vector<CelestialBody> all() const = 0

      :returns: a copy of every body in catalogue order. Callers that need a
                particular order (the body menu sorts by name) sort the copy.

   .. cpp:function:: virtual std::vector<CelestialBody> children_of(std::string_view parent_name) const = 0

      Direct children of a body — bodies whose
      :cpp:member:`CelestialBody::parent` equals ``parent_name``.

      :param parent_name: name of the parent body
      :returns: children sorted by :cpp:member:`CelestialBody::orbital_radius`
                ascending (innermost first). Empty if the body has no
                children or does not exist.

      This ordering is part of the contract: the altitude menu lists children
      in this order.
