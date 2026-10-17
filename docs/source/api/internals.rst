.. _internals:

Internal Functions
==================

These functions have internal linkage (``static``) in their ``.cpp`` files.
They are not part of the public API but contain much of the interactive
behaviour, so they are documented here.

``main.cpp``
------------

``int main()``
   The composition root. Creates :cpp:class:`KspBodyDatabase`,
   :cpp:class:`OrbitalMechanics`, :cpp:class:`ConstellationCalculator`,
   :cpp:class:`KerbinTimeFormatter`, and :cpp:class:`ConsoleReportFormatter`,
   injects them into :cpp:class:`Application`, calls
   :cpp:func:`Application::run`, and returns ``0``. See :ref:`architecture`.

``Application.cpp``
-------------------

``std::string read_line(const char* prompt)``
   Prints ``prompt``, flushes stdout, reads one line with ``std::getline``,
   and strips trailing whitespace (including ``\r``). Leading whitespace is
   kept. On end-of-file it returns an empty string.

``bool is_digits(const std::string& s)``
   ``true`` when ``s`` is non-empty and every character is a decimal digit.
   Used to decide whether input is a menu number. Signs, spaces, and decimal
   points are rejected.

.. _internals-select-body:

``CelestialBody select_body(const std::shared_ptr<IBodyDatabase>& db)``
   #. Gets :cpp:func:`IBodyDatabase::all` and sorts it by name.
   #. Prints ``NN. Name  R=<km> km[  atm up<km> km]  (orbits <parent> | the star)``
      for each body. The atmosphere height uses ``%g``.
   #. Repeats until valid: if the input is all digits and in range, returns
      that entry; otherwise tries :cpp:func:`IBodyDatabase::find` with the raw
      text; otherwise prints an error.

   .. uml:: ../diagrams/act_select_body.puml
      :caption: select_body

.. _internals-ask-altitude:

``double ask_altitude(const CelestialBody& body, const std::shared_ptr<IBodyDatabase>& db, const std::shared_ptr<IOrbitalMechanics>& mech)``
   Builds and runs the altitude menu, returning the chosen altitude in metres.

   **Precomputation**

   * ``h_sync`` = :cpp:func:`IOrbitalMechanics::sync_orbit_altitude`
   * ``h_soi_max`` = :cpp:func:`IOrbitalMechanics::soi_max_altitude`
   * ``kids`` = :cpp:func:`IBodyDatabase::children_of`
   * ``cword`` = ``"moon"`` if the body has a parent, ``"planet"`` otherwise

   **Menu entries** (label → internal action)

   .. list-table::
      :header-rows: 1

      * - Label
        - Action
        - Condition
      * - ``Synchronous orbit  ->  <tag>``
        - ``sync``
        - always
      * - ``At a <cword>'s orbital distance``
        - ``child_single``
        - ``kids`` non-empty
      * - ``Midpoint between two <cword>s``
        - ``child_mid``
        - ``kids.size() >= 2``
      * - ``Custom altitude``
        - ``custom``
        - always

   The sync ``<tag>`` is ``<h> km``, ``<h> km  (exceeds SOI limit <soi> km)``,
   or ``below surface / unavailable``.

   **Actions**

   ``sync``
      Rejected (back to the menu) if ``h_sync`` is empty or above
      ``h_soi_max``; otherwise returns ``h_sync``. No atmosphere check.

   ``child_single``
      Lists children with altitude ``orbital_radius − R``, reads a number,
      and returns that child's orbital altitude.

   ``child_mid``
      Reads two different child numbers and returns
      ``(r_a + r_b) / 2 − R``.

   ``custom``
      Reads kilometres and validates them (number, > 0, outside the
      atmosphere, inside the SOI) before returning metres.

   .. uml:: ../diagrams/act_ask_altitude.puml
      :caption: ask_altitude

   .. uml:: ../diagrams/act_custom_altitude.puml
      :caption: Custom altitude validation

``ConsoleReportFormatter.cpp``
------------------------------

``W``, ``row``, ``fmt_km``, ``fmt_ms``
   Layout and number-formatting helpers; see
   :cpp:class:`ConsoleReportFormatter`.
