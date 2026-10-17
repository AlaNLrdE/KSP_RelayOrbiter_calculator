.. _bodies:

Supported Bodies
================

All 17 stock KSP bodies are defined in ``src/KspBodyDatabase.cpp``.

Hierarchy
---------

.. uml:: diagrams/body_hierarchy.puml
   :caption: Parent/child relationships

Children are always listed innermost first (sorted by orbital radius).

Summary
-------

.. list-table::
   :header-rows: 1

   * - Body
     - Radius
     - μ (m³/s²)
     - Atmosphere
     - SOI radius
     - Parent
     - Children
   * - Kerbol
     - 261,600 km
     - 1.172×10¹⁸
     - none
     - unlimited
     - —
     - Moho, Eve, Kerbin, Duna, Dres, Jool, Eeloo
   * - Moho
     - 250 km
     - 1.686×10¹¹
     - none
     - 9,647 km
     - Kerbol
     - —
   * - Eve
     - 700 km
     - 8.172×10¹²
     - 90 km
     - 85,109 km
     - Kerbol
     - Gilly
   * - Gilly
     - 13 km
     - 8.289×10⁶
     - none
     - 126 km
     - Eve
     - —
   * - Kerbin
     - 600 km
     - 3.532×10¹²
     - 70 km
     - 84,159 km
     - Kerbol
     - Mun, Minmus
   * - Mun
     - 200 km
     - 6.514×10¹⁰
     - none
     - 2,430 km
     - Kerbin
     - —
   * - Minmus
     - 60 km
     - 1.766×10⁹
     - none
     - 2,247 km
     - Kerbin
     - —
   * - Duna
     - 320 km
     - 3.014×10¹¹
     - 50 km
     - 47,922 km
     - Kerbol
     - Ike
   * - Ike
     - 130 km
     - 1.857×10¹⁰
     - none
     - 1,050 km
     - Duna
     - —
   * - Dres
     - 138 km
     - 2.148×10¹⁰
     - none
     - 32,833 km
     - Kerbol
     - —
   * - Jool
     - 6,000 km
     - 2.825×10¹⁴
     - 200 km
     - 2,455,985 km
     - Kerbol
     - Laythe, Vall, Tylo, Bop, Pol
   * - Laythe
     - 500 km
     - 1.962×10¹²
     - 50 km
     - 3,724 km
     - Jool
     - —
   * - Vall
     - 300 km
     - 2.075×10¹¹
     - none
     - 2,406 km
     - Jool
     - —
   * - Tylo
     - 600 km
     - 2.825×10¹²
     - none
     - 10,857 km
     - Jool
     - —
   * - Bop
     - 65 km
     - 2.487×10⁹
     - none
     - 1,221 km
     - Jool
     - —
   * - Pol
     - 44 km
     - 7.217×10⁸
     - none
     - 1,042 km
     - Jool
     - —
   * - Eeloo
     - 210 km
     - 7.441×10¹⁰
     - none
     - 119,083 km
     - Kerbol
     - —

Raw catalogue values
--------------------

Exact values as stored in :cpp:member:`KspBodyDatabase::bodies_`
(SI units):

.. list-table::
   :header-rows: 1

   * - Body
     - radius (m)
     - mu (m³/s²)
     - atm (m)
     - rot_period (s)
     - soi (m)
     - orbital_radius (m)
   * - Kerbol
     - 261,600,000
     - 1.1723328e18
     - 0
     - 432,000
     - —
     - —
   * - Moho
     - 250,000
     - 1.6860938e11
     - 0
     - 1,210,000
     - 9,646,663
     - 5,263,138,304
   * - Eve
     - 700,000
     - 8.1717302e12
     - 90,000
     - 80,500
     - 85,109,365
     - 9,832,684,544
   * - Gilly
     - 13,000
     - 8,289,450
     - 0
     - 28,255
     - 126,123
     - 31,500,000
   * - Kerbin
     - 600,000
     - 3.5316e12
     - 70,000
     - 21,549.425
     - 84,159,286
     - 13,599,840,256
   * - Mun
     - 200,000
     - 6.5138398e10
     - 0
     - 138,984.38
     - 2,429,559
     - 12,000,000
   * - Minmus
     - 60,000
     - 1.7658e9
     - 0
     - 1,077,311.08
     - 2,247,428
     - 47,000,000
   * - Duna
     - 320,000
     - 3.0136321e11
     - 50,000
     - 65,517.859
     - 47,921,949
     - 20,726,155,264
   * - Ike
     - 130,000
     - 1.8568369e10
     - 0
     - 65,517.862
     - 1,049,598
     - 3,200,000
   * - Dres
     - 138,000
     - 2.1484489e10
     - 0
     - 34,800
     - 32,832,840
     - 40,839,348,203
   * - Jool
     - 6,000,000
     - 2.82528e14
     - 200,000
     - 36,000
     - 2,455,985,200
     - 68,773,560,320
   * - Laythe
     - 500,000
     - 1.962e12
     - 50,000
     - 52,980.879
     - 3,723,645
     - 27,184,000
   * - Vall
     - 300,000
     - 2.075815e11
     - 0
     - 105,962.09
     - 2,406,401
     - 43,152,000
   * - Tylo
     - 600,000
     - 2.82528e12
     - 0
     - 211,926.36
     - 10,856,518
     - 68,500,000
   * - Bop
     - 65,000
     - 2.4868349e9
     - 0
     - 544,507.4
     - 1,221,060
     - 128,500,000
   * - Pol
     - 44,000
     - 7.2170208e8
     - 0
     - 901,902.62
     - 1,042,138
     - 179,890,000
   * - Eeloo
     - 210,000
     - 7.4410815e10
     - 0
     - 19,460
     - 119,082,940
     - 113,549,713,200

Derived altitudes
-----------------

.. list-table::
   :header-rows: 1

   * - Body
     - Synchronous altitude
     - SOI limit
     - Sync usable?
   * - Kerbin
     - 2,863.33 km
     - 83,559.29 km
     - yes
   * - Mun
     - 2,970.6 km
     - 2,229.6 km
     - no
   * - Minmus
     - 3,670.4 km
     - 2,187.4 km
     - no
   * - Ike
     - 1,133.9 km
     - 919.6 km
     - no
   * - Vall
     - 3,593.8 km
     - 2,106.4 km
     - no
   * - Bop
     - 2,588.2 km
     - 1,156.1 km
     - no
   * - Pol
     - 2,415.1 km
     - 998.1 km
     - no
   * - Laythe
     - 4,686.3 km
     - 3,223.6 km
     - no
   * - Tylo
     - 14,157.9 km
     - 10,256.5 km
     - no
   * - Moho
     - 18,173.2 km
     - 9,396.7 km
     - no

Minimum safe target altitudes are listed in :ref:`theory`.
