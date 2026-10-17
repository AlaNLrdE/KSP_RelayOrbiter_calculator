.. _user-guide:

User Guide
==========

This page walks through one interactive session: choosing a body, picking an
altitude, reading the report, and flying the deployment.

.. uml:: diagrams/state_app.puml
   :caption: The session loop

Starting the program
--------------------

.. code-block:: sh

   ./build/obj/relay_calculator

The program prints a banner and the list of bodies::

   ==============================================================
     KSP -- 3-Satellite Relay Constellation Calculator
     Method: elliptical parking orbit, T_e = (2/3) T_c
   ==============================================================

   Available bodies:
      1. Bop         R=65 km  (orbits Jool)
      2. Dres        R=138 km  (orbits Kerbol)
      3. Duna        R=320 km  atm up50 km  (orbits Kerbol)
      4. Eeloo       R=210 km  (orbits Kerbol)
      5. Eve         R=700 km  atm up90 km  (orbits Kerbol)
      6. Gilly       R=13 km  (orbits Eve)
      7. Ike         R=130 km  (orbits Duna)
      8. Jool        R=6000 km  atm up200 km  (orbits Kerbol)
      9. Kerbin      R=600 km  atm up70 km  (orbits Kerbol)
     10. Kerbol      R=261600 km  (the star)
     11. Laythe      R=500 km  atm up50 km  (orbits Jool)
     12. Minmus      R=60 km  (orbits Kerbin)
     13. Moho        R=250 km  (orbits Kerbol)
     14. Mun         R=200 km  (orbits Kerbin)
     15. Pol         R=44 km  (orbits Jool)
     16. Tylo        R=600 km  (orbits Jool)
     17. Vall        R=300 km  (orbits Jool)

   Select body (number or name):

Step 1 — Choose a body
----------------------

Bodies are listed **alphabetically**. Each line shows the radius, the
atmosphere ceiling (if any), and the parent body.

Enter either the **list number** or the **exact name**. Names are
case-sensitive: ``Kerbin`` works, ``kerbin`` does not. Anything else prints
``Invalid selection -- enter a number or exact name.`` and asks again.

.. uml:: diagrams/act_select_body.puml
   :caption: Body selection logic

Step 2 — Choose a target altitude
---------------------------------

The altitude menu is built specifically for the selected body. The header
mentions the atmosphere ceiling when there is one::

   Target circular orbit altitude above Kerbin  (atmosphere ends at 70 km)
     1. Synchronous orbit  ->  2863.33 km
     2. At a moon's orbital distance
     3. Midpoint between two moons
     4. Custom altitude

.. list-table:: Menu options
   :header-rows: 1
   :widths: 30 20 50

   * - Option
     - Shown when
     - What it does
   * - Synchronous orbit
     - Always
     - Uses the altitude where the orbital period equals the body's sidereal
       rotation period. If that altitude is outside the body's sphere of
       influence (SOI) the label says ``(exceeds SOI limit …)`` and choosing
       it explains why and returns to the menu.
   * - At a moon's / planet's orbital distance
     - The body has at least one child
     - Lists the children (moons, or planets for Kerbol) with their orbital
       altitudes above this body and uses the one you pick.
   * - Midpoint between two moons / planets
     - The body has two or more children
     - Asks for two *different* children and uses the altitude halfway
       between their orbital radii.
   * - Custom altitude
     - Always (last option)
     - Asks for an altitude in **kilometres**.

"Moon" is used for planets and moons; "planet" is used for Kerbol.

.. uml:: diagrams/act_ask_altitude.puml
   :caption: Altitude menu construction and dispatch

Custom altitude rules
~~~~~~~~~~~~~~~~~~~~~

A custom altitude is accepted only when it is:

#. a number (parsed with ``std::stod``),
#. greater than zero,
#. not inside the atmosphere, and
#. not above the SOI limit (SOI radius minus body radius).

Each failed check prints a specific message and asks again.

.. uml:: diagrams/act_custom_altitude.puml
   :caption: Custom altitude validation chain

.. note::

   These checks apply to the **target circular orbit**. A target that passes
   them can still produce a parking orbit whose periapsis is too low; that is
   reported in the results (see :ref:`warnings`).

Step 3 — Read the report
------------------------

