Application
===========

:Header: ``hpp/Application.hpp``
:Source: ``src/Application.cpp``
:Kind: class
:Depends on: :cpp:class:`IBodyDatabase`, :cpp:class:`IConstellationCalculator`,
             :cpp:class:`IReportFormatter`, :cpp:class:`ITimeFormatter`,
             :cpp:class:`OrbitalMechanics`
:Tests: none (interactive; verified manually)

.. uml:: ../diagrams/classes/Application.puml
   :caption: Application and its file-local helpers

.. cpp:class:: Application

   Top-level orchestrator: runs the interactive loop of body selection,
   altitude selection, calculation, and reporting. It owns no domain logic;
   everything is delegated to injected collaborators.

   .. cpp:function:: Application(std::shared_ptr<IBodyDatabase> db, std::shared_ptr<IConstellationCalculator> calculator, std::shared_ptr<IReportFormatter> formatter, std::shared_ptr<ITimeFormatter> time_formatter)

      Stores the four collaborators. None may be null.

      :param db: body catalogue for the body list, lookups, and child-body
                 menu options
      :param calculator: computes the result for the chosen body and altitude
      :param formatter: prints the result
      :param time_formatter: currently unused by :cpp:class:`Application`

   .. cpp:function:: void run()

      Runs the session until the user declines to continue:

      #. Creates a local :cpp:class:`OrbitalMechanics` for synchronous-orbit
         and SOI queries in the altitude menu.
      #. Prints the banner.
      #. Loops:

         a. ``select_body`` — see :ref:`internals-select-body`.
         b. ``ask_altitude`` — see :ref:`internals-ask-altitude`.
         c. :cpp:func:`IConstellationCalculator::calculate`.
         d. :cpp:func:`IReportFormatter::print`.
         e. Asks ``Calculate for another body/altitude? [y/N]``. The answer is
            lower-cased; ``y`` or ``yes`` repeats the loop.

      #. Prints ``Good luck with the mission!``.

      All I/O uses ``std::printf`` / ``std::getline`` on stdin/stdout.

   .. cpp:member:: private std::shared_ptr<IBodyDatabase> db_

   .. cpp:member:: private std::shared_ptr<IConstellationCalculator> calculator_

   .. cpp:member:: private std::shared_ptr<IReportFormatter> formatter_

   .. cpp:member:: private std::shared_ptr<ITimeFormatter> time_formatter_

      Injected but not read anywhere in ``Application.cpp``.

Behaviour
---------

.. uml:: ../diagrams/state_app.puml
   :caption: Session state machine

.. uml:: ../diagrams/seq_session.puml
   :caption: Collaborator calls during one session

Design notes
------------

* The class is not unit-tested because it reads ``std::cin`` directly. Making
  input and output injectable (``std::istream&`` / ``std::ostream&``) would
  allow scripted tests.
* The local :cpp:class:`OrbitalMechanics` in :cpp:func:`Application::run` bypasses
  dependency injection. Injecting an :cpp:class:`IOrbitalMechanics` instead
  would make every dependency explicit.

See :ref:`limitations` for input-handling edge cases.
