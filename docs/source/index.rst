KSP Relay Constellation Calculator
==================================

An interactive C++20 command-line tool that computes the orbital parameters
needed to deploy **three relay satellites spaced exactly 120° apart** from a
single carrier vessel in *Kerbal Space Program* — no in-flight angle
measurement, no mods.

The carrier is placed on an elliptical *parking orbit* whose period is exactly
two thirds of the target circular orbit's period. Releasing one satellite at
apoapsis on each pass spaces the constellation evenly by construction. The
calculator works out that parking orbit — altitudes, periods, velocities, and
safety limits — for any of the 17 stock bodies.

.. uml:: diagrams/components.puml
   :caption: The application at a glance

.. toctree::
   :maxdepth: 2
   :caption: User documentation

   overview
   getting_started
   user_guide
   theory

.. toctree::
   :maxdepth: 2
   :caption: Developer documentation

   architecture
   api/index
   testing
   extending
   limitations

.. toctree::
   :maxdepth: 1
   :caption: Reference

   bodies
   glossary