The report has four sections. Example for Kerbin at 1000 km::

   ==============================================================
     RESULTS — Kerbin / target altitude 1000.00 km
   ==============================================================

     CIRCULAR ORBIT  (final satellite orbit)
     ------------------------------------------------------------
     Altitude                        : 1000.00  km
     Radius                          : 1600.00  km
     Orbital velocity                : 1485.682  m/s
     Orbital period                  : 0y 0d 1h 52m 46.7s  (6766.66 s)

     ELLIPTICAL PARKING ORBIT  (carrier vessel)
     ------------------------------------------------------------
     Apoapsis altitude               : 1000.00  km  (= target)
     Apoapsis radius                 : 1600.00  km
     Periapsis altitude              : 242.06  km  OK
     Periapsis radius                : 842.06  km
     Semi-major axis                 : 1221.03  km
     Orbital period                  : 0y 0d 1h 15m 11.1s  (4511.10 s)
     T_e / T_c ratio                 : 0.666667  (target 0.666667)

     VELOCITIES
     ------------------------------------------------------------
     Circular orbit (at apoapsis alt): 1485.682  m/s
     Carrier at apoapsis             : 1233.768  m/s
     Carrier at periapsis            : 2344.294  m/s

     DEPLOYMENT PROCEDURE
     ------------------------------------------------------------
      1. Circularize carrier at 1000.00 km.
      2. Burn retrograde at apoapsis to set periapsis to 242.06 km.
          (carrier is now in the elliptical parking orbit)
      3. At apoapsis: release SAT-1.  SAT-1 circularizes.
      4. Carrier completes 1 elliptical orbit  (0y 0d 1h 15m 11.1s).
          -> SAT-1 is now 120 deg ahead at apoapsis.
      5. At apoapsis: release SAT-2.  SAT-2 circularizes.
      6. Carrier completes 1 more elliptical orbit  (0y 0d 1h 15m 11.1s).
          -> SAT-1 at 120 deg, SAT-2 at 240 deg ahead.
      7. At apoapsis: release SAT-3.  SAT-3 circularizes.
      8. Constellation complete: 3 satellites, 120 deg spacing.

   ==============================================================

How to read it:

* **Altitudes** are above the surface; **radii** are from the body's centre.
  Both are in kilometres with two decimals.
* **Velocities** are in m/s with three decimals.
* **Periods** are given in the Kerbin calendar (``Xy Xd Xh Xm X.Xs``, with
  6-hour days and 426-day years) followed by raw seconds.
* The **periapsis altitude** row ends in ``OK`` when the periapsis clears both
  the surface and the atmosphere, otherwise in ``BELOW SURFACE`` or
  ``INSIDE ATMOSPHERE``.
* **T_e / T_c ratio** should always read ``0.666667``; it is printed as a
  sanity check.
* **Carrier at apoapsis** is always lower than the circular velocity. The
  difference is the Δv each satellite needs to circularize after release.

.. _warnings:

Safety warnings
~~~~~~~~~~~~~~~

If the periapsis would be underground or inside the atmosphere, a warning
block follows the procedure. For Kerbin at 600 km::

     !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     Periapsis is inside the atmosphere!
        Minimum target altitude (clear of atmosphere): 673.1 km
     !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

The underground variant reports the minimum altitude for clearing the surface
instead. Rerun with a target at or above the suggested minimum.

Step 4 — Repeat or exit
-----------------------

After the report::

   Calculate for another body/altitude? [y/N]:

``y`` or ``yes`` (any case) starts over at body selection. Anything else —
including just pressing Enter — prints ``Good luck with the mission!`` and
exits.

Flying the deployment
---------------------

#. Launch the carrier with all three satellites and circularize at the
   **target altitude**.
#. At any point on that orbit (which is now apoapsis), burn **retrograde**
   until the periapsis matches the reported **periapsis altitude**. The carrier
   is now on the parking orbit.
#. At apoapsis, decouple **SAT-1** and burn it prograde to circularize at the
   target altitude.
#. Let the carrier complete **one parking orbit** (the reported elliptical
   period).
#. At apoapsis, release and circularize **SAT-2**.
#. Wait one more parking orbit.
#. At apoapsis, release and circularize **SAT-3**.

The three satellites end up 120° apart. Fine-tune each satellite's orbital
period to match the others exactly so they do not drift over time.

Tips
----

* **Synchronous orbits** keep each satellite over a fixed longitude, which is
  convenient for ground stations, but are unavailable around several small
  moons (see :ref:`sync-outside-soi`).
* **Higher is safer**: a higher target raises the periapsis too, because the
  periapsis radius is always about 52.6 % of the target orbit radius.
* For **atmospheric bodies**, the atmosphere — not the surface — always sets
  the minimum target altitude.
