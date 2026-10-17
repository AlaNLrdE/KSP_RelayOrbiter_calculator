IConstellationCalculator
========================

:Header: ``hpp/IConstellationCalculator.hpp``
:Kind: interface (abstract class)
:Implemented by: :cpp:class:`ConstellationCalculator`
:Used by: :cpp:class:`Application`

.. uml:: ../diagrams/classes/IConstellationCalculator.puml
   :caption: IConstellationCalculator and its collaborators

.. cpp:class:: IConstellationCalculator

   Turns a body and a target altitude into a complete parking-orbit solution.
   This is the domain entry point of the program.

   .. cpp:function:: virtual ~IConstellationCalculator() = default

   .. cpp:function:: virtual OrbitalResult calculate(const CelestialBody &body, double h_target) const = 0

      Applies the :math:`T_e = \tfrac{2}{3}T_c` constraint (see
      :ref:`theory`) and fills every field of :cpp:class:`OrbitalResult`.

      :param body: the body to orbit
      :param h_target: target circular orbit altitude above the surface, in
                       metres
      :returns: the complete result, including safety flags

      **Contract**

      * Pure: no I/O, no observable side effects, same output for the same
        input.
      * Does not validate ``h_target``. Out-of-range input (negative, above
        the SOI) is computed anyway; it is the caller's job to validate. An
        unsafe *periapsis* is reported through
        :cpp:member:`OrbitalResult::surface_ok` and
        :cpp:member:`OrbitalResult::atm_ok`, not rejected.
